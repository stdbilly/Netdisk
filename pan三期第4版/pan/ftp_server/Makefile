SRC:=$(wildcard *.c)
thread_pool_server:$(SRC)
	gcc $^ -o $@ -lpthread -I /usr/include/mysql/ -lmysqlclient
clean:
	rm -rf thread_pool_server
