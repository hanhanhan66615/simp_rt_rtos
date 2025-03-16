#include "rtconfig.h"

/* 中断计数器,用于记录中断嵌套次数 */
volatile rt_uint8_t rt_interrupt_nest;          /* volatile: 系统总是在他所在内存读取数据并且不会进行编译优化 */

/* 
 中断服务函数进入时会调用该函数 
 */
void rt_interrupt_enter(void)
{
    rt_base_t level;

    /* 关中断 */
    level = rt_hw_interrupt_disable();

    /* 中断计数器++ */
    rt_interrupt_nest ++;

    /* 开中断 */
    rt_hw_interrupt_enable(level);
}

/* 
 中断服务函数离开时会调用该函数 
 */
void rt_interrupt_leave(void)
{
    rt_base_t level;

    /* 关中断 */
    level = rt_hw_interrupt_disable();

    /* 中断计数器++ */
    rt_interrupt_nest --;

    /* 开中断 */
    rt_hw_interrupt_enable(level);
}