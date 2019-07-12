SOURCE:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCE))
ELF:=main
CC:=gcc
CFLAGS:=-g -I../include -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@ -pthread -lcrypt  -I /usr/include/mysql/ -lmysqlclient -lcrypto
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS)

	

