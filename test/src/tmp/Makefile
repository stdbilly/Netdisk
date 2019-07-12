SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=mysql
CC:=gcc
CFLAGS:=-Wall
$(ELF):$(OBJS)
	gcc -g -o $@ $^ -I /usr/include/mysql/ -lmysqlclient
clean:
	rm -rf $(OBJS) $(ELF)
