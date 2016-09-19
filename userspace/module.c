#include <stdio.h>
#include "log.h"
extern ipc_init();
extern ipc_fini();

void __attribute__((constructor)) module_init(void)
{
	IPC_PRINT_DBG("ipc userspace init\n");
	ipc_init();
}

void __attribute__((destructor)) module_exit(void)
{
	IPC_PRINT_DBG("ipc userspace fini\n");
	ipc_fini();
}