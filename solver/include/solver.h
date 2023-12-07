#include "SDL2/SDL.h"

SDL_Surface* load_image(const char* path);
void solve_sudoku(char* sudoku_file);
SDL_Surface* build_grid(char *grid_path, char *sudoku_path);