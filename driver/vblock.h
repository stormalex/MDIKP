#ifndef __VBLOCK_H__
#define __VBLOCK_H__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>

struct vblock {
	unsigned long* addr;
	unsigned int size;
	struct mutex mutex;
};

int ipc_vblock_init(struct vblock* vblock, unsigned int size, struct proc_dir_entry* proc_dir);
void ipc_vblock_finalize(struct vblock* vblock);

#endif //__VBLOCK_H__