#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

extern const struct gpio_dt_spec* running_status_led;
extern const struct gpio_dt_spec* bt_conn_status_led;

extern const struct gpio_dt_spec* lbs_0_led;
extern const struct gpio_dt_spec* lbs_0_key;
extern const struct gpio_dt_spec* lbs_1_led;
extern const struct gpio_dt_spec* lbs_1_key;

extern uint8_t lbs_0_led_value;
extern uint8_t lbs_0_key_value;
extern uint8_t lbs_1_led_value;
extern uint8_t lbs_1_key_value;

int init_devices();

#endif // _DEVICES_H_
