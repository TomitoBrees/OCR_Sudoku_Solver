CC = gcc
CFLAGS += -Wall -Wextra -Werror -Ofast

all: libcustom_err.a

libcustom_err.a: win_err.o
	ar rcs libcustom_err.a win_err.o

win_err.o: win_err.c

clean:
	rm *.a *.o
