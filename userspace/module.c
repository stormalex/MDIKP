#include <stdio.h>


#define __init void __attribute__ ((constructor)) 
#define __exit void __attribute__ ((destructor))

extern ipc_init();
extern ipc_fini();

module_init(void)
{
	ipc_init();
}

module_exit(void)
{
	ipc_fini();
}