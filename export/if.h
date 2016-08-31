#ifndef __KERNEL_USER_IF_H__
#define __KERNEL_USER_IF_H__

struct share_mem_conf {
#define SHARE_MEM_MAGIC	0x87876565
	int magic;
	unsigned long user_addr;
	int size;
};

struct user_args {
	unsigned int id;
	unsigned int arg;
};

enum CMD {
	CMD_connect = 0,
	CMD_MAX,
};

struct connect_args {
	unsigned int size;
};

#endif //__KERNEL_USER_IF_H__