#ifndef __LOG_H__
#define __LOG_H__


#ifdef KERNEL_SPACE
#include <linux/kernel.h>
#define IPC_PRINT_DBG(format, ...) printk("[%s %d %s]"format"\n", __FILE__ , __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

#ifdef USER_SPACE
#include <stdio.h>
#define IPC_PRINT_DBG(format, ...) printf("[%s %d %s]"format"\n", __FILE__ , __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define IPC_PRINT_ERROR(format, ...) (printf("[%s %d]", __FILE__ , __LINE__), perror(format))
#endif


#endif //__LOG_H__