# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

#all: main

SRC = main.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

include inc.mk

${EXE}: ${OBJ}

.PHONY: clean

clean:
	${RM} test_rotate.png
	${RM} ${OBJ}
	${RM} ${EXE}

# END
