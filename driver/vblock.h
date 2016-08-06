#ifndef __VBLOCK_H__
#define __VBLOCK_H__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>

struct vblock {
	unsigned int* addr;
	unsigned int size;
	struct mutex vblock_mutex;
};

int vblock_init(unsigned int* addr, unsigned int size);

#endif //__VBLOCK_H__