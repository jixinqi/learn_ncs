#include <stdio.h>
#include <stdlib.h>

#include <zephyr/net/net_ip.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

#define I2C0_NODE DT_NODELABEL(gy302)

static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

int main(void)
{
    if(!device_is_ready(dev_i2c.bus))
    {
        LOG_ERR("I2C bus %s is not ready!\r\n", dev_i2c.bus->name);
        return -1;
    }
    
    int ec = 0;
    char iic_write_buffer[1] = { 0 };
    char iic_read_buffer[2] = { 0 };

    // init gy302
    iic_write_buffer[0] = 0x01;
    ec = i2c_write_dt(&dev_i2c, iic_write_buffer, 1);
    // LOG_DBG("init ec = %d", ec);

    for(;;)
    {
        memset(iic_write_buffer, 0, sizeof(iic_write_buffer));
        memset(iic_read_buffer, 0, sizeof(iic_read_buffer));
        
        iic_write_buffer[0] = 0x11;

        ec = i2c_write_read_dt(
            &dev_i2c,
            iic_write_buffer, 1,
            iic_read_buffer, 2
        );
        // LOG_DBG("get info ec = %d", ec);

        uint16_t brightness_value;
        memcpy(&brightness_value, iic_read_buffer, sizeof(brightness_value));
        brightness_value = ntohs(brightness_value);

        // LOG_HEXDUMP_INF(iic_read_buffer, sizeof(iic_read_buffer), "iic_read_buffer");
        LOG_INF("brightness_value is : %d", brightness_value);
        
        k_sleep(K_MSEC(1000 * 2));
    }

    while(1)
    {
        k_sleep(K_MSEC(10));
    }

    return 0;
}
