#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "def_ipc_common.h"
#include "vblock.h"

struct vblock vblock;

int vblock_init(unsigned int* addr, unsigned int size)
{
	return 0;
}