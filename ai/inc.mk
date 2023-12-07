include ../utils/inc.mk
AILIBPATH := $(shell git rev-parse --show-toplevel)/ai

CFLAGS += -I${AILIBPATH}/include
LDLIBS += -lm

innerSRC := network.c mnist/mnist.c xor/xor.c
innerSRC := ${innerSRC:%=${AILIBPATH}/%}
innerOBJ := ${innerSRC:.c=.o}

OBJ += ${innerOBJ}

#${innerOBJ}: ${innerSRC}
