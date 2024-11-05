all: server client

server: server.c account.c
	gcc -o server server.c account.c

client: client.c
	gcc -o client client.c

clean:
	rm -f server client
