CC = gcc
CFLAGS = -Wall -g
GL_LDFLAGS = -lglut -lGL -lGLU -lm -lGLEW -lglfw

all: evo_tester cdisp_evo_tester

evo_tester: evo_tester.c
	$(CC) $(CFLAGS) evo_tester.c -o evo_tester $(GL_LDFLAGS)
cdisp_evo_tester: cdisp_evo_tester.c
	$(CC) $(CFLAGS) cdisp_evo_tester.c -o cdisp_evo_tester $(GL_LDFLAGS)
clean:
	rm -f evo_tester cdisp_evo_tester

.PHONY: all clean
