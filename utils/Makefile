CC := gcc
CFLAGS := -Wall -Wextra -g `pkg-config --cflags sdl2 SDL2_image` -Iinclude/ -DDEBUG
LDLIBS := `pkg-config --libs sdl2 SDL2_image` -lm

SRC := main.c
OBJ := ${SRC:.c=.o}

include inc.mk
include inc.mk

main: $(OBJ)

.PHONY: clean

clean:
	${RM} ${OBJ} main
