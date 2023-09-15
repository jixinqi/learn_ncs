#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define I2C0_NODE DT_NODELABEL(myoled)

const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

int init_devices()
{
    if(!device_is_ready(dev_i2c.bus))
    {
        LOG_ERR("I2C bus %s is not ready!\r\n", dev_i2c.bus->name);
        return -1;
    }
    return 0;
}

int main(void)
{
    int ec; // error_code
    ec = init_devices();
    if (ec)
    {
        return -1;
    }

    uint8_t oled_init_commands[] = {
        0xAE, 0x20, 0x10, 0xb0,   0xc8, 0x00, 0x10, 0x40,
        0x81, 0xff, 0xa1, 0xa6,   0xa8, 0x3F, 0xa4, 0xd3,
        0x00, 0xd5, 0xf0, 0xd9,   0x22, 0xda, 0x12, 0xdb,
        0x20, 0x8d, 0x14, 0xaf,
    };

    for(int i = 0; i != sizeof(oled_init_commands)/sizeof(uint8_t); i++)
    {
        uint8_t i2c_write_buffer[] = { 0x00, oled_init_commands[i] };
        ec = i2c_write_dt(&dev_i2c, i2c_write_buffer, sizeof(i2c_write_buffer));
    }

    for(int i = 0; i != 128 * 8; i++)
    {
        uint8_t i2c_write_buffer[] = { 0xC0, 0x00 };
        ec = i2c_write_dt(&dev_i2c, i2c_write_buffer, sizeof(i2c_write_buffer));
    }

    for(int i = 0; i != 128 * 8; i++)
    {
        uint8_t i2c_write_buffer[] = { 0xC0, 0x00 };
        if(i % 8 > 2 && i % 8 < 7)
        {
            i2c_write_buffer[1] = 0x3C;
        }
        ec = i2c_write_dt(&dev_i2c, i2c_write_buffer, sizeof(i2c_write_buffer));
    }

    while(1)
    {
        LOG_INF("while(1)");
        k_sleep(K_MINUTES(1));
    }

    return 0;
}
