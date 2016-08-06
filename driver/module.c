#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"
#include "vblock.h"

struct ipc ipc;

static int ipc_open(struct inode *inode, struct file *file)
{
#ifdef IPC_DEBUG
	printk("ipc_open()\n");
#endif
	file->private_data = (void*)&ipc;
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

static int ipc_cdev_init(void)
{
	int ret = 0;
	ipc.major = register_chrdev(0, "ipc", &ipc_fops);
	if(ipc.major < 0) {
		printk("register_chrdev() error\n");
		ret = -1;
		goto exit1;
	}
	
	ipc.class = class_create(THIS_MODULE, "ipc");
	if(!ipc.class) {
		printk("class_create() error\n");
		ret = -1;
		goto exit2;
	}
	
	ipc.dev = device_create(ipc.class, NULL, MKDEV(ipc.major, 0), NULL, "ipc");
	if(!ipc.dev) {
		printk("device_create() error\n");
		ret = -1;
		goto exit3;
	}
	return ret;
	
exit3:
	class_destroy(ipc.class);
exit2:
	unregister_chrdev(ipc.major, "ipc");
exit1:
	return ret;
}

static void ipc_cdev_finalize(void)
{
	device_destroy(ipc.class, MKDEV(ipc.major, 0));
	class_destroy(ipc.class);
	unregister_chrdev(ipc.major, "ipc");
}

static int ipc_module_init(void)
{
	int ret = 0;
	
	ret = ipc_cdev_init();
	if(ret) {
		printk("ipc_cdev_init() error\n");
		ret = -1;
		goto exit1;
	}
	
	ret = ipc_vblock_init(&ipc.vblock, IPC_ALLOC_PAGE_SIZE);
	if(ret) {
		printk("ipc_vblock_init() error\n");
		goto exit2;
	}
	return ret;

exit2:
	ipc_cdev_finalize();
exit1:
	return ret;
}

static void ipc_module_exit(void)
{
	ipc_vblock_finalize(&ipc.vblock);
	ipc_cdev_finalize();
	return;
}
module_init(ipc_module_init);
module_exit(ipc_module_exit);