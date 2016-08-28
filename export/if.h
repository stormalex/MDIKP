#ifndef __KERNEL_USER_IF_H__
#define __KERNEL_USER_IF_H__

struct user_args {
	unsigned int arg1;
	unsigned long arg2;
};

enum CMD {
	CMD_connect = 0,
	CMD_MAX,
};


#endif //__KERNEL_USER_IF_H__