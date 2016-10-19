#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "if.h"
#include "ipkc.h"

//#define FBLOCK_MEM_TEST
//#define VPOOL_MEM_TEST


void test(int index)
{
#define L1_BITS 6
#define L2_BITS 3
#define L3_BITS 3
#define L4_BITS 3
    void* entries[1<<L1_BITS];
    int l1 = (index >> (L2_BITS + L3_BITS + L4_BITS)) & ((1<<L1_BITS) - 1);
    int l2 = (index >> (L3_BITS + L4_BITS)) & ((1<<L2_BITS) - 1);
    int l3 = (index >> (L4_BITS)) & ((1<<L3_BITS) - 1);
    int l4 = (index) & ((1<<L4_BITS) - 1);
    
    printf("index=%d l1=%d l2=%d l3=%d l4=%d\n", index, l1, l2, l3, l4);
}

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
        ret = ipkc_alloc_msg(&hdl[i], 256, 0);
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

    test(1);
    test(100);
    test(10000);
    test(0xffffffff);
    
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