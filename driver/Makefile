KERN_DIR=/home/nick/project/linux-3.4.2

all:
		make -C $(KERN_DIR) M=`pwd` modules

clean:
		make -C $(KERN_DIR) M=`pwd` modules clean
		rm -rf modules.order

obj-m += ipc.o
ipc-objs := module.o vblock.o fblock.o kif.o mem_wrapper.o