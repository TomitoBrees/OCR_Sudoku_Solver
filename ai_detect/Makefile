CC := gcc
CFLAGS := -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image` -DDEBUG
LDLIBS := `pkg-config --libs sdl2 SDL2_image`

SRC := main.c
OBJ := ${SRC:.c=.o}

include inc.mk
include ../ai/inc.mk
include ../utils/inc.mk

main: ${OBJ}

.PHONY: clean

clean:
	${RM} main ${OBJ}
