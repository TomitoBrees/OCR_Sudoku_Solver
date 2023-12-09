ifndef SOLVERLIBPATH

SOLVERLIBPATH := $(shell git rev-parse --show-toplevel)/solver

CFLAGS += `pkg-config --cflags sdl2` -I${SOLVERLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2` -lm

innerSRC := grid_construction.c solver.c
innerSRC := ${innerSRC:%=${SOLVERLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

endif

#${innerOBJ}: ${innerSRC}