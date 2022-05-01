TARGET=mywebserver

CSRCS=mywebserver.c
OBJS=$(CSRCS:.c=.o)

ifeq ($(USE_CUSTOM_IMPL),y) 
CFLAGS=-DUSE_CUSTOM_IMPL
endif

$(TARGET): $(OBJS)
	gcc -o $@ $(LDFLAGS) $(OBJS)

%.o: %.c
	gcc -c -o $@ $(CFLAGS) $<

getpid:
	ps -A | grep $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
