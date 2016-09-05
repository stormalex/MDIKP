TARGET = ipc.ko

KERN_DIR=/home/nick/project/linux-3.4.2

EXTRA_CFLAGS += -I$(INCDIR)

ccflags-y += -DKERNEL_SPACE

all:
	make -C $(KERN_DIR) M=`pwd` INCDIR=$(PWD)/../export modules

install:
	@if [ $(INSTALL_DIR) ];\
	then\
		if [ -d $(INSTALL_DIR) ];\
		then\
			if [ -f $(TARGET) ];\
			then\
				cp $(TARGET) $(INSTALL_DIR);\
			fi;\
		fi;\
	else\
		echo "NO INSTALL_DIR";\
	fi;

clean:
	make -C $(KERN_DIR) M=`pwd` INCDIR=$(PWD)/../export modules clean
	rm -rf modules.order

obj-m += ipc.o
ipc-objs := fblock.o kif.o k_mem.o module.o msg.o vblock.o