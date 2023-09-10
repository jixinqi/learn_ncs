/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

int main(void)
{
    const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
        BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
    };

    const unsigned char url_data[] = {
        0x17, '/', '/',
        'j', 'i', 'x', 'i', 'n', 'q', 'i', '.', 'c', 'o', 'm'
    };

    const struct bt_data sd[] = {
        BT_DATA(BT_DATA_URI, url_data,sizeof(url_data))
    };

    {
        int error_code;

        error_code = bt_enable(NULL);
        if (error_code)
        {
            // LOG_ERR("Bluetooth init failed (err %d)\n", error_code);
            return 0;
        }

        error_code = bt_le_adv_start(
            BT_LE_ADV_NCONN,
            ad, ARRAY_SIZE(ad),
            sd, ARRAY_SIZE(sd)
        );
        if (error_code)
        {
            // LOG_ERR("Advertising failed to start (err %d)\n", error_code);
            return 0;
        }
    }

    for(;;)
    {
    }

    return 0;
}
