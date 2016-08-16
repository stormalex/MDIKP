#ifndef __MEM_WRAPPER_H__
#define __MEM_WRAPPER_H__


#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

/********************wrapper**************************/


int ipc_mem_init(struct ipc* ipc, unsigned int size);
void ipc_mem_finalize(struct ipc* ipc);
int ipc_mem_dump(char *buf, int limit);
int ipc_mem_alloc(void** hdl, int size, int wait);
void ipc_mem_free(void* hdl, int size);
/********************wrapper**************************/
#endif //__MEM_WRAPPER_H__