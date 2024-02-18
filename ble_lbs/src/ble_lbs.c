#include "ble_lbs.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(BLE_LBS, LOG_LEVEL_DBG);

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios)
};
const struct gpio_dt_spec keys[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios)
};

const struct gpio_dt_spec* conn_status_led = &(leds[0]);

const struct gpio_dt_spec* lbs_led = &(leds[1]);
const struct gpio_dt_spec* lbs_key = &(keys[1]);

int init_devices()
{
    for(uint16_t i = 0; i != sizeof(leds) / sizeof(struct gpio_dt_spec); i++)
    {
        const struct gpio_dt_spec* led_ptr = &(leds[i]);
        if(!device_is_ready(led_ptr->port))
        {
            printk("device_is_ready() failed! led[%d]\n", i);
            return 1;
        }
        gpio_pin_configure_dt(led_ptr, (GPIO_OUTPUT_INACTIVE | GPIO_PULL_UP));
    }
    for(uint16_t i = 0; i != sizeof(keys) / sizeof(struct gpio_dt_spec); i++)
    {
        const struct gpio_dt_spec* key_ptr = &(keys[i]);
        if(!device_is_ready(key_ptr->port))
        {
            printk("device_is_ready() failed! key[%d]\n", i);
            return 1;
        }
        gpio_pin_configure_dt(key_ptr, (GPIO_INPUT | GPIO_PULL_UP));
    }
    return 0;
}

static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
    (BT_LE_ADV_OPT_CONNECTABLE |
     BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
    800, /* Min Advertising Interval 500ms (800*0.625ms) */
    801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
    NULL); /* Set to NULL for undirected advertising */

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_LBS_VAL),
};

void on_connected(struct bt_conn *conn, uint8_t ec)
{
    if(ec)
    {
        LOG_ERR("on_connected() failed!");
    }
    gpio_pin_set_dt(conn_status_led, 1);
}
void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    gpio_pin_set_dt(conn_status_led, 0);
}

struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected,
};

int init_bluetooth()
{
    on_disconnected(NULL, 0);
    bt_enable(NULL);
    bt_conn_cb_register(&connection_callbacks);
    bt_le_adv_start(
        adv_param,
        ad, ARRAY_SIZE(ad),
        sd, ARRAY_SIZE(sd)
    );
    return 0;
}

static ssize_t read_button(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    void *buf,
    uint16_t len,
    uint16_t offset
)
{
    uint8_t read_button_value = 0;
    read_button_value = gpio_pin_get_dt(lbs_key);
    LOG_INF("read_button: %d", read_button_value);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &read_button_value, sizeof(read_button_value));
}

static ssize_t write_led(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    const void *buf,
    uint16_t len,
    uint16_t offset,
    uint8_t flags
)
{
    if (len != 1U)
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

	if (offset != 0) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

    uint8_t val = *((uint8_t *)buf);

    LOG_INF("write_led: %d", val);

    if(val)
    {
        gpio_pin_set_dt(lbs_led, 1);
    }
    else
    {
        gpio_pin_set_dt(lbs_led, 0);
    }

    return len;
}

BT_GATT_SERVICE_DEFINE(LAZ_LBS,

    BT_GATT_PRIMARY_SERVICE(BT_UUID_LBS),

    BT_GATT_CHARACTERISTIC(
        BT_UUID_LBS_BUTTON,
        BT_GATT_CHRC_READ,
        BT_GATT_PERM_READ,
        read_button, NULL, NULL
    ),

    BT_GATT_CHARACTERISTIC(
        BT_UUID_LBS_LED,
        BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_WRITE,
        NULL, write_led, NULL
    ),
);
