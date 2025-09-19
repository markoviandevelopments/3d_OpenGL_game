CC = gcc
CFLAGS = -Wall -g
GL_LDFLAGS = -lglut -lGL -lGLU -lm

all: evo_tester

evo_tester: evo_tester.c
	$(CC) $(CFLAGS) evo_tester.c -o evo_tester $(GL_LDFLAGS)
clean:
	rm -f evo_tester

.PHONY: all clean
