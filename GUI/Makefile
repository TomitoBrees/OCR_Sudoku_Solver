CC = gcc
CFLAGS = -Wall -Wextra -O3
GTKFLAGS = $(shell pkg-config --cflags --libs gtk+-3.0 sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image)
TARGET = user_interface
SRC = user_interface.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(GTKFLAGS)

.PHONY: clean

clean:
	rm -f $(TARGET)