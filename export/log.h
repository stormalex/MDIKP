
#ifdef KERNEL_SPACE
#define IPC_PRINT_DBG(format, ...) printk("[%s %d]"format"\n", __FILE__ , __LINE__, ##__VA_ARGS__)
#endif

#ifdef USER_SPACE
#include <stdio.h>
#define IPC_PRINT_DBG(format, ...) printf("[%s %d]"format"\n", __FILE__ , __LINE__, ##__VA_ARGS__)
#define IPC_PRINT_ERROR(format, ...) (printf("[%s %d]", __FILE__ , __LINE__), perror(format))
#endif