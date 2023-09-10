#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

int init_devices();
void pin_isr(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins);

#define COMPANY_ID_CODE            0x0059
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

struct gpio_dt_spec leds0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
struct gpio_dt_spec sw0   = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

struct adv_mfg_data
{
    uint16_t comapny_code;
    uint16_t number_press;
};
static struct adv_mfg_data mfg_data = {
    COMPANY_ID_CODE,
    0
};



const struct bt_le_adv_param* adv_param = 
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_NONE, 32, 32, NULL);

const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, (unsigned char*)&mfg_data, sizeof(mfg_data))
};

const unsigned char url_data[] = {
    0x17, '/', '/',
    'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '/'
};
const struct bt_data sd[] = {
    BT_DATA(BT_DATA_URI, url_data, sizeof(url_data))
};


int init_devices()
{
    // check if device ready
    if(!device_is_ready(leds0.port)) { return 1; }
    if(!device_is_ready(sw0.port))   { return 1; }

    // configure led and keys
    gpio_pin_configure_dt(&leds0, (GPIO_OUTPUT_INACTIVE | GPIO_PULL_UP));
    gpio_pin_configure_dt(&sw0,   (GPIO_INPUT           | GPIO_PULL_UP));

    return 0;
}

void pin_isr(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins)
{
    gpio_pin_toggle_dt(&leds0);
    mfg_data.number_press++;

    printk("%d\n", mfg_data.number_press);

    #if 0
    bt_le_adv_update_data(
        ad, ARRAY_SIZE(ad),
        sd, ARRAY_SIZE(sd)
    );
    #endif
}


int main(void)
{
    printk("Hello, World!\n");

    int error_code;

    error_code = init_devices();
    if (error_code)
    {
        // log
        return 0;
    }

    // set interrupt callback
    struct gpio_callback pin_cb_data;
    gpio_init_callback(&pin_cb_data, pin_isr, UINT32_MAX);
    gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_TO_INACTIVE);
    gpio_add_callback_dt(&sw0, &pin_cb_data);

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
        #if 1
        bt_le_adv_update_data(
            ad, ARRAY_SIZE(ad),
            sd, ARRAY_SIZE(sd)
        );
        #endif
        k_sleep(K_MSEC(10));
    }

    return 0;
}
