#include "vblock.h"
#include "log.h"

int ipc_vblock_dump(struct vblock* vblock, char *buf, int limit)
{
	int len = 0;
	
	len += sprintf(buf + len, "vblock========\n");
	len += sprintf(buf + len, "base=0x%08x\ntotal_size=%d\nsize=%d\n", (unsigned int)vblock->addr, vblock->total_size, vblock->size);
	
	return len;
}
EXPORT_SYMBOL(ipc_vblock_dump);

int ipc_vblock_init(struct vblock* vblock, unsigned long addr, unsigned int size)
{
	if(!vblock) {
		IPC_PRINT_DBG("vblock is NULL\n");
		return -ENOMEM;
	}
	mutex_init(&vblock->mutex);
	vblock->wtsk_list = NULL;
	vblock->total_size = size;
	vblock->size = size;
	vblock->addr = addr;
	
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