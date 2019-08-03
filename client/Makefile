SRCS:=$(wildcard src/*.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=client
CC:=gcc
CFLAGS:=-Wall -I include/ -lpthread -lcrypto
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
.PYONY:clean
clean:
	rm -rf $(OBJS) $(ELF)
