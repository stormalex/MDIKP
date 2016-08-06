#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"
#include "vblock.h"

extern struct vblock vblock;

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
	ipc.major = register_chrdev(0, "ipc", &ipc_fops);
	ipc.class = class_create(THIS_MODULE, "ipc");
	if(!ipc.class) {
		printk("class_create() error\n");
		return -1;
	}
	ipc.dev = device_create(ipc.class, NULL, MKDEV(ipc.major, 0), NULL, "ipc");
	if(!ipc.dev) {
		printk("device_create() error\n");
		return -1;
	}
	return 0;
}

static int _k_init(void)
{
	int ret = 0;
	
	ret = ipc_cdev_init();
	if(ret) {
		printk("cdev_init() error\n");
		return -1;
	}
	
	vblock.size = IPC_ALLOC_PAGE_SIZE;
	vblock.addr = __get_free_pages(GFP_KERNEL, get_order(vblock.size));
	if(!vblock.addr) {
		printk("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(vblock.size));
		return -ENOMEM;
	}
	printk("allock size=%dK order=%d\n", vblock.size, get_order(vblock.size));
	
	ret = vblock_init(vblock.addr, vblock.size);
	if(ret) {
		printk("alloc_msg_init() error\n");
		return -1;
	}
	
	return 0;
}

static int ipc_module_init(void)
{
	int ret = 0;
	
	ret = _k_init();
	
	return ret;
}

static void ipc_module_exit(void)
{
	return;
}
module_init(ipc_module_init);
module_exit(ipc_module_exit);