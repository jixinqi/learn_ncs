#include "lbs_0.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LBS_0, LOG_LEVEL_DBG);

uint8_t laz_lbs_0_read_button_ccc_cfg = 0;
uint8_t laz_lbs_0_mysensor_ccc_cfg = 0;
struct bt_gatt_indicate_params laz_lbs_0_read_button_indicate_params;
char k_uptime_str[16] = { 0 };

static ssize_t laz_lbs_0_read_button_cb(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    void *buf,
    uint16_t len,
    uint16_t offset
)
{
    lbs_0_key_value = gpio_pin_get_dt(lbs_0_key);
    LOG_INF("laz_lbs_0_read_button: %d", lbs_0_key_value);
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &lbs_0_key_value, sizeof(lbs_0_key_value));
}

static void laz_lbs_0_read_button_ccc_cfg_changed_cb(
    const struct bt_gatt_attr *attr,
    uint16_t value
)
{
    laz_lbs_0_read_button_ccc_cfg = value;
    LOG_INF("laz_lbs_0_read_button_ccc_cfg: %d", laz_lbs_0_read_button_ccc_cfg);
}

int laz_lbs_0_read_button_indicate_cb()
{
    if(!(laz_lbs_0_read_button_ccc_cfg & BT_GATT_CCC_INDICATE))
    {
        return -EACCES;
    }

    lbs_0_key_value = gpio_pin_get_dt(lbs_0_key);

    laz_lbs_0_read_button_indicate_params.attr = &LAZ_LBS_0.attrs[2];
    laz_lbs_0_read_button_indicate_params.func = NULL;
    laz_lbs_0_read_button_indicate_params.destroy = NULL;
    laz_lbs_0_read_button_indicate_params.data = &lbs_0_key_value;
    laz_lbs_0_read_button_indicate_params.len = sizeof(lbs_0_key_value);
    return bt_gatt_indicate(NULL, &laz_lbs_0_read_button_indicate_params);
}

static void laz_lbs_0_read_button_mysensor_ccc_cfg_changed(
    const struct bt_gatt_attr *attr,
    uint16_t value
)
{
    laz_lbs_0_mysensor_ccc_cfg = value;
    LOG_INF("laz_lbs_0_mysensor_ccc_cfg: %d", laz_lbs_0_mysensor_ccc_cfg);
}

int laz_lbs_0_mysensor_notify()
{
    if(!(laz_lbs_0_mysensor_ccc_cfg & BT_GATT_CCC_NOTIFY))
    {
        return -EACCES;
    }

    sprintf(k_uptime_str, "%llu", k_uptime_get());

    return bt_gatt_notify(NULL, &LAZ_LBS_0.attrs[5], &k_uptime_str, sizeof(k_uptime_str));
}

BT_GATT_SERVICE_DEFINE(LAZ_LBS_0,

    BT_GATT_PRIMARY_SERVICE(BT_UUID_LBS),

    BT_GATT_CHARACTERISTIC(
        BT_UUID_LBS_BUTTON,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE,
        BT_GATT_PERM_READ,
        laz_lbs_0_read_button_cb, NULL, NULL
    ),
    BT_GATT_CCC(
        laz_lbs_0_read_button_ccc_cfg_changed_cb,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
    ),

    BT_GATT_CHARACTERISTIC(BT_UUID_LBS_MYSENSOR,
        BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_NONE,
        NULL, NULL, NULL
    ),
    BT_GATT_CCC(laz_lbs_0_read_button_mysensor_ccc_cfg_changed,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
    ),

);
