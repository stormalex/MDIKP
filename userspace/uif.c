#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "if.h"

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
		perror("ioctl() failed");
		return -1;
	}
	return 0;
}

int ipc_init()
{
	int ret = 0;
	struct connect_args cdata;

	printf("open the ipc dev node\n");
	//open dev node
	ipc_dev_fd = open("/dev/ipc", O_RDWR);
	if(ipc_dev_fd < 0) {
		perror("open() /dev/ipc failed");
		ret = -1;
		goto out_open;
	}

	printf("connect kernel\n");
	//connect
	ret = u_connect(&cdata);
	if(ret)
		goto out_connect_failed;
	
	printf("mem map ipc dev node\n");
	ipc_share_mem_size = cdata.size;
	//mmap
	ipc_map_mem = mmap(NULL, ipc_share_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_dev_fd, 0);
	if(ipc_map_mem == MAP_FAILED) {
		perror("mmap() failed");
		ret = -1;
		goto out_mmap;
	}

	mem_conf = (struct share_mem_conf*)ipc_map_mem;
	if(mem_conf->magic != SHARE_MEM_MAGIC) {
		printf("Connect failed\n");
		ret = -1;
		goto out_connect_failed;
	}

	mem_conf->user_addr = (unsigned long)ipc_map_mem;
	mem_conf->size = ipc_share_mem_size;
	printf("IPC dynamic library initialize OK\n");
	return ret;

out_connect_failed:
	if(munmap(ipc_map_mem, ipc_share_mem_size)) {
		perror("munmap error");
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
		perror("munmap error");
		ret = -1;
	}

	close(ipc_dev_fd);
	ipc_dev_fd = 0;
	printf("IPC dynamic library finalize OK\n");
	return ret;
}