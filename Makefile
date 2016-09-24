
export INSTALL_DIR=$(PWD)/install
DIRS := driver userspace test

all:
	@for dir in $(DIRS);\
	do\
		if [ -d $$dir ];\
		then\
			cd $$dir;\
				if [ -f Makefile ];\
				then\
					make;\
				fi;\
				cd ../;\
		fi;\
	done;
 
.PHONY: clean install
 
clean:
	@for dir in `ls`;\
	do\
		if [ -d $$dir ];\
		then\
			cd $$dir;\
				if [ -f Makefile ];\
				then\
					make clean;\
				fi;\
				cd ../;\
		fi;\
	done;

install:
	@for dir in `ls`;\
	do\
		if [ -d $$dir ];\
		then\
			cd $$dir;\
				if [ -f Makefile ];\
				then\
					make install;\
				fi;\
				cd ../;\
		fi;\
	done;