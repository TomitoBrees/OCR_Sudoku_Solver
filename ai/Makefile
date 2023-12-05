# BLASFLAGS = `pkg-config -cflags -libs openblas` -DBLAS_EXIST

CC := gcc
CFLAGS := -Wall -Wextra -Ofast # -DDEBUG

SRC := main.c
OBJ := ${SRC:.c=.o}

include inc.mk

main: ${OBJ}

release: $(SRC)
	gcc -o $@ $^ $(CFLAGS) -Werror -Ofast $(BLASFLAGS)

debug: $(SRC)
	gcc -o $@ $^ $(CFLAGS) -Og -g $(BLASFLAGS) -DDEBUG

clean:
	rm ${OBJ}
	rm main
	rm -f release
	rm -f debug
