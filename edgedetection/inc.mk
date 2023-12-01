LIBPATH := $(shell git rev-parse --show-toplevel)/edgedetection

CFLAGS += `pkg-config --cflags sdl2 ` -I${LIBPATH}/include
LDLIBS += `pkg-config --libs sdl2 ` -lm

innerSRC := edge_detection.c
innerSRC := ${innerSRC:%=${LIBPATH}/%}
innerOBJ := ${innerSRC:%.c=$%.o}

OBJ += ${innerOBJ}

${innerOBJ}: ${innerSRC}
