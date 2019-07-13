SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=server
CC:=gcc
CFLAGS:=-Wall -lpthread -I /usr/include/mysql/  -I ../include/ -lmysqlclient -lcrypt
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
.PYONY:clean
clean:
	rm -rf $(OBJS) $(ELF)
