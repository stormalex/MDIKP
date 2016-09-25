#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "k_common.h"
#include "vblock.h"
#include "fblock.h"
#include "if.h"
#include "log.h"

#define IPC_MEM_GRARD_MAGIC_A	0xAAAAAAAA
#define IPC_MEM_GRARD_MAGIC_B	0xBBBBBBBB

static struct vblock* vpool = NULL;
static struct fblock* fblock = NULL;

static inline void init_guard_magic(void* addr, int size)
{
	unsigned long* addr1 = addr;
	unsigned long* addr2 = addr + size - IPC_MEM_GUARD_SIZE;
	
	*addr1 = IPC_MEM_GRARD_MAGIC_A;
	*addr2 = IPC_MEM_GRARD_MAGIC_B;
	
	IPC_PRINT_DBG("guard magic 1: addr 0x%08x  val 0x%08x\n", (unsigned int)addr1, (unsigned int)*addr1);
	IPC_PRINT_DBG("guard magic 2: addr 0x%08x  val 0x%08x\n", (unsigned int)addr2, (unsigned int)*addr2);
}

static inline void check_guard_magic(void* addr, int size)
{
	unsigned long* addr1 = addr;
	unsigned long* addr2 = addr + size - IPC_MEM_GUARD_SIZE;
	
	if(*addr1 != IPC_MEM_GRARD_MAGIC_A) {
		IPC_PRINT_DBG("MEM GUARD MAGIC A ERROR!!!\n");
		IPC_PRINT_DBG("check guard magic 1: addr 0x%08x  val 0x%08x\n", (unsigned int)addr1, (unsigned int)*addr1);
	}
		
	if(*addr2 != IPC_MEM_GRARD_MAGIC_B) {
		IPC_PRINT_DBG("MEM GUARD MAGIC B ERROR!!!\n");
		IPC_PRINT_DBG("check guard magic 2: addr 0x%08x  val 0x%08x\n", (unsigned int)addr2, (unsigned int)*addr2);
	}
}

int ipc_mem_alloc(void** hdl, int size, int wait)
{
	int ret = 0;
	void* addr = 0;
	int act_size = size + IPC_MEM_GUARD_SIZE * 2;
	int practical_size = act_size;
	IPC_PRINT_DBG("CALL ipc_mem_alloc(), size=%d, wait=%d\n", act_size, wait);
	
	if(act_size <= IPC_FBLOCK_SIZE) {
		act_size = IPC_FBLOCK_SIZE;
		
		if(!fblock) {
			return -ENODEV;
		}
		
		addr = alloc_fblock(fblock, wait);
		if(!addr) {
			return -ENOMEM;
		}
	}
	else {
		if(!vpool) {
			return -ENODEV;
		}
		
		//addr = alloc_vpool(vpool, act_size, wait);
	}
	
	*hdl = addr + IPC_MEM_GUARD_SIZE;
	init_guard_magic(addr, practical_size);
	
	IPC_PRINT_DBG("addr=0x%08x, hdl=0x%08x size=%d\n", (unsigned int)addr, (unsigned int)*hdl, practical_size);
	
	return ret;
}
EXPORT_SYMBOL(ipc_mem_alloc);

void ipc_mem_free(void* hdl, int size)
{
	void* addr = hdl - IPC_MEM_GUARD_SIZE;
	unsigned int act_size = size + (IPC_MEM_GUARD_SIZE*2);
	
	IPC_PRINT_DBG("CALL ipc_mem_free(), hdl=0x%08x, size=%d\n", (unsigned int)addr, act_size);

	check_guard_magic(addr, act_size);
	
	if(act_size <= IPC_FBLOCK_SIZE) {
		free_fblock(fblock, addr);
	}
	else {
		//free_vpool(vpool, addr);
	}
}
EXPORT_SYMBOL(ipc_mem_free);

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
	
	ipc->vblock = kmalloc(sizeof(struct vblock), GFP_KERNEL);
	if(!ipc->vblock) {
		IPC_PRINT_DBG("kmalloc(struct vblock) error\n");
		goto exit1;
	}
	ipc->fblock = kmalloc(sizeof(struct fblock), GFP_KERNEL);
	if(!ipc->fblock) {
		IPC_PRINT_DBG("kmalloc(struct fblock) error\n");
		goto exit2;
	}
	
	vpool = ipc->vblock;
	fblock = ipc->fblock;
	
	addr = __get_free_pages(GFP_KERNEL, get_order(size));
	if(!addr) {
		IPC_PRINT_DBG("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(size));
		ret = -ENOMEM;
		goto exit3;
	}
	
	IPC_PRINT_DBG("allock addr=0x%08x, size=%dK order=%d\n", (unsigned int)addr, size, get_order(size));
	ipc->mem_base = addr;
	ipc->mem_size = size;

	addr += sizeof(struct share_mem_conf);
	addr = IPC_ALIGN_ADDR(addr);
	
	ret = ipc_fblock_init(fblock, addr, IPC_FBLOCK_MEM_SIZE);
	if(ret) {
		IPC_PRINT_DBG("ipc_fblock_init() error\n");
		goto exit4;
	}
	
	addr += IPC_FBLOCK_MEM_SIZE;
	addr = IPC_ALIGN_ADDR(addr);
	
	ret = ipc_vblock_init(vpool, addr, size - IPC_FBLOCK_MEM_SIZE);
	if(ret) {
		IPC_PRINT_DBG("ipc_vblock_init() error\n");
		goto exit5;
	}
	return ret;
exit5:
	ipc_fblock_finalize(fblock);
exit4:
	free_pages(ipc->mem_base, get_order(ipc->mem_size));
exit3:
	kfree(ipc->fblock);
exit2:
	kfree(ipc->vblock);
exit1:

	return ret;
}
EXPORT_SYMBOL(ipc_mem_init);

void ipc_mem_finalize(struct ipc* ipc)
{
	ipc_vblock_finalize(vpool);
	ipc_fblock_finalize(fblock);
	free_pages(ipc->mem_base, get_order(ipc->mem_size));
	kfree(ipc->fblock);
	kfree(ipc->vblock);
	
	return;
}
EXPORT_SYMBOL(ipc_mem_finalize);