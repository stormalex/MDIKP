#include "msg.h"
#include "log.h"

struct msg* alloc_msg(int size, int wait);
void free_msg(struct msg* p_msg);

static void init_msg(struct msg* msg, int size)
{
    msg->size = size;
    msg->src_qid = SRC_MAGIC;
}

#ifdef KERNEL_SPACE
static int _alloc_msg(void**hdl, int size, int wait)
{
    int ret = 0;
    int act_size = size + MSG_HSIZE;
    struct msg* p_msg = NULL;
    *hdl = NULL;
    
    IPC_PRINT_DBG("size = %d\n", act_size);
    
    ret = ipc_mem_alloc((void **)&p_msg, act_size, wait);
    if(ret) {
        IPC_PRINT_DBG("ipc_mem_alloc() failed!\n");
        return ret;
    }
    
    if(p_msg) {
        init_msg(p_msg, act_size);
    }
    
    IPC_PRINT_DBG("msg.size=%d\n", p_msg->size);
    
    *hdl = p_msg->payload;

    return ret;
}

static void _free_msg(void* hdl)
{
    struct msg* p_msg = (struct msg*)hdl;
    hdl -= MSG_HSIZE;
    p_msg = hdl;
    
    IPC_PRINT_DBG("msg.size=%d\n", p_msg->size);
    
    ipc_mem_free(p_msg, p_msg->size);
}
#endif
int ipkc_alloc_msg(void** hdl, int size, int wait)
{
    int ret = 0;

    IPKC_CHECK_SPACE(ipkc_alloc_msg, hdl, size, wait);
#ifdef KERNEL_SPACE
    ret = _alloc_msg(hdl, size, wait);
#endif
    return ret;
}

int ipkc_free_msg(void* hdl)
{
    IPKC_CHECK_SPACE(ipkc_free_msg, hdl);
#ifdef KERNEL_SPACE
    _free_msg(hdl);
#endif
    return 0;
}