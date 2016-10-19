#ifndef __MSG_H__
#define __MSG_H__
#ifdef KERNEL_SPACE

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#endif

//---------------------------------------------------------
// msg
//---------------------------------------------------------
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({            \
    const typeof(((type *)0)->member) * __mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })
#endif

#define MSG_HSIZE (offsetof(struct msg, payload))
#define mem2msg(hdl)    container_of(hdl, struct msg, payload)
#define msg2mem(msg)    (&(msg->payload))

//kernel or userspace interface
extern int ipc_mem_alloc_msg(void** hdl, int size, int wait);
extern void ipc_mem_free_msg(void* hdl, int size);

struct msg {
    struct msg* next;
    short size;
    short flags;
#define SRC_MAGIC    0x12345678
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
#define IPKC_CHECK_SPACE(func, args...)    
#endif
#ifdef USER_SPACE
int u_ipkc_alloc_msg(void** hdl, int size, int wait);
int u_ipkc_free_msg(void* hdl);
#define IPKC_CHECK_SPACE(func, args...) {\
        return u_##func(args);\
}

#endif

//---------------------------------------------------------
// msgq
//---------------------------------------------------------

#define MSGQ_NUM    (256)

struct msgq {
    int flag;
    struct msg* msg_list;
    void* async_cb;
    void* sync_cb;
};

//kernel or userspace interface
extern void* alloc_msgq(void);
extern void free_msgq(void* hdl);


#endif //__MSG_H__