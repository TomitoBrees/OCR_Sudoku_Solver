# BLASFLAGS = `pkg-config -cflags -libs openblas` -DBLAS_EXIST
SRC = sgemm.c

CFLAGS = -lm -Wall -Wextra

SRC := $(SRC) main.c network.c mnist/mnist.c xor/xor.c

release: $(SRC)
	gcc -o $@ $^ $(CFLAGS) -Werror -Ofast $(BLASFLAGS)

debug: $(SRC)
	gcc -o $@ $^ $(CFLAGS) -Og -g $(BLASFLAGS) -DDEBUG

clean:
	rm -f release
	rm -f debug
