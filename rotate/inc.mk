ROTLIBPATH := $(shell git rev-parse --show-toplevel)/Rotation

CFLAGS += `pkg-config --cflags sdl2` -I${ROTLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2`

innerSRC := rotate.c
innerSRC := ${innerSRC:%=${ROTLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

#${innerOBJ}: ${innerSRC}
