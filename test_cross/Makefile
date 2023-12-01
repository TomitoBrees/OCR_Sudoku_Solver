CC:=gcc
SRC:=test.c
OBJ:=${SRC:.c=.o}

include ../edgedetection/inc.mk


all: test

test: ${OBJ}

clean:
	rm *.o test
