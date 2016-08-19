#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "msg.h"

static void init_msg(struct msg* hdl, int size)
{
	hdl->size = size;
}

struct msg* alloc_msg(int size, int wait)
{
	int ret = 0;
	int act_size = size + MSG_HSIZE;
	struct msg* p_msg = NULL;
	
	ret = ipc_mem_alloc((void **)&p_msg, act_size, wait);
	if(ret) {
		printk("ipc_mem_alloc() failed!\n");
		return NULL;
	}
	
	if(p_msg) {
		init_msg(p_msg, act_size);
	}
	
	return p_msg;
}
EXPORT_SYMBOL(alloc_msg);

void free_msg(struct msg* p_msg)
{
	ipc_mem_free(p_msg, p_msg->size);
}
EXPORT_SYMBOL(free_msg);