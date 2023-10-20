FLAGS= -Wall -Wextra -Ofast -std=c99

all: solver

solver: solver.c
	gcc -o solver $^ $(FLAGS)


clean:
	rm -f solver
	rm -f *.result
