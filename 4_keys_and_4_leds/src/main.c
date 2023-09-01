/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios)
};
struct gpio_dt_spec keys[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(key0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(key1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(key2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(key3), gpios)
};


void pin_isr(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins)
{
    for(uint16_t i = 0; i != sizeof(keys) / sizeof(struct gpio_dt_spec); i++)
    {
        const struct gpio_dt_spec* key_ptr = &(keys[i]);
        if(pins & BIT(key_ptr->pin))
        {
            gpio_pin_toggle_dt(&(leds[i]));
        }
    }
}

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


int main(void)
{
    printk("Hello World! %s\n", CONFIG_BOARD);

    if(init_devices() != 0) // error_code: 0 is success
    {
        return 0;
    }

    struct gpio_callback pin_cb_data;
    gpio_init_callback(&pin_cb_data, pin_isr, UINT32_MAX);

    for(uint16_t i = 0; i != sizeof(keys) / sizeof(struct gpio_dt_spec); i++)
    {
        const struct gpio_dt_spec* key_ptr = &(keys[i]);
        gpio_pin_interrupt_configure_dt(key_ptr, GPIO_INT_EDGE_TO_INACTIVE);
        gpio_add_callback_dt(key_ptr, &pin_cb_data);
    }

    while(1)
    {
    }

    return 0;
}
