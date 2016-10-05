#ifndef __UTIL_H__
#define __UTIL_H__

#include <linux/sched.h>

#define WAIT_FOREVER    0xFFFFFFFF

struct wtsk {
    void* cookie;
    unsigned long data;
    struct wtsk* next;
};
/********************LIST**********************/

#define _LIST_ADD_HEAD(node, head, member) \
    do{                                    \
        node->member = head;            \
        head = node;                    \
    }while(0)

#define _LIST_ADD_TAIL(node, head, member) \
    do{                                        \
        typeof(node)* tmp = &head;            \
        while(*tmp)tmp = &((*tmp)->member);    \
        *tmp = node;                        \
        node->member = NULL;                \
    }while(0)
        
#define _LIST_DEL_HEAD(node, head, member)    \
    ({                                \
        typeof(node) tmp = head;    \
        head = tmp->member;            \
        tmp;                        \
    })

#define LIST_ADD_HEAD(node, head) _LIST_ADD_HEAD(node, head, next)
#define LIST_ADD_TAIL(node, head) _LIST_ADD_TAIL(node, head, next)
#define LIST_DEL_HEAD(node, head) _LIST_DEL_HEAD(node, head, next)



/************************TASK_SLEEP****************************/
static inline void prepare_sleep(void** cookie)
{
    *cookie = current;
    __set_current_state(TASK_INTERRUPTIBLE);  //http://www.linuxjournal.com/article/8144
}

static inline int do_sleep(void* cookie, signed long wait)
{
    int ret = 0;
    if(wait == WAIT_FOREVER) {
        schedule();
    }
    else {
        ret = schedule_timeout(wait * HZ);
    }
    
    if(ret && signal_pending(cookie)) {
        printk("wake up by signal");
        return -EINTR;
    }
    
    return 0;
}


static inline void wakeup(void* cookie)
{
    wake_up_process(cookie);
}


static inline void add_wtsk_list_tail(struct wtsk* node, struct wtsk** head)
{
    LIST_ADD_TAIL(node, *head);
}

static inline void del_wtsk_list(struct wtsk* node, struct wtsk** head)
{
    typeof(node)* cur = head;
    while(*cur) {
        if(*cur == node)
            *cur = node->next;
        cur = &(*cur)->next;
    }
}

#endif //__UTIL_H__