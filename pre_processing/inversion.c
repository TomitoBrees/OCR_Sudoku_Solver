#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

