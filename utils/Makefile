CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --clfags sdl2 SDL2_image` -Iinclude/
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

SRC = main.c sgemm.c

main: $(SRC)

