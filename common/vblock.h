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
	unsigned long addr;
	unsigned int total_size;
	unsigned int size;
	struct mutex mutex;
	struct wtsk* wtsk_list;
};

int ipc_vblock_init(struct vblock* vblock, unsigned long addr, unsigned int size);
void ipc_vblock_finalize(struct vblock* vblock);
int ipc_vblock_dump(struct vblock* pool, char *buf, int limit);

#endif //__VBLOCK_H__