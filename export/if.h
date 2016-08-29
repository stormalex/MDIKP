#ifndef __KERNEL_USER_IF_H__
#define __KERNEL_USER_IF_H__

struct user_args {
	unsigned int arg1;
	unsigned int arg2;
};

enum CMD {
	CMD_connect = 0,
	CMD_MAX,
};

struct connect_args {
	unsigned int size;
};

#endif //__KERNEL_USER_IF_H__