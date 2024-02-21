#include "devices.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(BLE_LBS, LOG_LEVEL_DBG);

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

const struct gpio_dt_spec* running_status_led = &(leds[0]);
const struct gpio_dt_spec* bt_conn_status_led = &(leds[1]);

const struct gpio_dt_spec* lbs_0_led = &(leds[2]);
const struct gpio_dt_spec* lbs_0_key = &(keys[2]);
const struct gpio_dt_spec* lbs_1_led = &(leds[3]);
const struct gpio_dt_spec* lbs_1_key = &(keys[3]);

uint8_t lbs_0_led_value = 0;
uint8_t lbs_0_key_value = 0;
uint8_t lbs_1_led_value = 0;
uint8_t lbs_1_key_value = 0;

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
