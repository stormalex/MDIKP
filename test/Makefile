
all:
	@for dir in `ls`;\
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
 
.PHONY: clean
 
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