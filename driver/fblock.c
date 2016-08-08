#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "def_ipc_common.h"
#include "fblock.h"

static struct fblock* p_fblock = NULL;

int ipc_fblock_init(struct fblock* fblock, unsigned int size)
{
	mutex_init(&fblock->mutex);
	
	fblock->wtsk_list = NULL;	
	fblock->size = 0;
	fblock->addr = __get_free_pages(GFP_KERNEL, get_order(size));
	if(!fblock->addr) {
		printk("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(size));
		return -ENOMEM;
	}
	printk("allock size=%dK order=%d\n", size, get_order(size));
	
	fblock->size = size;
	
	p_fblock = fblock;
	
	return 0;
}
EXPORT_SYMBOL(ipc_fblock_init);

void ipc_fblock_finalize(struct fblock* fblock)
{
	p_fblock = NULL;
	
	if(fblock->addr && fblock->size) {
		free_pages((unsigned long)fblock->addr, get_order(fblock->size));
		fblock->addr = 0;
		fblock->size = 0;
	}

	return;
}
EXPORT_SYMBOL(ipc_fblock_finalize);