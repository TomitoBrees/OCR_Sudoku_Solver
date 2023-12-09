#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifndef _WIN32

#include "err.h"

#endif

#define GRID_SIZE 9

// Pas opti mais nique sa mere
//
// btw tous les #ifdef c'est a cause de windows qui fait sa crise d'ado

char get_char(char *sudoku, FILE *f) {
    int c = 0;
    while (c != EOF && c != '.' && (c > '9' || c < '1'))
        c = fgetc(f);
    if (c == EOF)
#ifdef _WIN32
        exit(2);
#else
        err(EXIT_FAILURE,
                "unexpected End Of File (EOF) while parsing input file '%s'",
                sudoku);
#endif
    if (c == '.')
        return 0;
    return (char)c - '0';
}

void parse_sudoku(char *sudoku, char *res) {
    FILE *f = fopen(sudoku, "rb");
    if (f == NULL)
#ifdef _WIN32
        exit(3);
#else
        err(EXIT_FAILURE, "unable to open input file: %s", sudoku);
#endif

    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            res[j + i * GRID_SIZE] = get_char(sudoku, f);

    fclose(f);
}

void save_sudoku(char *sudoku, char *filename) {
    char output[GRID_SIZE * GRID_SIZE + 30];
    int c = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        if (i != 0 && i % 3 == 0)
            output[c++] = '\n';
        //output[c++] = sudoku[i * GRID_SIZE];
        for (int j = 0; j < GRID_SIZE; j++) {
            if (j != 0 && j % 3 == 0)
                output[c++] = ' ';
            output[c++] = sudoku[j + i*GRID_SIZE] + '0';
        }
        output[c++] = '\n';
    }

    FILE* f = fopen(filename, "wb");
    fwrite(output, sizeof(char), sizeof(output) / sizeof(char), f);
    fclose(f);
}

int is_valid_subgrid(char *sudoku, int gridi, int gridj) {
    gridi *= 3;
    gridj *= 3;
    char check[GRID_SIZE+1] = {0};
    for (int i = 0; i < GRID_SIZE; i++) {
        int v = (int)sudoku[gridj + (i % 3) + (i / 3 + gridi) * GRID_SIZE];
        if (v != 0 && check[v] == 1) {
            //printf("invalid subgrid\n");
            return 0;
        }
        check[v] = 1;
    }

    return 1;
}

int is_valid(char *sudoku, int i, int j) {
    char check[GRID_SIZE + 1] = {0}; // +1 as sudoku number are use (could be 9)
    for (int ii = 0; ii < GRID_SIZE; ii++) {
        int v = (int)sudoku[j + ii * GRID_SIZE];
        if (v != 0 && check[v] == 1) {// duplicate character detected
            //printf("invalid i\n");
            return 0;
        }
        check[v] = 1;
    }
    
    for (int ii = 0; ii < GRID_SIZE+1; ii++)
        check[ii] = 0;

    for (int jj = 0; jj < GRID_SIZE; jj++) {
        int v = (int)sudoku[jj + i * GRID_SIZE];
        if (v != 0 && check[v] == 1) {
            //printf("invalid j\n");
            return 0;
        }
        check[v] = 1;
    }

    return is_valid_subgrid(sudoku, i / 3, j / 3);
}

int __solver(char *sudoku, int k, int depth) {
    const int maxk = GRID_SIZE * GRID_SIZE;

    while (k < maxk && sudoku[k] != 0)
        k++;
    if (k == maxk) {
        //printf("win\n");
        return 1;
    }

    int i = k / GRID_SIZE;
    int j = k % GRID_SIZE;



    for (int n = 0; n < GRID_SIZE; n++) {
        sudoku[j + i * GRID_SIZE] = n+1;
        //printf("(i = %i, j = %i), n=%i depth=%i\n", i, j, n+1, depth);
        if (is_valid(sudoku, i, j) == 1 && __solver(sudoku, k+1, depth+1) == 1) {
            return 1;
        }
    }
    sudoku[j + i * GRID_SIZE] = 0;
    return 0;
}

void solve(char* sudoku) {
    if (__solver(sudoku, 0, 0) == 0) {
#ifdef _WIN32
        exit(4);
#else
        errx(EXIT_FAILURE, "unable to solve sudoku grid");
#endif
        //printf("not solved!");
    }
}


void solve_sudoku(char* sudoku_file)
{
    char sudoku[GRID_SIZE * GRID_SIZE];

    parse_sudoku(sudoku_file, sudoku);
    solve(sudoku);

    char outputname[strlen(sudoku_file) + 7 + 1];
    strcpy(outputname, sudoku_file);
    strcat(outputname, ".result");

    save_sudoku(sudoku, outputname);
}

/*
int main()
{
    solve_sudoku("grid_1");
    return EXIT_SUCCESS;
}
 */
