#ifndef __LOG_H__
#define __LOG_H__


//#define DEBUG

#ifdef KERNEL_SPACE
#ifdef DEBUG
#include <linux/kernel.h>
#define IPC_PRINT_DBG(format, ...) printk("[%s %d %s]"format"\n", __FILE__ , __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define IPC_PRINT_DBG(format, ...)
#endif
#endif

#ifdef USER_SPACE
#ifdef DEBUG
#include <stdio.h>
#define IPC_PRINT_DBG(format, ...) printf("[%s %d %s]"format"\n", __FILE__ , __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define IPC_PRINT_ERROR(format, ...) (printf("[%s %d]", __FILE__ , __LINE__), perror(format))
#else
#define IPC_PRINT_DBG(format, ...)
#define IPC_PRINT_ERROR(format, ...)
#endif
#endif


#endif //__LOG_H__