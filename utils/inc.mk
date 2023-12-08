ifndef UTILSLIBPATH

UTILSLIBPATH := $(shell git rev-parse --show-toplevel)/utils

CFLAGS += `pkg-config --cflags sdl2 SDL2_image` -I${UTILSLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2 SDL2_image` -lm

innerSRC := utils.c gemm.c to_file.c
innerSRC := ${innerSRC:%=${UTILSLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

endif

#${innerOBJ}: ${innerSRC}
