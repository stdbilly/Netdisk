SRC:=$(wildcard *.c)
thread_pool_client:$(SRC)
	g++ -g  $^ -o $@ -lpthread -lcrypt
clean:
	rm thread_pool_client
