/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
static unsigned char rx_buffer[512] = { 0 };
static unsigned char tx_buffer[512] = { 0 };

int init_device()
{
    if (!device_is_ready(uart)) {
        return -1;
    }

    struct uart_config uart_cfg =
    {
        .baudrate  = 115200,
        .parity    = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE
    };
    uart_configure(uart, &uart_cfg);

    return 0;
}

static void uart_cb(
    const struct device *dev,
    struct uart_event *evt,
    void *user_data
)
{
    if(evt->type == UART_TX_DONE)
    {
        LOG_INF("evt->type == UART_TX_DONE");
    }
    else if(evt->type == UART_TX_ABORTED)
    {
        LOG_INF("evt->type == UART_TX_ABORTED");
    }
    else if(evt->type == UART_RX_RDY)
    {
        LOG_INF("evt->type == UART_RX_RDY");
        uint8_t recv_data[512] = { 0 };
        memcpy(recv_data, rx_buffer + evt->data.rx.offset, evt->data.rx.len);
        LOG_INF("uart_rx: %s", recv_data);
    }
    else if(evt->type == UART_RX_BUF_REQUEST)
    {
        LOG_INF("evt->type == UART_RX_BUF_REQUEST");
        uart_rx_buf_rsp(dev, rx_buffer, sizeof(rx_buffer));
    }
    else if(evt->type == UART_RX_BUF_RELEASED)
    {
        LOG_INF("evt->type == UART_RX_BUF_RELEASED");
    }
    else if(evt->type == UART_RX_DISABLED)
    {
        LOG_INF("evt->type == UART_RX_DISABLED");
    }
    else if(evt->type == UART_RX_STOPPED)
    {
        LOG_INF("evt->type == UART_RX_STOPPED");
    }
}

int main(void)
{
    init_device();

    uart_callback_set(uart, uart_cb, NULL);

    uart_rx_enable(uart, rx_buffer, sizeof(rx_buffer), 0xFF);

    sprintf(tx_buffer, "AT...\r\n");
    uart_tx(uart, tx_buffer, strlen(tx_buffer), SYS_FOREVER_US);
    LOG_INF("uart_tx: %s", tx_buffer);
    k_sleep(K_MSEC(500+1));

    sprintf(tx_buffer, "AT+HELLO?\r\n");
    uart_tx(uart, tx_buffer, strlen(tx_buffer), SYS_FOREVER_US);
    LOG_INF("uart_tx: %s", tx_buffer);
    k_sleep(K_MSEC(500+1));

    sprintf(tx_buffer, "AT+UART?\r\n");
    uart_tx(uart, tx_buffer, strlen(tx_buffer), SYS_FOREVER_US);
    LOG_INF("uart_tx: %s", tx_buffer);
    k_sleep(K_MSEC(500+1));

    while(1)
    {
        k_sleep(K_MINUTES(5));
        LOG_INF("while(1)");
    }

    return 0;
}
