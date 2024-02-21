#include "bt.h"

#include "../devices/devices.h"
#include "lbs_0.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(BT, LOG_LEVEL_DBG);

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_LBS_VAL),
};
static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
    (BT_LE_ADV_OPT_CONNECTABLE |
     BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
    800, /* Min Advertising Interval 500ms (800*0.625ms) */
    801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
    NULL /* Set to NULL for undirected advertising */
);

void on_connected(struct bt_conn *conn, uint8_t ec)
{
    if(ec)
    {
        LOG_ERR("on_connected() failed!");
    }
    gpio_pin_set_dt(bt_conn_status_led, 1);
}
void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    gpio_pin_set_dt(bt_conn_status_led, 0);
}

struct bt_conn_cb connection_callbacks = {
    .connected    = on_connected,
    .disconnected = on_disconnected,
};

int init_bluetooth()
{
    int ec = 0;
    
    on_disconnected(NULL, 0);
    bt_enable(NULL);
    bt_conn_cb_register(&connection_callbacks);
    bt_le_adv_start(
        adv_param,
        ad, ARRAY_SIZE(ad),
        sd, ARRAY_SIZE(sd)
    );

    // Debug
    // Print all attrs uuid
    for
    (
        int attrs_index = 0;
        attrs_index != LAZ_LBS_0.attr_count;
        attrs_index++
    )
    {
        char uuid_str_buf[64];
        bt_uuid_to_str(LAZ_LBS_0.attrs[attrs_index].uuid, uuid_str_buf, sizeof(uuid_str_buf));
        LOG_INF("attr[%d].uuid: %s", attrs_index, uuid_str_buf);
    }

    return ec;
}
