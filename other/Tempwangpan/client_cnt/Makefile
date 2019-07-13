SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=client
CFFLAG:=-lcrypt -lpthread -lrt
$(ELF):$(OBJS)
	gcc $^ -Wall -o $@ $(CFFLAG)
clean:
	rm -rf $(OBJS) $(ELF)

