#include <stdio.h>
#include <dlfcn.h>

#include "if.h"
#include "ipkc.h"

int mem_test(void)
{
	int ret = 0;
	void* hdl = NULL;
	
	printf("Enter mem_test\n");

	ret = ipkc_alloc_msg(&hdl, 10, 0);
	if(ret){
		return ret;
	}
	sleep(3);

	ret = ipkc_free_msg(hdl);

	printf("Exit mem_test\n");
	return ret;
}

int main(int argc, char* argv[])
{
	int ret = 0;
	void* dl = NULL;

	printf("Open libipc.so\n");
	sleep(2);
	dl = dlopen("./libipc.so", RTLD_LAZY);
	if(!dl) {
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	printf("Sleep...\n");
	sleep(3);
	mem_test();

	ret = dlclose(dl);
	if(ret){
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	return 0;
}