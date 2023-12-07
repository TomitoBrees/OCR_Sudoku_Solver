CC := gcc
CFLAGS := -Wall -Wextra -Werror -O3 `pkg-config --cflags sdl2 SDL2_image`
LDLIBS := `pkg-config --libs sdl2 SDL2_image`

SRC := main.c
OBJ := ${SRC:.c=.o}

include ../ai/inc.mk
include ../utils/inc.mk

main: ${OBJ}

.PHONY: clean

clean:
	${RM} main ${OBJ}
