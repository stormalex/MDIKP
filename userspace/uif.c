#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "if.h"
#include "log.h"

static int ipc_dev_fd = 0;
static void* ipc_map_mem = NULL;
static int ipc_share_mem_size = 0;
static struct share_mem_conf* mem_conf = NULL;

static int u_connect(struct connect_args* cdata)
{
	struct user_args args;
	args.id = 0;
	args.arg = (unsigned long)cdata;

	if(ioctl(ipc_dev_fd, CMD_connect, &args) == -1) {
		IPC_PRINT_ERROR("ioctl() failed");
		return -1;
	}
	return 0;
}

int ipc_init()
{
	int ret = 0;
	struct connect_args cdata;

	IPC_PRINT_DBG("open the ipc dev node\n");
	//open dev node
	ipc_dev_fd = open("/dev/ipc", O_RDWR);
	if(ipc_dev_fd < 0) {
		IPC_PRINT_ERROR("open() /dev/ipc failed");
		ret = -1;
		goto out_open;
	}

	IPC_PRINT_DBG("connect kernel\n");
	//connect
	ret = u_connect(&cdata);
	if(ret)
		goto out_connect_failed;
	
	IPC_PRINT_DBG("mem map ipc dev node\n");
	ipc_share_mem_size = cdata.size;
	//mmap
	ipc_map_mem = mmap(NULL, ipc_share_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_dev_fd, 0);
	if(ipc_map_mem == MAP_FAILED) {
		IPC_PRINT_ERROR("mmap() failed");
		ret = -1;
		goto out_mmap;
	}

	mem_conf = (struct share_mem_conf*)ipc_map_mem;
	if(mem_conf->magic != SHARE_MEM_MAGIC) {
		IPC_PRINT_DBG("Connect failed\n");
		ret = -1;
		goto out_connect_failed;
	}

	mem_conf->user_addr = (unsigned long)ipc_map_mem;
	mem_conf->size = ipc_share_mem_size;
	IPC_PRINT_DBG("IPC dynamic library initialize OK\n");
	return ret;

out_connect_failed:
	if(munmap(ipc_map_mem, ipc_share_mem_size)) {
		IPC_PRINT_ERROR("munmap error");
	}
out_mmap:
out_ioctl:
	close(ipc_dev_fd);
	ipc_dev_fd = 0;
out_open:

	return ret;
}

int ipc_fini()
{
	int ret = 0;

	if(munmap(ipc_map_mem, ipc_share_mem_size)) {
		IPC_PRINT_ERROR("munmap error");
		ret = -1;
	}

	close(ipc_dev_fd);
	ipc_dev_fd = 0;
	IPC_PRINT_DBG("IPC dynamic library finalize OK\n");
	return ret;
}

struct msg* u_alloc_msg(int size, int wait)
{
	struct alloc_msg_args alloc_msg_args;
	struct user_args args;
	args.id = 0;
	args.arg = (unsigned long)&alloc_msg_args;

	if(ioctl(ipc_dev_fd, CMD_alloc_msg, &args) == -1) {
		IPC_PRINT_ERROR("ioctl() failed");
		return NULL;
	}
	return alloc_msg_args.hdl;
}