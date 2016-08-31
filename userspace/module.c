#include <stdio.h>

extern ipc_init();
extern ipc_fini();

void __attribute__((constructor)) module_init(void)
{
	printf("ipc userspace init\n");
	ipc_init();
}

void __attribute__((destructor)) module_exit(void)
{
	printf("ipc userspace fini\n");
	ipc_fini();
}