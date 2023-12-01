#include "err.h"

//#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "rotate.h"

// Loads an image in a surface.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surfaceRGB = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    if (surfaceRGB == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);
    return surfaceRGB;
}

int main(int argc, char** argv)
{
    //Checks the number of arguments.
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: <image-file> <angle>");

    int degree = 0;
    int i = 0;
    while (argv[2][i] != 0)
    {
        degree += argv[2][i] - 48;
        degree *= 10;
        i++;
    }

    degree /= 10;

    //Create a surface from the colored image.
    SDL_Surface* surface = load_image(argv[1]);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* rotated = rotation(surface, degree);
    if (rotated == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    IMG_SavePNG(rotated, "test_rotate.png");

    return EXIT_SUCCESS;
}
