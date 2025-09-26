CC = gcc
CFLAGS = -Wall -O2 -std=c99
LDFLAGS = -lglut -lGL -lGLU -lm
SOURCES = main.c init.c draw.c physics.c network.c input.c utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = willohlandia

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

main.o: main.c game.h init.h draw.h network.h input.h physics.h utils.h
	$(CC) $(CFLAGS) -c main.c -o main.o

init.o: init.c init.h game.h
	$(CC) $(CFLAGS) -c init.c -o init.o

draw.o: draw.c draw.h game.h
	$(CC) $(CFLAGS) -c draw.c -o draw.o

physics.o: physics.c physics.h game.h
	$(CC) $(CFLAGS) -c physics.c -o physics.o

network.o: network.c network.h input.h game.h
	$(CC) $(CFLAGS) -c network.c -o network.o

input.o: input.c input.h physics.h draw.h game.h
	$(CC) $(CFLAGS) -c input.c -o input.o

utils.o: utils.c utils.h game.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean