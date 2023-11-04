# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

all: grayscale B_W smoothing threshold median medium inversion contrast

SRC = grayscale.c B_W.c smoothing.c median.c medium.c inversion.c threshold.c contrast.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

contrast: contrast.o

grayscale: grayscale.o

smoothing: smoothing.o

B_W: B_W.o

threshold: threshold.o

inversion: inversion.o

median: median.o

medium: medium.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END

