ifndef PREPROLIBPATH

PREPROLIBPATH := $(shell git rev-parse --show-toplevel)/pre_processing

CFLAGS += `pkg-config --cflags sdl2` -I${PREPROLIBPATH}/include
LDLIBS += `pkg-config --libs sdl2` -lm

innerSRC := B_W.c contrast.c grayscale.c inversion.c median.c medium.c \
			smoothing.c threshold.c normalisation.c threshold2.c all.c
innerSRC := ${innerSRC:%=${PREPROLIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

endif

#${innerOBJ}: ${innerSRC}
