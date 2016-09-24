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

#define IOCTL_DEV_MAGIC		0x1234
#define IOCTL_CONSTRUCT_CMD(cmd)	(IOCTL_DEV_MAGIC << 16 | cmd)
#define IOCTL_DESTRUCT_CMD(cmd)		(0x00001111 & cmd)

#define IOCTL_REQ_MAGIC	'i'
#define IOCTL_CONNECT_REQ	_IOR(IOCTL_REQ_MAGIC, CMD_connect, struct connect_args)
#define IOCTL_ALLOC_MSG_REQ	_IOW(IOCTL_REQ_MAGIC, CMD_alloc_msg, struct alloc_msg_args)
#define IOCTL_FREE_MSG_REQ	_IOW(IOCTL_REQ_MAGIC, CMD_free_msg, struct free_msg_args)

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