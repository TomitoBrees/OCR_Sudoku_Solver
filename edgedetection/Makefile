# Makefile

CC = gcc
CFLAGS = -g -Wall -Wextra -O3 `pkg-config --cflags SDL2_image`
LDLIBS = -lm `pkg-config --libs SDL2_image`

SRC := main.c
OBJ := ${SRC:.c=.o}
EXE := ${SRC:.c=}

include inc.mk
include ../rotate/inc.mk

${EXE}: ${OBJ}

.PHONY: clean clean_all

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

clean_all: clean
	${RM} -r PICTURES/*

# END
