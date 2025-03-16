#ifndef __THREAD_H__
#define __THREAD_H__

#include "rtdef.h"
#include "schedule.h"
#include "cpuport.h"
#include "service.h"
/* 线程初始化 
 * thread:线程控制块 
 * (*entry)(void *parameter):线程入口地址 
 * parameter:线程形参 
 * stack_start:线程栈起始地址
 * stack_size:线程栈大小 */
rt_err_t rt_thread_init(struct rt_thread *thread,//
                        const char          *name,
                        void (*entry)(void *parameter),//
                        void                *parameter,//
                        void                *stack_start,//
                        rt_uint32_t          stack_size,
                        rt_uint8_t           priority);

/* 阻塞延时 */
void rt_thread_delay(rt_tick_t tick);
rt_thread_t rt_thread_self(void);
rt_err_t rt_thread_startup(rt_thread_t thread);
rt_err_t rt_thread_resume(rt_thread_t thread);

#endif