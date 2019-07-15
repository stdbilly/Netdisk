SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=client
CC:=gcc
CFLAGS:=-Wall -lpthread -I ../include/ -lcrypt
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
.PYONY:clean
clean:
	rm -rf $(OBJS) $(ELF)
