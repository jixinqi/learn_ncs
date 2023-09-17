#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include <string.h>

#define STACKSIZE 1024
#define THREAD0_PRIORITY 2
#define THREAD1_PRIORITY 3
#define WORK_PRIORITY    4

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_DBG);

static K_THREAD_STACK_DEFINE(my_stack_area, STACKSIZE);
static struct k_work_q offload_work_q = {0};

static inline void emulate_work()
{
    LOG_INF("emulate_work() start.\n");
    for(volatile int count_out = 0; count_out < 150000; count_out ++);
    LOG_INF("emulate_work() end.\n");

}

struct work_info
{
    struct k_work work;
    char name[25];
};
struct work_info my_work;

void offload_function(struct k_work *work_tem)
{
    emulate_work();
}

void thread0()
{
    uint64_t time_stamp;
    uint64_t time_delta;

    k_work_queue_start(
        &offload_work_q,
        my_stack_area,
        K_THREAD_STACK_SIZEOF(my_stack_area),
        WORK_PRIORITY,
        NULL
    );
    strcpy(my_work.name, "Thread0 emulate_work()");
    k_work_init(&my_work.work, offload_function);

    for(;;)
    {
        time_stamp = k_uptime_get();
        // emulate_work();
        printk("Call k_work_submit_to_queue(&offload_work_q, &my_work.work).\r\n");
        int ec;
        ec = k_work_submit_to_queue(&offload_work_q, &my_work.work);
        printk("ec = %d\n", ec);
        
        time_delta = k_uptime_delta(&time_stamp);

        printk("thread0 yielding this round in %lld ms\n", time_delta);
        k_msleep(1);
    }
}
void thread1()
{
    uint64_t time_stamp;
    uint64_t time_delta;

    for(;;)
    {
        time_stamp = k_uptime_get();
        emulate_work();
        time_delta = k_uptime_delta(&time_stamp);

        printk("thread1 yielding this round in %lld ms\n", time_delta);
        k_msleep(20);
    }
}

K_THREAD_DEFINE(thread0_id, STACKSIZE, thread0, NULL, NULL, NULL, THREAD0_PRIORITY, 0, 10 * 1000);
K_THREAD_DEFINE(thread1_id, STACKSIZE, thread1, NULL, NULL, NULL, THREAD1_PRIORITY, 0, 10 * 1000);

int main(void)
{
    LOG_INF("Hello, World.");

    while(1)
    {
        k_sleep(K_MSEC(100));
    }

    return 0;
}
