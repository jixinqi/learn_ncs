/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

static void uart_cb(
    const struct device *dev,
    struct uart_event *evt,
    void *user_data
)
{
    if (evt->type == UART_TX_DONE)
    {
    }
    else if (evt->type == UART_TX_ABORTED)
    {
    }
    else if (evt->type == UART_RX_RDY)
    {
    }
    else if (evt->type == UART_RX_BUF_REQUEST)
    {
    }
    else if (evt->type == UART_RX_BUF_RELEASED)
    {
    }
    else if (evt->type == UART_RX_DISABLED)
    {
    }
    else if (evt->type == UART_RX_STOPPED)
    {
    }
    else
    {
    }
}

int main(void)
{
    printk("Hello World! %s %s %s\n", CONFIG_BOARD, __DATE__, __TIME__);

    const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
    if (!device_is_ready(uart)) {
        return 0;
    }

    struct uart_config uart_cfg = {
        .baudrate  = 115200,
        .parity    = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    };
    uart_configure(uart, &uart_cfg);
    // uart_callback_set(uart, uart_cb, NULL);
    // uart_irq_tx_enable(uart);

    char send_msg[] = "Hello, World!";
    char send_buf[128];
    for(uint32_t i = 0; i != 1000; i++)
    {
        // uint32_t time = sys_clock_cycle_get_32();
        sprintf(send_buf, "index:[%d] msg: %s\r\n", i, send_msg);
        int ret = INT32_MAX;
        while(ret != 0)
        {
            ret = uart_tx(uart, send_buf, strlen(send_buf), SYS_FOREVER_US);
        }
    }

    while(1)
    {
    }

    return 0;
}
