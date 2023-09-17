/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/rand32.h>

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

#define THREAD0_STACKSIZE 1024
#define THREAD1_STACKSIZE 1024

#define THREAD0_PRIORITY 4
#define THREAD1_PRIORITY 4

K_MUTEX_DEFINE(test_mutex);

void thread0(void);
void thread1(void);
void shared_code_section();

void thread0(void)
{
    LOG_INF("Thread 0 started.");
    while(1)
    {
        shared_code_section();
        k_msleep(sys_rand32_get() % 10);
    }
}
void thread1(void)
{
    LOG_INF("Thread 1 started.");
    while(1)
    {
        shared_code_section();
        k_msleep(sys_rand32_get() % 10);
    }
}
void shared_code_section()
{
    k_mutex_lock(&test_mutex, K_FOREVER);

    printk("<");
    for(uint8_t i = 0; i != 26; i++)
    {
        printk("%c", 'A'+i);
        k_msleep(sys_rand32_get() % 10);
    }
    printk(">");

    printk("\n");

    k_mutex_unlock(&test_mutex);
}

K_THREAD_DEFINE(
    thread0_id, THREAD0_STACKSIZE,
    thread0, NULL, NULL, NULL,
    THREAD0_PRIORITY, 0, 0
);
K_THREAD_DEFINE(
    thread1_id, THREAD1_STACKSIZE,
    thread1, NULL, NULL, NULL,
    THREAD1_PRIORITY, 0, 0
);

int main(void)
{
    LOG_INF("Hello, World!");

    while(1)
    {
        k_sleep(K_MSEC(10));
    }

    return 0;
}
