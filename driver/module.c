#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "k_common.h"
#include "k_mem.h"
#include "log.h"

/*************function declaration**************/
int ipc_cdev_init(struct ipc* ipc);
void ipc_cdev_finalize(struct ipc* ipc);

static struct ipc ipc;

static int dump_mem(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	int limit = count - 80;

	len += sprintf(page + len, "===========ipc meminfo===========\n");
	len += ipc_mem_dump(page + len, limit - len);
	len += sprintf(page + len, "=================================\n");
	
	*eof = 1;
	return len;
}

static int ipc_module_init(void)
{
	int ret = 0;
	
	memset(&ipc, 0, sizeof(struct ipc));
	
	ipc.proc_dir = proc_mkdir(IPC_PROC_DIR, NULL);
	if(!ipc.proc_dir) {
		IPC_PRINT_DBG("proc_mkdir() error\n");
		ret = -1;
		goto exit1;
	}
	
	ipc.mem_entry = create_proc_read_entry(IPC_PROC_MEM_ENTRY, 0400, ipc.proc_dir, dump_mem, NULL);
	if(!ipc.mem_entry) {
		IPC_PRINT_DBG("create_proc_read_entry() error\n");
		ret = -1;
		goto exit2;
	}
	
	ret = ipc_mem_init(&ipc, IPC_MEM_SIZE);
	if(ret) {
		IPC_PRINT_DBG("ipc_mem_init() error\n");
		ret = -1;
		goto exit3;
	}
	
	ret = ipc_cdev_init(&ipc);
	if(ret) {
		IPC_PRINT_DBG("ipc_cdev_init() error\n");
		ret = -1;
		goto exit4;
	}
	
	

	return ret;

exit4:
	ipc_mem_finalize(&ipc);
exit3:
	remove_proc_entry(IPC_PROC_MEM_ENTRY, ipc.proc_dir);
exit2:
	remove_proc_entry(IPC_PROC_DIR, NULL);
exit1:
	return ret;
}

static void ipc_module_exit(void)
{
	ipc_cdev_finalize(&ipc);
	ipc_mem_finalize(&ipc);
	remove_proc_entry(IPC_PROC_MEM_ENTRY, ipc.proc_dir);
	remove_proc_entry(IPC_PROC_DIR, NULL);
	return;
}
module_init(ipc_module_init);
module_exit(ipc_module_exit);
MODULE_LICENSE("GPL");