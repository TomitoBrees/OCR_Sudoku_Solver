ifndef UTILSLIBPATH

UTILSLIBPATH := $(shell git rev-parse --show-toplevel)/utils

CFLAGS += `pkg-config --cflags sdl2` -I${UTILSLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2` -lm

innerSRC := utils.c gemm.c
innerSRC := ${innerSRC:%=${UTILSLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

endif

#${innerOBJ}: ${innerSRC}
