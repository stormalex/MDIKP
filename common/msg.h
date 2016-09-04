#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

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

#define MSG_HSIZE (offsetof(struct msg, payload))
#define mem2msg(hdl)	container_of(hdl, struct msg, payload)
#define msg2mem(msg)	(&(msg->payload))


inline void set_msg_src(struct msg* hdl, int src_qid)
{
	hdl->src_qid = src_qid;
}

inline int get_msg_src(struct msg* hdl)
{
	return hdl->src_qid;
}


struct msg* alloc_msg(int size, int wait);
void free_msg(struct msg* p_msg);