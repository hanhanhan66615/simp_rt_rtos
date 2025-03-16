#ifndef __OBJECT_H
#define __OBJECT_H

void rt_object_init(struct rt_object          *object,
                    enum rt_object_class_type type,
                    const char                *name);

#endif