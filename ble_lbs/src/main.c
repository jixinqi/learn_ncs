#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "devices/devices.h"
#include "bluetooth/bt.h"
#include "bluetooth/lbs_0.h"

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

int main()
{
    int ec = 0;

    ec = init_devices();

    ec = init_bluetooth();

    gpio_pin_set_dt(running_status_led, 1);

    while(1)
    {
        laz_lbs_0_read_button_indicate_cb();
        laz_lbs_0_mysensor_notify();
        k_sleep(K_SECONDS(1));
    }

    while(1)
    {
        k_sleep(K_HOURS(1));
    }

    return 0;
}
