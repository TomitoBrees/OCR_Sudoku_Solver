# BLASFLAGS = `pkg-config -cflags -libs openblas` -DBLAS_EXIST

CC := gcc
CFLAGS := -Wall -Wextra -Ofast # -DDEBUG

SRC := main.c dataset/dataset.c
OBJ := ${SRC:.c=.o}

include inc.mk
include ../utils/inc.mk

main: ${OBJ}

clean:
	rm ${OBJ}
	rm main
