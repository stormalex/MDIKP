#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "common.h"
#include "vblock.h"
#include "fblock.h"
#include "if.h"
#include "log.h"

static struct vblock* vpool = NULL;
static struct fblock* fblock = NULL;

void* alloc_msgq(void)
{
    void* addr = 0;
    addr = alloc_fblock(fblock, 0);
    if(!addr) {
        IPC_PRINT_ERR("alloc_msgq() failed\n");
        return NULL;
    }
    return (struct msgq*)addr;
}
EXPORT_SYMBOL(alloc_msgq);

void free_msgq(void* hdl)
{
    free_fblock(fblock, hdl);
}
EXPORT_SYMBOL(free_msgq);

int ipc_mem_alloc_msg(void** hdl, int size, int wait)
{
    int ret = 0;
    void* addr = 0;
    IPC_PRINT_DBG("CALL ipc_mem_alloc_msg(), size=%d, wait=%d\n", size, wait);
    
    if(size <= IPC_FBLOCK_SIZE) {
        size = IPC_FBLOCK_SIZE;
        
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
        
        addr = alloc_vpool(vpool, size, wait);
    }
    
    *hdl = addr;
    
    IPC_PRINT_DBG("addr=0x%08x, hdl=0x%08x size=%d\n", (unsigned int)addr, (unsigned int)*hdl, size);
    
    return ret;
}
EXPORT_SYMBOL(ipc_mem_alloc_msg);

void ipc_mem_free_msg(void* hdl, int size)
{
    if(size <= IPC_FBLOCK_SIZE) {
        free_fblock(fblock, hdl);
    }
    else {
        free_vpool(vpool, hdl, size);
    }
}
EXPORT_SYMBOL(ipc_mem_free_msg);

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