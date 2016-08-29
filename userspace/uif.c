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

int ipc_init()
{
	int ret = 0;
	struct connect_args cdata;

	//open dev node
	ipc_dev_fd = open("/dev/ipc", O_RDWR);
	if(ipc_dev_fd < 0) {
		perror("open() /dev/ipc failed\n");
		goto out_open;
	}

	//connect
	if(ioctl(ipc_dev_fd, CMD_connect, &cdata) == -1) {
		perror("ioctl() failed\n");
		goto out_ioctl;
	}

	ipc_share_mem_size = cdata.size;
	//mmap
	ipc_map_mem = mmap(NULL, ipc_share_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_dev_fd, 0);
	if(ipc_map_mem = MAP_FAILED) {
		perror("mmap() failed\n");
		goto out_mmap;
	}
out_mmap:

out_ioctl:

out_open:

	return ret;
}

int ipc_fini()
{
	int ret = 0;
	return ret;
}