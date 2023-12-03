UTILSLIBPATH := $(shell git rev-parse --show-toplevel)/utils

CFLAGS += `pkg-config --cflags sdl2` -I${UTILSLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2` -lm

innerSRC := utils.c sgemm.c
innerSRC := ${innerSRC:%=${UTILSLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

#${innerOBJ}: ${innerSRC}
