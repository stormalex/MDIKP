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

int ipc_init()
{
	int ret = 0;
	struct connect_args cdata;

	//open dev node
	ipc_dev_fd = open("/dev/ipc", O_RDWR);
	if(ipc_dev_fd < 0) {
		perror("open() /dev/ipc failed\n");
		ret = -1;
		goto out_open;
	}

	//connect
	if(ioctl(ipc_dev_fd, CMD_connect, &cdata) == -1) {
		perror("ioctl() failed\n");
		ret = -1;
		goto out_ioctl;
	}

	ipc_share_mem_size = cdata.size;
	//mmap
	ipc_map_mem = mmap(NULL, ipc_share_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_dev_fd, 0);
	if(ipc_map_mem = MAP_FAILED) {
		perror("mmap() failed\n");
		ret = -1;
		goto out_mmap;
	}

	mem_conf = (struct share_mem_conf*)ipc_map_mem;
	if(mem_conf->magic != SHARE_MEM_MAGIC) {
		printk("Connect failed\n");
		ret = -1;
		goto out_connect_failed;
	}

	return ret;

out_connect_failed:
	if(munmap(ipc_map_mem, ipc_share_mem_size)) {
		perror("munmap error\n");
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
		perror("munmap error\n");
		ret = -1;
	}

	close(ipc_dev_fd);
	ipc_dev_fd = 0;

	return ret;
}