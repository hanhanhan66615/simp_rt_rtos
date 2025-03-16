#include "schedule.h"

/* 线程就绪列表 */
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX]; //

extern struct rt_thread idle;
extern struct rt_thread rt_flag1_thread;
extern struct rt_thread rt_flag2_thread;
rt_uint32_t rt_thread_ready_priority_group;
rt_uint32_t rt_current_priority;

struct rt_thread *rt_current_thread;

/* 已知一个结构体里面的成员的地址，反推出该结构体的首地址 */
#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define rt_list_entry(node, type, member) \
    rt_container_of(node, type, member)

/* 调度器初始化 */
void rt_system_scheduler_init(void)
{
#if 0
    register rt_base_t offset;

    /* 线程就绪列表初始化 */
    for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset++)
    {
        rt_list_init(&rt_thread_priority_table[offset]);
    }

    /* 初始化当前线程控制块指针 */
    rt_current_thread = RT_NULL;
#else
    register rt_base_t offset;

    for(offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset++)
    {
        rt_list_init(&rt_thread_priority_table[offset]);
    }

    /* 初始化当前优先级为空闲线程的优先级 */
    rt_current_priority = RT_THREAD_PRIORITY_MAX - 1;

    /* 初始化当前线程控制块指针 */
    rt_current_thread = RT_NULL;

    /* 初始化线程就绪队列 */
    rt_thread_ready_priority_group = 0;
#endif
}

/* 启动调度器 */
void rt_system_scheduler_start(void)
{
#if 0
    register struct rt_thread *to_thread;

    /* 手动指定第一个运行的线程 */
    to_thread = rt_list_entry(rt_thread_priority_table[0].next,
                              struct rt_thread,
                              tlist);
    rt_current_thread = to_thread;

    /* 切换到第一个线程，该函数在context_rvds.S中实现，在rthw.h声明，
        用于实现第一次任务切换。当一个汇编函数在C文件中调用的时候，
        如果有形参，则执行的时候会将形参传人到CPU寄存器r0。*/
    rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);
#else
    register struct rt_thread *to_thread;
    register rt_ubase_t highest_ready_priority;

    /* 获取就绪的最高优先级 */
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;

    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                             struct rt_thread,
                             tlist);
    rt_current_thread = to_thread;    

    /* 切换到新线程 */
    rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);
#endif
}

void rt_schedule_insert_thread(struct rt_thread *thread)
{
    /* register修饰的变量，如果可能得话应尽量保存在cpu的寄存器中,以加快存储速度 */
    register rt_base_t temp; 

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 改变线程状态 */
    thread->stat = RT_THREAD_READY;

    /* 将线程插入就绪列表 */
    rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                        &(thread->tlist));

    /* 设置线程就绪优先级组中对应的位 */
    rt_thread_ready_priority_group |= thread->number_mask;

    /* 开中断 */

    rt_hw_interrupt_enable(temp);
}

void rt_thread_remove_thread(struct rt_thread *thread)
{
    /* register修饰的变量，如果可能得话应尽量保存在cpu的寄存器中,以加快存储速度 */
    register rt_base_t temp; 

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 将线程从就绪队列中删除 */
    rt_list_remove(&(thread->tlist));

    if(rt_list_isempty(&(rt_thread_priority_table[thread->current_priority])))
        rt_thread_ready_priority_group &= ~thread->number_mask;

    /* 开中断 */
    rt_hw_interrupt_enable(temp);
}

/* 系统调度 */
void rt_schedule(void)
{
#if 0
    struct rt_thread *to_thread;
    struct rt_thread *from_thread;
    
    /* 两个线程轮流切换 */
    // if (rt_current_thread == rt_list_entry(rt_thread_priority_table[0].next,
    //                                        struct rt_thread,
    //                                        tlist))
    // {
    //     from_thread = rt_current_thread;
    //     to_thread = rt_list_entry(rt_thread_priority_table[1].next,
    //                               struct rt_thread,
    //                               tlist);
    //     rt_current_thread = to_thread;
    // }
    // else
    // {
    //     from_thread = rt_current_thread;
    //     to_thread = rt_list_entry(rt_thread_priority_table[0].next,
    //                               struct rt_thread,
    //                               tlist);
    //     rt_current_thread = to_thread;
    // }
    if(rt_current_thread == &idle)
    {
        if(rt_flag1_thread.remaining_tick == 0)
        {
            from_thread = rt_current_thread;
            to_thread = &rt_flag1_thread;
            rt_current_thread = to_thread;
        }
        else if(rt_flag2_thread.remaining_tick == 0)
        {
            from_thread = rt_current_thread;
            to_thread = &rt_flag2_thread;
            rt_current_thread = to_thread;
        }
        else
        {
            return;
        }
    }
    else
    {
        if(rt_current_thread == &rt_flag1_thread)
        {
            if(rt_flag2_thread.remaining_tick == 0)
            {
                from_thread = rt_current_thread;
                to_thread = &rt_flag2_thread;
                rt_current_thread = to_thread;
            }
            else if(rt_current_thread->remaining_tick != 0)
            {
                from_thread = rt_current_thread;
                to_thread = &idle;
                rt_current_thread = to_thread;
            }
            else
            {
                return;
            }
        }
        else if(rt_current_thread == &rt_flag2_thread)
        {
            if(rt_flag1_thread.remaining_tick == 0)
            {
                from_thread = rt_current_thread;
                to_thread = &rt_flag1_thread;
                rt_current_thread = to_thread;
            }
            else if(rt_current_thread->remaining_tick != 0)
            {
                from_thread = rt_current_thread;
                to_thread = &idle;
                rt_current_thread = to_thread;
            }
            else
            {
                return;
            }
        }
    }
    #else
    rt_base_t level;
    register rt_ubase_t highest_ready_priority;
    struct rt_thread *to_thread;
    struct rt_thread *from_thread;

    /* 关中断 */
    level = rt_hw_interrupt_disable();

    /* 获取就绪的最高优先级 */                         
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
    /* 获取就绪的最高优先级对应的线程控制块 */             
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                                struct rt_thread,
                                tlist);

    /* 如果目标线程不是当前线程，则要进行线程切换 */
    if (to_thread != rt_current_thread)              
    {
        rt_current_priority = (rt_uint8_t)highest_ready_priority;
        from_thread         = rt_current_thread;
        rt_current_thread   = to_thread;

        rt_hw_context_switch((rt_uint32_t)&from_thread->sp,
                            (rt_uint32_t)&to_thread->sp);

        /* 开中断 */
        rt_hw_interrupt_enable(level);

    }
    else
    {
        /* 开中断 */
        rt_hw_interrupt_enable(level);
    }
#endif
    /* 产生上下文切换 */
    rt_hw_context_switch((rt_uint32_t)&from_thread->sp, (rt_uint32_t)&to_thread->sp);
}