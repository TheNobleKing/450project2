all : client.c server.c
	gcc client.c -o client.o
	gcc server.c -o server.o
clean: 
	rm *.o
	rm a.out
	rm out.txt

client: client.o
	./client.o 0 .5 4

server: server.o
	./server.o 0 .5 4
