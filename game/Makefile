CC = gcc
CFLAGS = -Wall -O2 -std=c99
LDFLAGS = -lGL -lGLU -lglut -lm
SOURCES = main.c init.c draw.c physics.c network.c input.c utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = willohlandia

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)