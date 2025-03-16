#ifndef __DEMO_H__
#define __DEMO_H__

#include "rtconfig.h"
#include "main.h"

void rt_rtos_init(void);
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);

#endif