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

#define IPC_MEM_GUARD_SIZE (sizeof(unsigned long))

//alloc memory size
#define IPC_MEM_SIZE		1024*16
#define IPC_FBLOCK_MEM_SIZE	1024*2

#define IPC_ALIGN_ADDR(x)	((x+0x03)&(~0x03))

#define IPC_PROC_DIR		"ipc"
#define IPC_PROC_MEM_ENTRY	"meminfo"


struct ipc {
	int major;
	struct class* class;
	struct device* dev;
	struct proc_dir_entry* proc_dir;
	struct proc_dir_entry* mem_entry;
	unsigned long mem_base;
	unsigned int mem_size;
	void* vblock;
	void* fblock;
};

#define WAIT_FOREVER	0xFFFFFFFF

struct wtsk {
	void* cookie;
	unsigned long data;
	struct wtsk* next;
};


#endif //__DEF_IPC_COMMON_H__