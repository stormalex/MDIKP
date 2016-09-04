TARGET=test_ipc.elf
CC = arm-linux-gcc

CFLAGS := $(CFLAGS)
CFLAGS += -DUSER_SPACE

INCDIR += -I../../export
CFLAGS += $(INCDIR)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -shared -fPIC -o $(TARGET)

%.o:%.c
	$(CC) -c $< $(CFLAGS) -o $@

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@ > $@; \
	rm -f $@
-include $(SRCS:.c=.d)

.PHONY: clean 
clean:
	rm -f $(OBJS) $(TARGET) *.d