#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "if.h"
#include "ipkc.h"

//#define FBLOCK_MEM_TEST
#define VPOOL_MEM_TEST


int fblock_mem_test(void)
{
    int ret = 0;
    void* hdl[51];
    int i;
    
    printf("Enter fblock_mem_test\n");

    memset(hdl, 0, sizeof(hdl));
    for(i = 0; i < 51; i++) {
        ret = ipkc_alloc_msg(&hdl[i], 20, 0);
        if(ret){
            printf("ipkc_alloc_msg() failed\n");
            return ret;
        }
        memset(hdl[i], 'A', 20);
        printf("[%d]msg hdl=0x%08x\n", i+1, (unsigned int)hdl[i]);
    }

    for(i = 0; i < 51; i++) {
        ret = ipkc_free_msg(hdl[i]);
        if(ret){
            printf("ipkc_free_msg() failed\n");
            return ret;
        }
    }

    printf("Exit fblock_mem_test\n");
    return ret;
}

int vpool_mem_test(void)
{
    int ret = 0;
    void* hdl[51];
    int i;
    
    printf("Enter vpool_mem_test\n");

    memset(hdl, 0, sizeof(hdl));
    for(i = 0; i < 4; i++) {
        ret = ipkc_alloc_msg(&hdl[i], 236, 0);
        if(ret){
            printf("ipkc_alloc_msg() failed\n");
            return ret;
        }
        memset(hdl[i], 'A', 20);
        printf("[%d]msg hdl=0x%08x\n", i+1, (unsigned int)hdl[i]);
    }

    for(i = 0; i < 4; i++) {
        ret = ipkc_free_msg(hdl[i]);
        if(ret){
            printf("ipkc_free_msg() failed\n");
            return ret;
        }
    }

    printf("Exit vpool_mem_test\n");
    return ret;
}

int main(int argc, char* argv[])
{
    int ret = 0;
    void* dl = NULL;

    printf("Open libipc.so\n");
    dl = dlopen("./libipc.so", RTLD_LAZY);
    if(!dl) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }

    printf("Sleep...\n");
#ifdef FBLOCK_MEM_TEST
    fblock_mem_test();
#endif
#ifdef VPOOL_MEM_TEST
    vpool_mem_test();
#endif
    ret = dlclose(dl);
    if(ret){
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }

    return 0;
}