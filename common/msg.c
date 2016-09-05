#include "msg.h"
#include "log.h"

struct msg* alloc_msg(int size, int wait);
void free_msg(struct msg* p_msg);

static void init_msg(struct msg* hdl, int size)
{
	hdl->size = size;
}

struct msg* ipkc_alloc_msg(int size, int wait)
{
	int ret = 0;
	int act_size = size + MSG_HSIZE;
	struct msg* p_msg = NULL;
	
	ret = ipc_mem_alloc((void **)&p_msg, act_size, wait);
	if(ret) {
		IPC_PRINT_DBG("ipc_mem_alloc() failed!\n");
		return NULL;
	}
	
	if(p_msg) {
		init_msg(p_msg, act_size);
	}
	
	return p_msg;
}

void ipkc_free_msg(struct msg* p_msg)
{
	ipc_mem_free(p_msg, p_msg->size);
}