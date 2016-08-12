#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "def_ipc_common.h"
#include "vblock.h"

static struct vblock* vpool = NULL;
static struct fblock* fblock = NULL;

int ipc_mem_dump(char *buf, int limit)
{
	int len = 0;
	
	len += ipc_vblock_dump(vpool, buf + len, limit);
	len += ipc_fblock_dump(fblock, buf + len, limit);
	
	return len;
}
EXPORT_SYMBOL(ipc_mem_dump);

int ipc_mem_init(struct ipc* ipc, unsigned int size)
{
	int ret = 0;
	unsigned long addr = 0;
	
	vpool = &ipc->vblock;
	fblock = &ipc->fblock;
	
	if((vpool == NULL) || (fblock == NULL)) {
		printk("vpool or fblock is NULL\n");
		return -1;
	}
	
	addr = __get_free_pages(GFP_KERNEL, get_order(size));
	if(!addr) {
		printk("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(size));
		ret = -ENOMEM;
		goto exit1;
	}
	
	printk("allock addr=0x%08x, size=%dK order=%d\n", (unsigned int)addr, size, get_order(size));
	ipc->mem_base = addr;
	ipc->mem_size = size;
	
	ret = ipc_fblock_init(fblock, addr, IPC_FBLOCK_MEM_SIZE);
	if(ret) {
		printk("ipc_fblock_init() error\n");
		goto exit2;
	}
	
	addr += IPC_FBLOCK_MEM_SIZE;
	addr = IPC_ALIGN_ADDR(addr);
	
	ret = ipc_vblock_init(vpool, addr, size - IPC_FBLOCK_MEM_SIZE);
	if(ret) {
		printk("ipc_vblock_init() error\n");
		goto exit3;
	}
	return ret;
exit3:
	ipc_fblock_finalize(fblock);
exit2:
	free_pages(ipc->mem_base, get_order(ipc->mem_size));
exit1:

	return ret;
}
EXPORT_SYMBOL(ipc_mem_init);

void ipc_mem_finalize(struct ipc* ipc)
{
	ipc_vblock_finalize(vpool);
	ipc_fblock_finalize(fblock);
	free_pages(ipc->mem_base, get_order(ipc->mem_size));
	
	return;
}
EXPORT_SYMBOL(ipc_mem_finalize);