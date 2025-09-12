CC = gcc
CFLAGS = -Wall -g
GL_LDFLAGS = -lglut -lGL -lGLU

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: game.c
	$(CC) $(CFLAGS) game.c -o client $(GL_LDFLAGS)

clean:
	rm -f server client

.PHONY: all clean