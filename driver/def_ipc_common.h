#ifndef __DEF_IPC_COMMON_H__
#define __DEF_IPC_COMMON_H__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/proc_fs.h>

#define IPC_DEBUG

#include "vblock.h"
#include "fblock.h"

//alloc memory size
#define IPC_VBLOCK_SIZE	1024*16
#define IPC_FBLOCK_SIZE	1024*4

#define IPC_PROC_DIR		"ipc"
#define IPC_PROC_MEM_ENTRY	"meminfo"

struct ipc {
	int major;
	struct class* class;
	struct device* dev;
	struct proc_dir_entry* proc_dir;
	struct proc_dir_entry* mem_entry;
	struct vblock vblock;
	struct fblock fblock;
};

struct wtsk {
	void** cookie;
	unsigned long data;
	struct wtsk* next;
};

#endif //__DEF_IPC_COMMON_H__