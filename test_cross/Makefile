CC:=gcc
SRC:=test.c
OBJ:=${SRC:.c=.o}

include ../edgedetection/inc.mk
include ../rotate/inc.mk
include ../ai/inc.mk

all: test
test: ${OBJ}

clean:
	rm ${OBJ} test
