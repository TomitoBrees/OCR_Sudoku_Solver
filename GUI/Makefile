CC = gcc
CFLAGS = -Wall -Wextra -O3 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image)
TARGET = user_interface

SRC = user_interface.c
OBJ := ${SRC:.c=.o}

include ../edgedetection/inc.mk
include ../pre_processing/inc.mk
include ../rotate/inc.mk

$(TARGET): $(OBJ)

.PHONY: clean

clean:
	rm -f ${OBJ} $(TARGET)