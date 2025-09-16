CC = gcc
CFLAGS = -Wall -g
GL_LDFLAGS = -lglut -lGL -lGLU -lm

all: server client agent_server

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(GL_LDFLAGS)

client: game.c
	$(CC) $(CFLAGS) game.c -o client $(GL_LDFLAGS)

agent_server: agent_server.c
	$(CC) $(CFLAGS) agent_server.c -o agent_server $(GL_LDFLAGS)

clean:
	rm -f server client agent_server

.PHONY: all clean