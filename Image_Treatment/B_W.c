#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


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

//Black & White conversion
Uint32 pixel_to_blackandwhite(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    if((r + g + b)/3 > 155)
    {
        Uint32 color = SDL_MapRGB(format, 255, 255, 255);
        return color;
    }
    else
    {
        Uint32 color = SDL_MapRGB(format, 0, 0, 0);
        return color;
    }
}

void surface_to_blackandwhite(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    SDL_LockSurface(surface);

    for (int i = 0; i < len; ++i)
    {
        pixels[i] = pixel_to_blackandwhite(pixels[i], format);
    }

    SDL_UnlockSurface(surface);
}



int main(int argc, char** argv)
{
        //Checks the number of arguments.
        if (argc != 2)
                errx(EXIT_FAILURE, "Usage: image-file");

        //Create a surface from the colored image.
        SDL_Surface* surface = load_image(argv[1]);
        if (surface == NULL)
        {
            errx(EXIT_FAILURE, "%s", SDL_GetError());

        }
        //Convert the surface into greyscale
        surface_to_blackandwhite(surface);

        //Save greyscale image
        IMG_SavePNG(surface, "test_B_W.png");

        return EXIT_SUCCESS;
}
