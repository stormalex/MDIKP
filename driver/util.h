#ifndef __UTIL_H__
#define __UTIL_H__


#define _LIST_ADD_HEAD(node, head, member) \
	do{								\
		typeof(node) tmp = head;	\
		head = node;				\
		node->member = tmp;			\
	}while(0)
	

#define LIST_ADD_HEAD(node, head) _LIST_ADD_HEAD(node, head, next)

#endif //__UTIL_H__