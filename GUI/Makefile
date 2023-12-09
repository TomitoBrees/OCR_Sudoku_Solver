CC = gcc
CFLAGS = -g -Wall -Wextra -O3 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image) #-DDEBUG
LDLIBS = $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image)
TARGET = user_interface

all: $(TARGET)

SRC = user_interface.c
OBJ := ${SRC:.c=.o}

include ../utils/inc.mk
include ../ai/inc.mk
include ../ai_detect/inc.mk

include ../edgedetection/inc.mk
include ../pre_processing/inc.mk
include ../rotate/inc.mk
include ../solver/inc.mk

$(TARGET): $(OBJ)

.PHONY: clean

clean:
	rm -f ${OBJ} $(TARGET)
