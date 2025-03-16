#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "rtdef.h"
#include "rtthread_cfg.h"
#include "service.h"
#include "cpuport.h"

extern void rt_hw_context_switch_to(rt_uint32_t to);
extern void rt_hw_context_switch(rt_uint32_t from, rt_uint32_t to);
extern rt_uint8_t rt_hw_interrupt_disable();
extern void rt_hw_interrupt_enable();

void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);
void rt_schedule(void);
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_thread_remove_thread(struct rt_thread *thread);

#endif