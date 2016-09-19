#ifndef __IPKC_H__
#define __IPKC_H__

int ipkc_alloc_msg(void** hdl, int size, int wait);
int ipkc_free_msg(void* hdl);

#endif