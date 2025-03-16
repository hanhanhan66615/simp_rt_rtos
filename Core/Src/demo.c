#include "demo.h"

rt_uint8_t flag1;
rt_uint8_t flag2;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/* 定义线程控制块 */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;

ALIGN(RT_ALIGN_SIZE)
/* 定义线程栈 */
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
/* 线程声明 */
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);

void rt_rtos_init(void)
{
    /* 关中断 */
    rt_hw_interrupt_disable();

    /* SysTick中断频率设置 */
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND);

    /* 调度器初始化 */
    rt_system_scheduler_init();

    /* 初始化空闲线程 */
    rt_thread_idle_init();

    /* 初始化线程 */
    rt_thread_init(&rt_flag1_thread,               /* 线程控制块 */
                   "task1",
                   flag1_thread_entry,             /* 线程入口地址 */
                   RT_NULL,                        /* 线程形参 */
                   &rt_flag1_thread_stack[0],      /* 线程栈起始地址 */
                   sizeof(rt_flag1_thread_stack),  /* 线程栈大小，单位为字节 */
                   2);                             /* 优先级 */
    /*  将线程插入就绪列表 */
    //rt_list_insert_before(&(rt_thread_priority_table[0]), &(rt_flag1_thread.tlist));
    rt_thread_startup(&rt_flag1_thread);

    /* 初始化线程 */
    rt_thread_init(&rt_flag2_thread,               /* 线程控制块 */
                   "task2",
                   flag2_thread_entry,             /* 线程入口地址 */
                   RT_NULL,                        /* 线程形参 */
                   &rt_flag2_thread_stack[0],      /* 线程栈起始地址 */
                   sizeof(rt_flag2_thread_stack),  /* 线程栈大小，单位为字节 */
                   3);
    /*  将线程插入就绪列表 */
    //rt_list_insert_before(&(rt_thread_priority_table[1]), &(rt_flag2_thread.tlist));
    rt_thread_startup(&rt_flag2_thread);

    /* 启动系统调度器 */
    rt_system_scheduler_start();
}

void flag1_thread_entry(void *p_arg)
{
    (void)p_arg;
    while (1)
    {
    #if 0
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        HAL_Delay(300);

        /* 线程切换 */
        rt_schedule();
    #else
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        rt_thread_delay(100);

        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        rt_thread_delay(100);
    #endif
    }
}

void flag2_thread_entry(void *p_arg)
{
    (void)p_arg;
    while (1)
    {
    #if 0
        HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
        HAL_Delay(300);

        /* 线程切换 */
        rt_schedule();
    #else
        HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
        rt_thread_delay(10);

        HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
        rt_thread_delay(10);
    #endif
    }
}