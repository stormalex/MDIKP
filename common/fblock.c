#include "fblock.h"
#include "util.h"
#include "log.h"

int ipc_fblock_dump(struct fblock* fblock, char *buf, int limit)
{
    int len = 0;
    
    len += sprintf(buf + len, "fblock========\n");
    len += sprintf(buf + len, "base=0x%08x\ntotal_size=%d\nsize=%d\n", (unsigned int)fblock->addr, fblock->total_size, fblock->size);
    len += sprintf(buf + len, "block num=%d\n", fblock->num);
    return len;
}
EXPORT_SYMBOL(ipc_fblock_dump);


void* alloc_fblock(struct fblock* fblock, int wait)
{
    union block* block = 0;
    
    IPC_PRINT_DBG("CALL alloc_fblock()\n");
    
    mutex_lock(&fblock->mutex);
    
    if(fblock->list) {
        block = LIST_DEL_HEAD(block, fblock->list);
        fblock->num--;
        fblock->size -= 32;
    }
    else {
        if(!wait) {
            goto out;
        }
        
        {
            struct wtsk wtsk = {
                .data = 0,
            };

            add_wtsk_list_tail(&wtsk, &fblock->wtsk_list);
            mutex_unlock(&fblock->mutex);
            
            prepare_sleep(&wtsk.cookie);
            do_sleep(wtsk.cookie, wait);
            
            mutex_lock(&fblock->mutex);
            
            block = (union block*)wtsk.data;
            if(!block) {
                del_wtsk_list(&wtsk, &fblock->wtsk_list);
                goto out;
            }
            
            fblock->num--;
            fblock->size -= 32;
        }
    }
out:
    mutex_unlock(&fblock->mutex);

    IPC_PRINT_DBG("EXIT alloc_fblock() addr=0x%08x\n", (unsigned int)block);
    
    return block;
}
EXPORT_SYMBOL(alloc_fblock);

void free_fblock(struct fblock* fblock, void* addr)
{
    union block* block = addr;
    
    IPC_PRINT_DBG("CALL free_fblock() addr=0x%08x\n", (unsigned int)addr);
    
    mutex_lock(&fblock->mutex);
    if(fblock->wtsk_list) {
        
        struct wtsk* p_wtsk = NULL;
        p_wtsk = LIST_DEL_HEAD(p_wtsk, fblock->wtsk_list);
        mutex_unlock(&fblock->mutex);
        p_wtsk->data = (unsigned long)block;
        fblock->num++;
        fblock->size += 32;
        wakeup(p_wtsk->cookie);
        return;
    }
    LIST_ADD_HEAD(block, fblock->list);
    fblock->num++;
    fblock->size += 32;
    mutex_unlock(&fblock->mutex);
    return;
}
EXPORT_SYMBOL(free_fblock);

int ipc_fblock_init(struct fblock* fblock, unsigned long addr, unsigned int size)
{
    unsigned int residue_size = 0;

    if(!fblock) {
        IPC_PRINT_DBG("fblock is NULL\n");
        return -ENOMEM;
    }
    mutex_init(&fblock->mutex);
    
    fblock->wtsk_list = NULL;    
    fblock->list = NULL;
    fblock->total_size = size;
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
    
    IPC_PRINT_DBG("ipc_fblock_init() done! block number=%d\n", fblock->num);
    
    return 0;
}
EXPORT_SYMBOL(ipc_fblock_init);

void ipc_fblock_finalize(struct fblock* fblock)
{
    if(!fblock) {
        IPC_PRINT_DBG("vblock error\n");
        return;
    }
    fblock->addr = 0;
    fblock->size = 0;
    fblock = NULL;
    
    //wake up all sleep task, delete all task from list
    
    
    return;
}
EXPORT_SYMBOL(ipc_fblock_finalize);