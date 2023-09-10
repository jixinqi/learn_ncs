/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
    (BT_LE_ADV_OPT_CONNECTABLE|BT_LE_ADV_OPT_USE_IDENTITY), 
    32,
    32,
    NULL
);
const unsigned char url_data[] = {
    0x17, '/', '/',
    'j', 'i', 'x', 'i', 'n', 'q', 'i', '.', 'c', 'o', 'm'
};

const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};
const struct bt_data sd[] = {
    // BT_DATA(BT_DATA_URI, url_data, sizeof(url_data)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)),
};

int main(void)
{
    int error_code;

    bt_addr_le_t bt_addr;

    error_code = bt_addr_le_from_str("FF:EE:DD:CC:BB:AA", "random", &bt_addr);
    if (error_code)
    {
        printk("Invalid BT address (err %d)\n", error_code);
        return 0;
    }

    error_code = bt_id_create(&bt_addr, NULL);
    if (error_code)
    {
        printk("Creating new ID failed (err %d)\n", error_code);
        return 0;
    }

    error_code = bt_enable(NULL);
    if (error_code)
    {
        // LOG_ERR("Bluetooth init failed (err %d)\n", error_code);
        return 0;
    }

    error_code = bt_le_adv_start(
        adv_param,
        ad, ARRAY_SIZE(ad),
        sd, ARRAY_SIZE(sd)
    );
    if (error_code)
    {
        // LOG_ERR("Advertising failed to start (err %d)\n", error_code);
        return 0;
    }


    for(;;)
    {
    }

    return 0;
}
