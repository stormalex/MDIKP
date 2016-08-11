#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"

static int ipc_open(struct inode *inode, struct file *file)
{
#ifdef IPC_DEBUG
	printk("ipc_open()\n");
#endif
	
	return 0;
}
static int ipc_close(struct inode *inode, struct file *file)
{
#ifdef IPC_DEBUG
	printk("ipc_close()\n");
#endif
	return 0;
}

static const struct file_operations ipc_fops = {
	.owner = THIS_MODULE,
	.open = ipc_open,
	.release = ipc_close,
	//.ioctl = ipc_ioctl,
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
