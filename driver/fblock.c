#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "def_ipc_common.h"
#include "fblock.h"
#include "util.h"

int ipc_fblock_init(struct fblock* fblock, unsigned long addr, unsigned int size)
{
	unsigned int residue_size = 0;
	
	mutex_init(&fblock->mutex);
	
	fblock->wtsk_list = NULL;	
	fblock->list = NULL;
	fblock->size = size;
	fblock->addr = addr;
	fblock->num = 0;
	
	residue_size = size;
	
	while(sizeof(union block) < residue_size) {
		union block* block;
		block = (union block*)(addr + residue_size);
		residue_size -= sizeof(union block);
		LIST_ADD_HEAD(block, fblock->list);
		fblock->num++;
	}
	
	printk("ipc_fblock_init() done! block number=%d\n", fblock->num);
	
	return 0;
}
EXPORT_SYMBOL(ipc_fblock_init);

void ipc_fblock_finalize(struct fblock* fblock)
{
	fblock->addr = 0;
	fblock->size = 0;
	
	//wake up all sleep task, delete all task from list
	
	
	return;
}
EXPORT_SYMBOL(ipc_fblock_finalize);