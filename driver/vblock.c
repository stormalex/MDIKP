#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"
#include "vblock.h"


int ipc_vblock_init(struct vblock* vblock, unsigned int size, struct proc_dir_entry* proc_dir)
{
	mutex_init(&vblock->mutex);

	if(!proc_dir)
		return -1;
	
	
	
	vblock->size = 0;
	vblock->addr = (unsigned long*)__get_free_pages(GFP_KERNEL, get_order(size));
	if(!vblock->addr) {
		printk("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(size));
		return -ENOMEM;
	}
	printk("allock size=%dK order=%d\n", size, get_order(size));
	
	vblock->size = size;
	
	return 0;
}
EXPORT_SYMBOL(ipc_vblock_init);

void ipc_vblock_finalize(struct vblock* vblock)
{
	if(vblock->addr && vblock->size) {
		free_pages((unsigned long)vblock->addr, get_order(vblock->size));
		vblock->addr = NULL;
		vblock->size = 0;
	}
	
	return;
}
EXPORT_SYMBOL(ipc_vblock_finalize);