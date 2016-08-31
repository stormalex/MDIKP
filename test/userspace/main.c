#include <stdio.h>
#include <dlfcn.h>

#include "if.h"



int main(int argc, char* argv[])
{
	int ret = 0;
	void* dl = NULL;
	dl = dlopen("./libipc.so", RTLD_LAZY);
	if(!dl) {
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	printf("Sleep...\n");
	sleep(10);

	ret = dlclose(dl);
	if(ret){
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	return 0;
}