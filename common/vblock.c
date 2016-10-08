#include "vblock.h"
#include "util.h"
#include "log.h"

int ipc_vblock_dump(struct vblock* vblock, char *buf, int limit)
{
    int len = 0;
    
    len += sprintf(buf + len, "vblock========\n");
    len += sprintf(buf + len, "base=0x%08x\ntotal_size=%d\nsize=%d\n", (unsigned int)vblock->addr, vblock->total_size, vblock->size);
    
    return len;
}
EXPORT_SYMBOL(ipc_vblock_dump);

void* alloc_vpool(struct vblock* vpool, int size, int wait)
{
    struct slot *cur;
    struct slot **prve;
    struct slot *new_slot = NULL;
    void *addr;
    
    mutex_lock(&vpool->mutex);
    if(size > vpool->total_size) {
        return NULL;
        mutex_unlock(&vpool->mutex);
    }
    
retry:
    cur = vpool->next;
    prve = &cur;
    while(*prve) {
        if(cur->size > size) {
            addr = cur;
            addr += size;
            new_slot = addr;
            new_slot->next = cur->next;
            new_slot->size = cur->size - size;
            *prve = new_slot;
            
            vpool->size -= size;
            goto out;
        }
        else if(cur->size == size) {
            *prve = cur->next;
            
            vpool->size -= size;
            goto out;
        }
        prve = &((*prve)->next);
        cur = *prve;
    }
    
    if(!wait) {
        cur = NULL;
        goto out;
    }
    
    {
        struct wtsk wtsk = {
                .data = size,
        };
        add_wtsk_list_tail(&wtsk, &vpool->wtsk_list);
        mutex_unlock(&vpool->mutex);
        prepare_sleep(&wtsk.cookie);
        do_sleep(wtsk.cookie, wait);
        mutex_lock(&vpool->mutex);
        goto retry;
    }
out:
    mutex_unlock(&vpool->mutex);

    IPC_PRINT_DBG("EXIT alloc_vpool() addr=0x%08x\n", (unsigned int)cur);
    return (void *)cur;
}

void free_vpool(struct vblock* vpool, void* addr, int size)
{
    struct slot *new_slot = addr;
    struct slot *cur = vpool->next;
    struct slot **prve = &cur;
    
    if(((unsigned long)addr >= vpool->addr_end) && ((unsigned long)addr < vpool->addr))
        return;
    
    new_slot->size = size;
    
    while(*prve) {
        if(addr < (void *)(vpool->next)) {      //should be first node
            new_slot->next = vpool->next;
            vpool->next = new_slot;
        }
        else if(cur->next == NULL) {     //should be last node
            cur->next = new_slot;
            new_slot->next = NULL;
        }
        else if((addr < (void*)cur) && (addr > (void*)prve)) {
            *prve = new_slot;
            new_slot->next = cur;
        }
        
        //boundary
        
        
        prve = &((*prve)->next);
        cur = *prve;
    }
    
    //wake up
    
    return;
}

int ipc_vblock_init(struct vblock* vblock, unsigned long addr, unsigned int size)
{
    struct slot* slot = NULL;
    
    if(!vblock) {
        IPC_PRINT_DBG("vblock is NULL\n");
        return -ENOMEM;
    }
    mutex_init(&vblock->mutex);
    vblock->wtsk_list = NULL;
    vblock->total_size = size;
    vblock->size = size;
    vblock->addr = addr;
    vblock->addr_end = addr + size;
    
    slot = (struct slot*)addr;
    slot->next = NULL;
    slot->size = size;
    
    vblock->next = (struct slot *)addr;
    
    return 0;
}
EXPORT_SYMBOL(ipc_vblock_init);

void ipc_vblock_finalize(struct vblock* vblock)
{
    if(!vblock) {
        IPC_PRINT_DBG("vblock error\n");
        return;
    }
    vblock->addr = 0;
    vblock->size = 0;
    vblock = NULL;

    //wake up all sleep task, delete all task from list
    
    return;
}
EXPORT_SYMBOL(ipc_vblock_finalize);