#include "rtconfig.h"

#define _OBJ_CONTAINER_LIST_INIT(c) \
{&(rt_object_container[c].object_list),&(rt_object_container[c].object_list)}

static struct rt_object_information                      
rt_object_container[RT_Object_Info_Unknown] =          
{
    /* 初始化对象容器 - 线程 */                           
    {
        RT_Object_Class_Thread,                        
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Thread),                            
        sizeof(struct rt_thread)                        
    },

#ifdef RT_USING_SEMAPHORE                            
    /* 初始化对象容器 - 信号量 */
    {
        RT_Object_Class_Semaphore,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Semaphore),
        sizeof(struct rt_semaphore)
    },
#endif

#ifdef RT_USING_MUTEX                                  
    /* 初始化对象容器 - 互斥量 */
    {
        RT_Object_Class_Mutex,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Mutex),
        sizeof(struct rt_mutex)
    },
#endif

#ifdef RT_USING_EVENT                                 
    /* 初始化对象容器 - 事件 */
    {
        RT_Object_Class_Event,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Event),
        sizeof(struct rt_event)
    },
#endif

#ifdef RT_USING_MAILBOX                                 
    /* 初始化对象容器 - 邮箱 */
    {
        RT_Object_Class_MailBox,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_MailBox),
        sizeof(struct rt_mailbox)
    },
#endif

#ifdef RT_USING_MESSAGEQUEUE                          
    /* 初始化对象容器 - 消息队列 */
    {
        RT_Object_Class_MessageQueue,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_MessageQueue),
        sizeof(struct rt_messagequeue)
    },
#endif

#ifdef RT_USING_MEMHEAP                                 
    /* 初始化对象容器 - 内存堆 */
    {
        RT_Object_Class_MemHeap,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_MemHeap),
        sizeof(struct rt_memheap)
    },
#endif

#ifdef RT_USING_MEMPOOL                                
    /* 初始化对象容器 - 内存池 */
    {
        RT_Object_Class_MemPool,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_MemPool),
        sizeof(struct rt_mempool)
    },
#endif

#ifdef RT_USING_DEVICE                                 
    /* 初始化对象容器 - 设备 */
    {
        RT_Object_Class_Device,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Device), sizeof(struct rt_device)},
#endif
    /* 初始化对象容器 - 定时器 */                          
    /*
    {
        RT_Object_Class_Timer,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Timer),
        sizeof(struct rt_timer)
    },
    */
#ifdef RT_USING_MODULE                                   
    /* 初始化对象容器 - 模块 */
    {
        RT_Object_Class_Module,
        _OBJ_CONTAINER_LIST_INIT(RT_Object_Info_Module),
        sizeof(struct rt_module)
    },
#endif
};

struct rt_object_information *rt_object_get_information(enum rt_object_class_type type)
{
    int index;

    for (index = 0; index < RT_Object_Info_Unknown; index ++)
        if (rt_object_container[index].type == type) return &rt_object_container[index];

    return RT_NULL;
}

/**
* 该函数将初始化对象并将对象添加到对象容器中
*
* @param object 要初始化的对象
* @param type 对象的类型
* @param name 对象的名字，在整个系统中，对象的名字必须是唯一的
*/
void rt_object_init(struct rt_object          *object,
                    enum rt_object_class_type type,
                    const char                *name)
{
    register rt_base_t temp;
    struct rt_object_information *information;

    /* 获取对象信息，即从容器里拿到对应对象列表头指针 */
    information = rt_object_get_information(type);
    
    /* 设置对象类型为静态 */
    object->type = type | RT_Object_Class_Static;

    /* 拷贝名字 */
    rt_strncpy(object->name, name, RT_NAME_MAX);

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 将对象插入到容器的对应列表中，不同类型的对象所在列表不一样 */
    rt_list_insert_after(&(information->object_list), &(object->list));

    /* 使能中断 */
    rt_hw_interrupt_enable(temp);
}