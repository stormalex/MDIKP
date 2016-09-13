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
	CMD_alloc_msg,
	CMD_free_msg,
	CMD_MAX,
};

struct connect_args {
	unsigned int size;
};

struct alloc_msg_args {
	void** hdl;
	int size;
	int wait;
};

struct free_msg_args{
	void* hdl;
};


#endif //__KERNEL_USER_IF_H__