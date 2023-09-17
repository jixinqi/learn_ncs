/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/random/rand32.h>

#include <zephyr/debug/thread_analyzer.h>

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

#define PRODUCER_STACKSIZE 2048
#define CONSUMER_STACKSIZE 2048

#define PRODUCER_PRIORITY 5
#define CONSUMER_PRIORITY 4

K_SEM_DEFINE(instance_monitor_sem, 10, 10);

volatile uint32_t available_instance_count = 10;

void producer_thread_handle(void);
void consumer_thread_handle(void);
void get_access();
void release_access();


void producer_thread_handle(void)
{
    LOG_INF("Producer thread started.");

    uint64_t time_stamp;
    uint64_t time_delta;

    while(1)
    {
        time_stamp = k_uptime_get();
    
        release_access();

        time_delta = k_uptime_delta(&time_stamp);

        LOG_INF("Producer thread yielding this round in %lld ms.", time_delta);
        k_sleep(K_MSEC(500 + sys_rand32_get() % 10));
    }
}
void consumer_thread_handle(void)
{
    LOG_INF("Consumer thread started.");

    uint64_t time_stamp;
    uint64_t time_delta;

    while(1)
    {
        time_stamp = k_uptime_get();

        get_access();

        time_delta = k_uptime_delta(&time_stamp);

        LOG_INF("Consumer thread yielding this round in %lld ms.", time_delta);
        k_sleep(K_MSEC(sys_rand32_get() % 10));
    }
}


void get_access()
{
    k_sem_take(&instance_monitor_sem, K_FOREVER);

    available_instance_count--;
    LOG_INF("Resource taken and available_instance_count = %d", available_instance_count);
}
void release_access()
{
    available_instance_count++;
    LOG_INF("Resource given and available_instance_count = %d", available_instance_count);

    k_sem_give(&instance_monitor_sem);
}


K_THREAD_DEFINE(
    producer_id, PRODUCER_STACKSIZE, producer_thread_handle, NULL, NULL, NULL,
    PRODUCER_PRIORITY, 0, 0
);
K_THREAD_DEFINE(
    consumer_id, CONSUMER_STACKSIZE, consumer_thread_handle, NULL, NULL, NULL,
    CONSUMER_PRIORITY, 0, 0
);


int main(void)
{
    printk("Hello World! %s\n", CONFIG_BOARD);

    while(1)
    {
        LOG_RAW("\n");
        thread_analyzer_print();
        LOG_RAW("\n");

        k_sleep(K_SECONDS(10));
    }

    return 0;
}
