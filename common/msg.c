#include "msg.h"
#include "log.h"

struct msg* alloc_msg(int size, int wait);
void free_msg(struct msg* p_msg);

static void init_msg(struct msg* hdl, int size)
{
	hdl->size = size;
}

static int _alloc_msg(void**hdl, int size, int wait)
{
	int ret = 0;
	int act_size = size + MSG_HSIZE;
	struct msg* p_msg = NULL;
	*hdl = NULL;
	ret = ipc_mem_alloc((void **)&p_msg, act_size, wait);
	if(ret) {
		IPC_PRINT_DBG("ipc_mem_alloc() failed!\n");
		return ret;
	}
	
	if(p_msg) {
		init_msg(p_msg, act_size);
	}
	
	*hdl = p_msg;

	return ret;
}

static void _free_msg(void* hdl)
{
	struct msg* p_msg = (struct msg*)hdl;
	ipc_mem_free(p_msg, p_msg->size);
}

int ipkc_alloc_msg(void** hdl, int size, int wait)
{
	int ret = 0;

	IPKC_CHECK_SPACE(ipkc_alloc_msg, hdl, size, wait);

	ret = _alloc_msg(hdl, size, wait);

	return ret;
}