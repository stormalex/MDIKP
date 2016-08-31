TARGET=test_ipc.elf
CC = arm-linux-gcc

INCDIR += -I../../export

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -ldl -rdynamic -o $(TARGET)

%.o:%.c
	$(CC) -c $< $(INCDIR) -o $@

%.d:%.c
	@set -e; rm -f $@;\
	$(CC) -MM $(INCDIR) $< > $@;\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@ > $@; \
	rm -f $@.$$$$

sinclude $(SRCS:.c=.d)

.PHONY: clean 
clean:
	rm -f $(OBJS) $(TARGET) *.d
