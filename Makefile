CC = gcc
CFLAGS = -Wall -g
GL_LDFLAGS = -lglut -lGL -lGLU -lm

all: server client agent_server 2d_agent_client

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(GL_LDFLAGS)

client: game.c
	$(CC) $(CFLAGS) game.c -o client $(GL_LDFLAGS)

agent_server: agent_server.c
	$(CC) $(CFLAGS) agent_server.c -o agent_server $(GL_LDFLAGS)

2d_agent_client: 2d_agent_client.c
	$(CC) $(CFLAGS) 2d_agent_client.c -o 2d_agent_client $(GL_LDFLAGS)

clean:
	rm -f server client agent_server

.PHONY: all clean