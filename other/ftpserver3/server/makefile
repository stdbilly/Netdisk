SRC:=$(wildcard *.c)
thread_pool_server:$(SRC)
	g++ $^ -o $@ -lpthread  -I /usr/include/mysql/ -lmysqlclient
clean:
	rm thread_pool_server

