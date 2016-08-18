#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"
#include "mem_wrapper.h"

struct msg {
	short size;
	short flags;
	unsigned int src_qid;
	char payload[0];
};

#define MSG_HSIZE (offsetof(struct msg, payload))
#define mem2msg(hdl)	container_of(hdl, struct msg, payload)
#define msg2mem(msg)	(&(msg->payload))