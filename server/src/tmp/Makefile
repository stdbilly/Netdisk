SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=mysql
CC:=gcc
CFLAGS:=-Wall -g -I /usr/include/mysql/ -lmysqlclient -lcrypt
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
clean:
	rm -rf $(OBJS) $(ELF)
