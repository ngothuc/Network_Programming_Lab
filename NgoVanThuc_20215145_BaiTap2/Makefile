TARGET = lookup
CC = gcc
CFLAGS = -Wall -g
SRC = main.c
OBJ = main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c domain.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(TARGET) $(OBJ)
