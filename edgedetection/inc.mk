ifndef EDGELIBPATH

EDGELIBPATH := $(shell git rev-parse --show-toplevel)/edgedetection

CFLAGS += `pkg-config --cflags sdl2` -I${EDGELIBPATH}/include
LDLIBS += `pkg-config --libs sdl2` -lm

innerSRC := edge_detection.c
innerSRC := ${innerSRC:%=${EDGELIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

endif

#${innerOBJ}: ${innerSRC}
