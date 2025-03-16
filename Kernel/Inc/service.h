#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "rtdef.h"

int __rt_ffs(int value);

/* inline 内联函数只适合简单函数的使用不适合while,switch等函数
 相当于直接复制一份代码在堆内存？
 内联函数避免了调用函数对栈内存的重复开辟
 */
rt_inline void rt_list_init(rt_list_t *l) 
{
    l->next = l->prev = l;
}
rt_inline void rt_list_insert_after(rt_list_t *l, rt_list_t *n)
{
    l->next->prev = n;
    n->next = l->next;
    l->next = n;
    n->prev = l;
}
rt_inline void rt_list_insert_before(rt_list_t *l, rt_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;
    l->prev = n;
    n->next = l; 
}
rt_inline void rt_list_remove(rt_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;
    n->next = n->prev = n;
}

rt_inline rt_uint8_t rt_list_isempty(rt_list_t *n)
{
    if(n->next != n->prev)
        return RT_ERROR;
    
    else 
        return RT_EOK;
}
/**
* 该函数将指定个数的字符串从一个地方拷贝到另外一个地方
*
* @param dst 字符串拷贝的目的地
* @param src 字符串从哪里拷贝
* @param n 要拷贝的最大长度
*
* @return the result
*/
static inline char *rt_strncpy(char *dst, const char *src, rt_ubase_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0); 
    }
    
    return RT_EOK;
}

#endif
