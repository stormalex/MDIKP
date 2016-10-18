#include "vblock.h"
#include "util.h"
#include "log.h"

int ipc_vblock_dump(struct vblock* vpool, char *buf, int limit)
{
    int len = 0;
    int i = 0;
    struct slot *cur = NULL;
    
    len += sprintf(buf + len, "vpool========\n");
    len += sprintf(buf + len, "base=0x%08x\ntotal_size=%d\nsize=%d\n", (unsigned int)vpool->addr, vpool->total_size, vpool->size);
    
    mutex_lock(&vpool->mutex);
    cur = vpool->next;
    while(cur){
        i++;
        len += sprintf(buf + len, "[%d]SLOT########\n", i);
        len += sprintf(buf + len, "%p %d\n", cur, cur->size);
        len += sprintf(buf + len, "SLOT########\n");
        cur = cur->next;
    }
    mutex_unlock(&vpool->mutex);
    return len;
}
EXPORT_SYMBOL(ipc_vblock_dump);

void* alloc_vpool(struct vblock* vpool, int size, int wait)
{
    struct slot *cur;
    struct slot **prve;
    void *addr = NULL;
    
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
            cur->size -= size;
            addr = (void *)cur;
            addr = addr + cur->size;
            
            vpool->size -= size;
            goto out;
        }
        else if(cur->size == size) {
            *prve = cur->next;
            addr = (void*)cur;
            vpool->size -= size;
            goto out;
        }
        prve = &cur->next;
        cur = *prve;
    }
    
    if(!wait) {
        goto out;
    }
    
    {
        struct wtsk wtsk = {
                .data = 0,
        };
        add_wtsk_list_tail(&wtsk, &vpool->wtsk_list);
        mutex_unlock(&vpool->mutex);
        prepare_sleep(&wtsk.cookie);
        do_sleep(wtsk.cookie, wait);
        mutex_lock(&vpool->mutex);
        if(wtsk.data == 1) {
            del_wtsk_list(&wtsk, &vpool->wtsk_list);
            cur = NULL;
            goto retry;
        }
    }
out:
    mutex_unlock(&vpool->mutex);

    IPC_PRINT_DBG("EXIT alloc_vpool() addr=%p\n", addr);
    return addr;
}

void free_vpool(struct vblock* vpool, void* addr, int size)
{
    struct slot *new_slot = addr;
    struct slot *cur = NULL;
    struct slot **prve = &vpool->next;
    
    if(((unsigned long)addr >= vpool->addr_end) && ((unsigned long)addr < vpool->addr))
        return;
    
    new_slot->size = size;
    
    mutex_lock(&vpool->mutex);
    
    while(*prve) {
        cur = *prve;
        
        if(addr > ((void*)cur + cur->size)) {
            IPC_PRINT_ERR("1\n");
            prve = &cur->next;
            continue;
        }
        
        if((addr + size) == (void*)cur) {
            IPC_PRINT_ERR("2\n");
            new_slot->next = cur->next;
            new_slot->size = size + cur->size;
            *prve = new_slot;
            goto out;
        }
        else if(((void*)cur + cur->size) == addr) {
            struct slot* next = cur->next;
            cur->size = cur->size + size;
            IPC_PRINT_ERR("3 next=%p addr=%p (addr+size)=%p\n", next, addr, (addr + size));
            if(((void*)next) == (addr + size)) {
                IPC_PRINT_ERR("3.5\n");
                cur->size = cur->size + next->size;
                cur->next = next->next;
            }
            goto out;
        }
        else {
            IPC_PRINT_ERR("4\n");
            new_slot->next = cur->next;
            cur->next = new_slot;
            goto out;
        }
        
        prve = &cur->next;
    }
    IPC_PRINT_ERR("5\n");
    cur = addr;
    cur->size = size;
    cur->next = *prve;
    *prve = cur;
out:
    //wake up
    while(vpool->wtsk_list) {
        struct wtsk* wtsk = LIST_DEL_HEAD(wtsk, vpool->wtsk_list);
        wtsk->data = 1;
        wakeup(&wtsk->cookie);
    }
    
    mutex_unlock(&vpool->mutex);
    
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