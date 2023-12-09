#define _GNU_SOURCE
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/include/utils.h"

SDL_Surface* number_images[10];
int solved_grid[10][10];

/* UTILITIES */

/*
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surfaceRGB = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    if (surfaceRGB == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);
    return surfaceRGB;
}
 */

void load_number_images()
{
    for (int i = 1; i < 10; ++i)
    {
        char *path;
        asprintf(&path, "number_images/%i.png", i);

        SDL_Surface *surface = IMG_Load(path);
        if (surface == NULL)
            errx(EXIT_FAILURE, "%s", SDL_GetError());

        SDL_Surface *surfaceRGB = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
        if (surfaceRGB == NULL)
            errx(EXIT_FAILURE, "%s", SDL_GetError());

        number_images[i] = surfaceRGB;
        IMG_SavePNG(surfaceRGB, "test.png");

        SDL_FreeSurface(surface);
    }
}

void load_grid(char* path)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Loop through each line in the file
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            int value;
            char c;

            // Read one character at a time
            if (fscanf(file, " %c", &c) != 1) {
                fprintf(stderr, "Error reading from file\n");
                fclose(file);
                return;
            }

            // Convert the character to an integer
            value = c - '0';
            solved_grid[i][j] = value;
        }
    }

    fclose(file);
}

/* RECONSTRUCTION */

SDL_Surface* put_number(SDL_Surface *background, SDL_Surface *foreground, int x, int y)
{
    // Create a result surface with the same format as the background
    SDL_Surface *result = SDL_CreateRGBSurface(0, background->w, background->h, background->format->BitsPerPixel,
                                               background->format->Rmask, background->format->Gmask,
                                               background->format->Bmask, background->format->Amask);

    // Blit the background onto the result surface
    SDL_BlitSurface(background, NULL, result, NULL);

    // Set the position where you want to blit the foreground image
    SDL_Rect foregroundRect = {x, y, 0, 0};  // Adjust the position as needed

    // Blit the foreground onto the result surface at the specified position
    SDL_BlitSurface(foreground, NULL, result, &foregroundRect);

    return result;
}

SDL_Surface* build_grid(char *grid_path, char *sudoku_path)
{
    SDL_Surface* grid = load_image_RGBA(sudoku_path);
    load_number_images();
    load_grid(grid_path);

    int x = 5;
    int y = 5;

    for (int i = 1; i < 10; ++i)
    {
        for (int j = 1; j < 10; ++j)
        {
            int number = solved_grid[i][j];
            grid = put_number(grid, number_images[number], x, y);
            x += 55;
        }
        x = 5;
        y += 55;
    }

    return grid;
}


/*
void main()
{
    SDL_Surface *solved_sudoku = build_grid("base_images/grid_1.result", "base_images/blank-sudoku-grid.png");
    IMG_SavePNG(solved_sudoku, "SOLVED.png");
}
 */
