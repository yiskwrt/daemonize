TARGET=mywebserver

CSRCS=mywebserver.c
OBJS=$(CSRCS:.c=.o)

## custom / glibc / none
ifeq ($(DAEMON_TYPE),custom)
CFLAGS+=-DUSE_CUSTOM_IMPL
else
ifeq ($(DAEMON_TYPE),glibc)
CFLAGS+=-DUSE_GLIBC_IMPL
endif
endif

$(TARGET): $(OBJS)
	gcc -o $@ $(LDFLAGS) $(OBJS)

%.o: %.c
	gcc -c -o $@ $(CFLAGS) $<

getpid:
	ps -A | grep $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
