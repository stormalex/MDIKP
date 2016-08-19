#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>

#include "def_ipc_common.h"

struct info {
	int connected;
	pid_t pid;
	struct mm_struct *mm;
	void* user_addr;
	struct info* next;
};

struct info* user_list;

static unsigned long ipc_mem_addr = 0;
static unsigned long ipc_mem_size = 0;

static struct info* add_new_user(void)
{
	struct info* p_info = kmalloc(sizeof(*p_info), GFP_KERNEL);
	if(!p_info) {
		printk("kmalloc() failed\n");
		return NULL;
	}
	
	p_info->mm = current->mm;
	
	return p_info;
}

static int ipc_open(struct inode *inode, struct file *file)
{
	struct info* p_info = add_new_user();
	
	if(p_info == NULL)
		return -ENOMEM;
	
	file->private_data = p_info;
	file->private_data = NULL;
	
	return 0;
}
static int ipc_close(struct inode *inode, struct file *file)
{
	struct info* p_info = file->private_data;
	
	kfree(p_info);
	
	return 0;
}

static int ipc_mmap (struct file* file, struct vm_area_struct* vm_area)
{
	struct info* p_info = file->private_data;
	
	
	
	return 0;
}

static const struct file_operations ipc_fops = {
	.owner 		= 	THIS_MODULE,
	.open 		= 	ipc_open,
	.release 	= 	ipc_close,
	//.ioctl 	= 	ipc_ioctl,
	.mmap 		= 	ipc_mmap,
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
