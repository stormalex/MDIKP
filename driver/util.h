#ifndef __UTIL_H__
#define __UTIL_H__

#include <linux/sched.h>

#include "def_ipc_common.h"

/********************LIST**********************/

#define _LIST_ADD_HEAD(node, head, member) \
	do{								\
		typeof(node) tmp = head;	\
		node->member = NULL;		\
		head = node;				\
		node->member = tmp;			\
	}while(0)

#define _LIST_DEL_HEAD(node, head, member)	\
	do {							\
		typeof(node) tmp = head;	\
		head = tmp->member;			\
	}while(0)						\

#define LIST_ADD_HEAD(node, head) _LIST_ADD_HEAD(node, head, next)
#define LIST_DEL_HEAD(node, head) _LIST_DEL_HEAD(node, head, next)



/************************TASK_SLEEP****************************/
inline void prepare_sleep(void** cookie)
{
	*cookie = current;
	__set_current_state(TASK_INTERRUPTIBLE);  //http://www.linuxjournal.com/article/8144
}

inline void do_sleep(signed long wait)
{
	if(wait == WAIT_FOREVER) {
		schedule();
	}
	else {
		schedule_timeout(wait * HZ);
	}
}


inline void wakeup(void* cookie)
{
	wake_up_process(cookie);
}







#endif //__UTIL_H__