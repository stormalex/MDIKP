#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "def_ipc_common.h"
#include "vblock.h"

ssize_t vblock_proc_read(void)
{
	return 0;
}

int ipc_vblock_init(struct vblock* vblock, unsigned long addr, unsigned int size)
{
	mutex_init(&vblock->mutex);
	
	vblock->wtsk_list = NULL;
	vblock->size = size;
	vblock->addr = addr;
	
	return 0;
}
EXPORT_SYMBOL(ipc_vblock_init);

void ipc_vblock_finalize(struct vblock* vblock)
{
	vblock->addr = 0;
	vblock->size = 0;

	//wake up all sleep task, delete all task from list
	
	return;
}
EXPORT_SYMBOL(ipc_vblock_finalize);