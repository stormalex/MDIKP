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
#include "def_ipc_common.h"
#include "if.h"

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

typedef long (*CMD_FUNC)(struct user_info*, unsigned int, unsigned long);

static struct user_info* add_new_user(void)
{
	struct user_info* p_info = kmalloc(sizeof(*p_info), GFP_KERNEL);
	if(!p_info) {
		printk("kmalloc() failed\n");
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
	unsigned long size = vm_area->vm_start - vm_area->vm_end;
	
	pfn = __phys_to_pfn(virt_to_phys((void *)ipc_mem_addr));
	ret = remap_pfn_range(vm_area,
					vm_area->vm_start,
					pfn,
					size,
					vm_area->vm_page_prot);
	if(ret) {
		printk("remap_pfn_range error\n");
		return -EAGAIN;
	}

	p_info->user_addr = vm_area->vm_start;
	
	return 0;
}

static long cmd_connect(struct user_info* info, unsigned int arg1, unsigned long arg2)
{
	struct connect_args cdata;

	printk("CALL connect\n");
	
	cdata.size = ipc_mem_size;
	
	if(copy_to_user((void *)arg2, &cdata, sizeof(cdata))) {
		printk("copy_to_user() error\n");
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

	printk("CMD:0x%08x\n", cmd);

	//command error
	if(cmd >= CMD_MAX) {
		printk("CMD error\n");
		return -EINVAL;
	}

	if(p_info->mm != current->mm) {
		printk("Different process\n");
		return -EACCES;
	}

	if(copy_from_user(&args, (void *)arg, sizeof(args))) {
		printk("copy_from_user() error\n");
		return -EFAULT;
	}

	return cmd_funcs[cmd](p_info, args.arg1, args.arg2);
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
		printk("register_chrdev() error\n");
		ret = -1;
		goto exit1;
	}
	
	ipc->class = class_create(THIS_MODULE, "ipc");
	if(!ipc->class) {
		printk("class_create() error\n");
		ret = -1;
		goto exit2;
	}
	
	ipc->dev = device_create(ipc->class, NULL, MKDEV(ipc->major, 0), NULL, "ipc");
	if(!ipc->dev) {
		printk("device_create() error\n");
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
