#ifndef __DEF_IPC_COMMON_H__
#define __DEF_IPC_COMMON_H__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "vblock.h"

//alloc memory size
#define IPC_ALLOC_PAGE_SIZE	1024

#define IPC_DEBUG

struct ipc {
	int major;
	struct class* class;
	struct device* dev;
	struct vblock vblock;
};


#endif //__DEF_IPC_COMMON_H__