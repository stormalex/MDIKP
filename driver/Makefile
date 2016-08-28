KERN_DIR=/home/nick/project/linux-3.4.2

EXTRA_CFLAGS += -I$(INCDIR)

all:
	make -C $(KERN_DIR) M=`pwd` INCDIR=$(PWD)/../export modules

clean:
	make -C $(KERN_DIR) M=`pwd` INCDIR=$(PWD)/../export modules clean
	rm -rf modules.order

obj-m += ipc.o
ipc-objs := module.o vblock.o fblock.o kif.o mem_wrapper.o msg.o