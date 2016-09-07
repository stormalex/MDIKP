#ifdef KERNEL_SPACE

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })
#endif

#define MSG_HSIZE (offsetof(struct msg, payload))
#define mem2msg(hdl)	container_of(hdl, struct msg, payload)
#define msg2mem(msg)	(&(msg->payload))

//kernel or userspace interface
extern int ipc_mem_alloc(void** hdl, int size, int wait);
extern void ipc_mem_free(void* hdl, int size);

struct msg {
	struct msg* next;
	short size;
	short flags;
	unsigned int src_qid;
	char payload[0];
};

inline void set_msg_src(struct msg* hdl, int src_qid)
{
	hdl->src_qid = src_qid;
}

inline int get_msg_src(struct msg* hdl)
{
	return hdl->src_qid;
}


#ifdef KERNEL_SPACE

#endif
#ifdef USER_SPACE
extern struct msg* u_alloc_msg(int size, int wait);
#define IPKC_CHECK_ALLOC_MSG(size, wait) {\
		return u_alloc_msg(size, wait);}

#endif