#ifndef __FBLOCK_H__
#define __FBLOCK_H__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>

#include "def_ipc_common.h"


#define IPC_MEM_GUARD_SIZE (sizeof(unsigned long))
#define IPC_BLOCK_SIZE (32)
#define IPC_FBLOCK_SIZE  (IPC_BLOCK_SIZE + (IPC_MEM_GUARD_SIZE * 2))

union block {
	union block* next;
	char data[IPC_FBLOCK_SIZE];
};

struct fblock {
	unsigned long addr;
	unsigned int total_size;
	unsigned int size;
	struct mutex mutex;
	struct wtsk* wtsk_list;
	union block* list;
	unsigned int num;
};

int ipc_fblock_init(struct fblock* fblock, unsigned long addr, unsigned int size);
void ipc_fblock_finalize(struct fblock* fblock);
int ipc_fblock_dump(struct fblock* fblock, char *buf, int limit);
unsigned long alloc_fblock(struct fblock* fblock, int wait);
void free_fblock(struct fblock* fblock, unsigned long addr);
#endif //__FBLOCK_H__