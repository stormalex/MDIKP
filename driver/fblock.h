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

struct fblock {
	unsigned long addr;
	unsigned int size;
	struct mutex mutex;
	struct wtsk* wtsk_list;
};

int ipc_fblock_init(struct fblock* fblock, unsigned int size);
void ipc_fblock_finalize(struct fblock* fblock);

#endif //__FBLOCK_H__