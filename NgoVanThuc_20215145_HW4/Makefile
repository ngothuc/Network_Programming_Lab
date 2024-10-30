SERVER_EXEC = server
CLIENT_EXEC = client

CC = gcc

CFLAGS = -Wall -g

SERVER_SRC = server.c account.h 
CLIENT_SRC = client.c 

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): server.c account.h 
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) server.c

$(CLIENT_EXEC): client.c 
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) client.c

clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC)
