#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"
#include "vblock.h"
#include "fblock.h"

extern int ipc_cdev_init(struct ipc* ipc);
extern void ipc_cdev_finalize(struct ipc* ipc);
extern unsigned long ipc_mem_addr;

static struct ipc ipc;

static int dump_mem(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	
	*eof = 1;
	return 0;
}

static int ipc_module_init(void)
{
	int ret = 0;
	unsigned long addr = 0;
	unsigned int size = 0;
	
	memset(&ipc, 0, sizeof(struct ipc));
	
	ipc.proc_dir = proc_mkdir(IPC_PROC_DIR, NULL);
	if(!ipc.proc_dir) {
		printk("proc_mkdir() error\n");
		ret = -1;
		goto exit1;
	}
	
	ipc.mem_entry = create_proc_read_entry(IPC_PROC_MEM_ENTRY, 0400, ipc.proc_dir, dump_mem, NULL);
	if(!ipc.mem_entry) {
		printk("create_proc_read_entry() error\n");
		ret = -1;
		goto exit2;
	}
	
	ret = ipc_cdev_init(&ipc);
	if(ret) {
		printk("ipc_cdev_init() error\n");
		ret = -1;
		goto exit3;
	}
	
	size = IPC_MEM_SIZE;
	addr = __get_free_pages(GFP_KERNEL, get_order(size));
	if(!addr) {
		printk("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(size));
		ret = -ENOMEM;
		goto exit4;
	}
	
	printk("allock addr=0x%08x, size=%dK order=%d\n", (unsigned int)addr, size, get_order(size));
	ipc_mem_addr = addr;
	
	ret = ipc_fblock_init(&ipc.fblock, addr, IPC_FBLOCK_MEM_SIZE);
	if(ret) {
		printk("ipc_vblock_init() error\n");
		goto exit5;
	}
	
	addr += IPC_FBLOCK_MEM_SIZE;
	addr = IPC_ALIGN_ADDR(addr);
	
	ret = ipc_vblock_init(&ipc.vblock, addr, size - IPC_FBLOCK_MEM_SIZE);
	if(ret) {
		printk("ipc_fblock_init() error\n");
		goto exit6;
	}
	return ret;
exit6:
	ipc_fblock_finalize(&ipc.fblock);
exit5:
	free_pages(ipc_mem_addr, IPC_MEM_SIZE);
exit4:
	ipc_cdev_finalize(&ipc);
exit3:
	remove_proc_entry(IPC_PROC_MEM_ENTRY, ipc.proc_dir);
exit2:
	remove_proc_entry(IPC_PROC_DIR, NULL);
exit1:
	return ret;
}

static void ipc_module_exit(void)
{
	ipc_vblock_finalize(&ipc.vblock);
	ipc_fblock_finalize(&ipc.fblock);
	free_pages(ipc_mem_addr, IPC_MEM_SIZE);
	ipc_cdev_finalize(&ipc);
	remove_proc_entry(IPC_PROC_MEM_ENTRY, ipc.proc_dir);
	remove_proc_entry(IPC_PROC_DIR, NULL);
	return;
}
module_init(ipc_module_init);
module_exit(ipc_module_exit);
MODULE_LICENSE("GPL");