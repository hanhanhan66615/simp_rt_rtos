    .cpu    cortex-m3
    .fpu    softvfp
    .syntax unified
    .thumb                          /* 设置为Thumb模式 */
    .align 2                        /* 对齐到4字节边界 */

    .equ    SCB_VTOR, 0xE000ED08            /* 向量表偏移寄存器 */
    .equ    NVIC_INT_CTRL, 0xE000ED04       /* 中断控制状态寄存器 */
    .equ    NVIC_SYSPRI2, 0xE000ED20        /* 系统优先级寄存器（2） */
    .equ    NVIC_PENDSV_PRI, 0x00FF0000     /* PendSV 优先级值（lowest） */
    .equ    NVIC_PENDSVSET, 0x10000000      /* 触发PendSV exception的值 */

/* 函数原型：void rt_hw_context_switch_to(rt_uint32 to) 
 * r0 --> to
 * 该函数用于开启第一次线程切换 
 */
.global rt_hw_context_switch_to
.type rt_hw_context_switch_to, %function
rt_hw_context_switch_to:
    LDR     r1, =rt_interrupt_to_thread
    STR     r0, [r1]

    LDR     r1, =rt_interrupt_from_thread
    MOV     r0, #0x0
    STR     r0, [r1]

    LDR     r1, =rt_thread_switch_interrupt_flag
    MOV     r0, #1
    STR     r0, [r1]

    /* 设置PendSV异常优先级 */
    LDR     r0, =NVIC_SYSPRI2
    LDR     r1, =NVIC_PENDSV_PRI
    LDR     r2, [r0,#0x00]
    ORR     r1,r1,r2
    STR     r1,[r0]

    /* 触发PendSV异常优先级 */
    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    /* 开中断 */
    CPSIE   F
    CPSIE   I

    BX      LR
    .align  8

/* 
 * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to)
 * r0 --> from
 * r1 --> to 
 */
.global rt_hw_context_switch
.type rt_hw_context_switch, %function
rt_hw_context_switch:
    LDR     r2, =rt_thread_switch_interrupt_flag
    LDR     r3, [r2]

    CMP     r3, #1          /* r3与1比较，相等执行BEQ指令，否则不执行 */
    BEQ     _reswitch 

    MOV     r3, #1
    STR     r3, [r2]

    LDR     r2, =rt_interrupt_from_thread
    STR     r0, [r2]

_reswitch:
    LDR     r2, =rt_interrupt_to_thread
    STR     r1, [r2]

    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET         /* 设置PendSV异常标志 */
    STR     r1, [r0]                    /* 出发PendSV异常 */

    BX      LR

/* 
 * void PendSV_Handler(void);
 * r0 --> switch from thread stack
 * r1 --> switch to thread stack
 * psr,pc,lr,r12,r3,r2,r1,r0 are pushed into [from] stack
 */
.global PendSV_Handler
.type PendSV_Handler, %function
PendSV_Handler:
    /* 失能中断，为了保护上下文不被中断 */
    MRS     r2, PRIMASK     /* 将当前中断的使能状态（PRIMASK寄存器的值）保存到寄存器R2中 */
    CPSID   I               /* 禁用中断，确保上下文切换过程不会被其他中断打断 */

    /* 获取中断标志位 */
    LDR     r0, =rt_thread_switch_interrupt_flag
    LDR     r1, [r0]
    CBZ     r1, pendsv_exit /* 判断r1是否为0，为0则跳转到pendsv_exit */

    /* 清除中断标志位 */
    MOV     r1, #0x00
    STR     r1, [r0]   

    LDR     r0, =rt_interrupt_from_thread
    LDR     r1, [r0]
    CBZ     r1, switch_to_thread

    /* 当进入PendSVC Handler时，上一个线程的运行环境即：xPSR,PC(线程入口地址)，R14,R12,R3,R2,R1,R0（线程的形参）
    这些CPU寄存器的值会自动保存到线程的栈中，剩下的r4~r11需要手动保存 */
    MRS     r1, psp
    STMFD   r1!, {r4 - r11}
    LDR     r0, [r0]
    STR     r1, [r0]

switch_to_thread:
    LDR     r1, =rt_interrupt_to_thread
    LDR     r1, [r1]
    LDR     r1, [r1]

    LDMFD   r1!, {r4 - r11}
    MSR     psp, r1

pendsv_exit:
    /* 恢复中断 */
    MSR     PRIMASK, r2

    ORR     lr, lr, #0x04
    BX      lr

/* 
 rt_base_t rt_hw_interrupt_disable();
 */
.global rt_hw_interrupt_disable
.type rt_hw_interrupt_disable, %function
rt_hw_interrupt_disable:
    MRS     r0, PRIMASK         // 将PRIMASK寄存器的值读取到r0
    CPSID   i                   // 禁用中断
    BX      lr                  // 返回

/*
 rt_hw_interrupt_enable(rt_base_t level)
 */
.global rt_hw_interrupt_enable
.type rt_hw_interrupt_enable, %function
rt_hw_interrupt_enable:
    MSR     PRIMASK, r0
    BX      lr