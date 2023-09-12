/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

#include <zephyr/drivers/gpio.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>


LOG_MODULE_REGISTER(Lesson3_Exercise1, LOG_LEVEL_INF);

struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios)
};
struct gpio_dt_spec keys[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios)
};

struct bt_conn *my_conn = NULL;

#define DEVICE_NAME "JIXINQI"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

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
        gpio_pin_configure_dt(led_ptr, (GPIO_OUTPUT_HIGH | GPIO_ACTIVE_LOW));
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

void on_connected(struct bt_conn *conn, uint8_t ec)
{
    if(ec)
    {
        // LOG_ERR("Connection error %d", ec);
        return;
    }
    LOG_INF("Connected.");
    my_conn = bt_conn_ref(conn);

    for(uint16_t i = 0; i != sizeof(leds) / sizeof(struct gpio_dt_spec); i++)
    {
        gpio_pin_set_dt(&(leds[i]), 1);
    }
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    for(uint16_t i = 0; i != sizeof(leds) / sizeof(struct gpio_dt_spec); i++)
    {
        gpio_pin_set_dt(&(leds[i]), 0);
    }

    // LOG_INF("Disonnected. Reason %d.", reason);
    bt_conn_unref(my_conn);


}

static struct bt_le_adv_param *adv_param = 
    BT_LE_ADV_PARAM(
        (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY),
        BT_GAP_ADV_FAST_INT_MIN_1,
        BT_GAP_ADV_FAST_INT_MAX_1,
        NULL
    );

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN)
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(
        BT_DATA_UUID128_ALL,
        BT_UUID_128_ENCODE(
            0x00001523,
            0x1212,
            0xefde,
            0x1523,
            0x785feabcd123
        )
    )
};

struct bt_conn_cb connection_callbacks = {
    .connected    = on_connected,
    .disconnected = on_disconnected
};

int main(void)
{
    init_devices();
    // error code
    int ec;

    ec = bt_enable(NULL);

    ec = bt_le_adv_start(
        adv_param,
        ad, ARRAY_SIZE(ad),
        sd, ARRAY_SIZE(sd)
    );

    bt_conn_cb_register(&connection_callbacks);

    for(;;)
    {
    }

    return 0;
}
