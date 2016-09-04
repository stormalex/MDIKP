TARGET=libipc.so
CC = arm-linux-gcc

CFLAGS := $(CFLAGS)
CFLAGS += -DUSER_SPACE

INCDIR += -I../export

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -shared -fPIC -o $(TARGET)

%.o:%.c
	$(CC) -c $< $(INCDIR) $(CFLAGS) -o $@

%.d:%.c
	@set -e; rm -f $@;\
	$(CC) -MM $(INCDIR) $< > $@;\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@ > $@; \
	rm -f $@

include $(SRCS:.c=.d)

.PHONY: clean 
clean:
	rm -f $(OBJS) $(TARGET) *.d