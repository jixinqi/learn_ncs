#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ble_lbs.h"

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

int main()
{
    LOG_INF("Hello World! %s", CONFIG_BOARD);

    init_devices();
    init_bluetooth();

    while(1)
    {
        k_sleep(K_HOURS(1));
    }

    return 0;
}
