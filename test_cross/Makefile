CC:=gcc
SRC:=test.c
OBJ:=${SRC:.c=.o}

include ../edgedetection/inc.mk
include ../Rotation/inc.mk
include ../ai/inc.mk
include ../custom_err/err.mk

all: test
test: ${OBJ}

clean:
	rm ${OBJ} test
