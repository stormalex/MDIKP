#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "util.h"
#include "k_common.h"
#include "if.h"
#include "log.h"

struct user_info {
	int connected;
	pid_t pid;
	struct mm_struct *mm;
	unsigned long user_addr;
	struct user_info* next;
};

static struct user_info* user_list;
static struct mutex usr_list_mutex;

static unsigned long ipc_mem_addr = 0;
static unsigned long ipc_mem_size = 0;

struct share_mem_conf* mem_conf;

typedef long (*CMD_FUNC)(struct user_info*, unsigned int, unsigned long);

static struct user_info* add_new_user(void)
{
	struct user_info* p_info = kmalloc(sizeof(*p_info), GFP_KERNEL);
	if(!p_info) {
		IPC_PRINT_DBG("kmalloc() failed\n");
		return NULL;
	}

	memset(p_info, 0, sizeof(*p_info));

	p_info->mm = current->mm;
	
	return p_info;
}

static int ipc_open(struct inode *inode, struct file *file)
{
	struct user_info* p_info = add_new_user();
	
	if(p_info == NULL)
		return -ENOMEM;
	
	file->private_data = p_info;
	
	return 0;
}
static int ipc_close(struct inode *inode, struct file *file)
{
	struct user_info* p_info = file->private_data;
	
	kfree(p_info);
	
	return 0;
}

static int ipc_mmap (struct file* file, struct vm_area_struct* vm_area)
{
	int ret = 0;
	unsigned long pfn = 0;
	struct user_info* p_info = file->private_data;
	unsigned long size = vm_area->vm_end - vm_area->vm_start;
	
	pfn = page_to_pfn(virt_to_page((void *)ipc_mem_addr));
	ret = remap_pfn_range(vm_area,
					vm_area->vm_start,
					pfn,
					size,
					vm_area->vm_page_prot);
	if(ret) {
		IPC_PRINT_DBG("remap_pfn_range error\n");
		return -EAGAIN;
	}
	mem_conf = (struct share_mem_conf*)ipc_mem_addr;
	mem_conf->magic = SHARE_MEM_MAGIC;
	p_info->user_addr = vm_area->vm_start;
	
	return 0;
}

static long cmd_connect(struct user_info* info, unsigned int id, unsigned long arg)
{
	struct connect_args cdata;

	IPC_PRINT_DBG("CALL connect\n");
	
	cdata.size = ipc_mem_size;
	
	if(copy_to_user((void *)arg, &cdata, sizeof(cdata))) {
		IPC_PRINT_DBG("copy_to_user() error\n");
		return -EFAULT;
	}

	mutex_lock(&usr_list_mutex);
	LIST_ADD_TAIL(info, user_list);
	mutex_unlock(&usr_list_mutex);

	info->connected = 1;

	return 0;
}

CMD_FUNC cmd_funcs[CMD_MAX + 1] = {
	cmd_connect,
	NULL,
};

static long ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct user_args args;
	struct user_info* p_info = file->private_data;

	IPC_PRINT_DBG("CMD:0x%08x\n", cmd);

	//command error
	if(cmd >= CMD_MAX) {
		IPC_PRINT_DBG("CMD error\n");
		return -EINVAL;
	}

	if(p_info->mm != current->mm) {
		IPC_PRINT_DBG("Different process\n");
		return -EACCES;
	}

	if(copy_from_user(&args, (void *)arg, sizeof(args))) {
		IPC_PRINT_DBG("copy_from_user() error\n");
		return -EFAULT;
	}

	return cmd_funcs[cmd](p_info, args.id, args.arg);
}

static const struct file_operations ipc_fops = {
	.owner 			= 	THIS_MODULE,
	.open 			= 	ipc_open,
	.release 		= 	ipc_close,
	.unlocked_ioctl	= 	ipc_ioctl,
	.mmap 			= 	ipc_mmap,
};

int ipc_cdev_init(struct ipc* ipc)
{
	int ret = 0;
	ipc->major = register_chrdev(0, "ipc", &ipc_fops);
	if(ipc->major < 0) {
		IPC_PRINT_DBG("register_chrdev() error\n");
		ret = -1;
		goto exit1;
	}
	
	ipc->class = class_create(THIS_MODULE, "ipc");
	if(!ipc->class) {
		IPC_PRINT_DBG("class_create() error\n");
		ret = -1;
		goto exit2;
	}
	
	ipc->dev = device_create(ipc->class, NULL, MKDEV(ipc->major, 0), NULL, "ipc");
	if(!ipc->dev) {
		IPC_PRINT_DBG("device_create() error\n");
		ret = -1;
		goto exit3;
	}
	
	ipc_mem_addr = ipc->mem_base;
	ipc_mem_size = ipc->mem_size;

	user_list = NULL;
	mutex_init(&usr_list_mutex);
	
	return ret;
	
exit3:
	class_destroy(ipc->class);
exit2:
	unregister_chrdev(ipc->major, "ipc");
exit1:
	return ret;
}
EXPORT_SYMBOL(ipc_cdev_init);

void ipc_cdev_finalize(struct ipc* ipc)
{
	device_destroy(ipc->class, MKDEV(ipc->major, 0));
	class_destroy(ipc->class);
	unregister_chrdev(ipc->major, "ipc");
}
EXPORT_SYMBOL(ipc_cdev_finalize);
