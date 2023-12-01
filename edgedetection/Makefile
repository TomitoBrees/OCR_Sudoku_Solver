# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2`
LDLIBS = `pkg-config --libs sdl2` -lm

SRC = main.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

include inc.mk

${EXE}: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
