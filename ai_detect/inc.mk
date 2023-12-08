ifndef AIDETECTLIBPATH

AIDETECTLIBPATH := $(shell git rev-parse --show-toplevel)/ai_detect

CFLAGS += `pkg-config --cflags sdl2 SDL2_image` -I${AIDETECTLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2 SDL2_image` -lm

innerSRC := ai_detect.c
innerSRC := ${innerSRC:%=${AIDETECTLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

endif

#include ../ai/inc.mk
#include ../utils/inc.mk

#${innerOBJ}: ${innerSRC}
