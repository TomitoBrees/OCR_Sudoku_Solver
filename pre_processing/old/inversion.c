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

void surface_inversion(SDL_Surface* surface)
{
        Uint32* pixels=surface->pixels;
        SDL_LockSurface(surface);
        SDL_PixelFormat* format=surface->format;

        Uint8 r,g,b;

        for(int i=0;i<surface->h;i++)
        {
                for(int j=0;j<surface->w;j++)
                {
                        SDL_GetRGB(pixels[i*surface->w+j],format,&r,&g,&b);
                        pixels[i*surface->w+j]=SDL_MapRGB(format,255-r,255-g,255-b);
                }
        }
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
        surface_inversion(surface);

        //Save greyscale image
        IMG_SavePNG(surface, "test_inversion.png");

        return EXIT_SUCCESS;
}

