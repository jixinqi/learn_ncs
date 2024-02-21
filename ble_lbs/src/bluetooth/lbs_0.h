#ifndef _LBS_0_H_
#define _LBS_0_H_

#include "bt.h"
#include "../devices/devices.h"

extern uint8_t laz_lbs_0_read_button_ccc_cfg;
extern uint8_t laz_lbs_0_mysensor_ccc_cfg;

extern const struct bt_gatt_service_static LAZ_LBS_0;

int laz_lbs_0_read_button_indicate_cb();
int laz_lbs_0_mysensor_notify();

#endif
