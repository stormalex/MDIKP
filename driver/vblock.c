#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "def_ipc_common.h"
#include "vblock.h"

static struct vblock* p_vblock = NULL;

ssize_t vblock_proc_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	long ret = 0;
	char put_buf[512];

	memset(put_buf, 0, sizeof(put_buf));
	
	snprintf(put_buf, sizeof(put_buf), "block addr=0x%08x\nblock size=%d\n", (unsigned int)p_vblock->addr, p_vblock->size);
	
	ret = copy_to_user(buf, put_buf, strlen(put_buf));
	if(ret) {
		printk("copy_to_user() error\n");
		return strlen(put_buf) - ret;
	}
	
	return strlen(put_buf);
}

const struct file_operations vblock_proc_fops = {
	.read = vblock_proc_read,
};

int ipc_vblock_init(struct vblock* vblock, unsigned int size)
{
	mutex_init(&vblock->mutex);
	
	vblock->wtsk_list = NULL;
	vblock->size = 0;
	vblock->addr = __get_free_pages(GFP_KERNEL, get_order(size));
	if(!vblock->addr) {
		printk("__get_free_pages(GFP_KERNEL, %d) error\n", get_order(size));
		return -ENOMEM;
	}
	printk("allock size=%dK order=%d\n", size, get_order(size));
	
	vblock->size = size;
	
	p_vblock = vblock;
	
	return 0;
}
EXPORT_SYMBOL(ipc_vblock_init);

void ipc_vblock_finalize(struct vblock* vblock)
{
	p_vblock = NULL;
	
	if(vblock->addr && vblock->size) {
		free_pages((unsigned long)vblock->addr, get_order(vblock->size));
		vblock->addr = 0;
		vblock->size = 0;
	}

	return;
}
EXPORT_SYMBOL(ipc_vblock_finalize);