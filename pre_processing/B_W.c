#include <SDL2/SDL.h>
#include <math.h>

//Black & White conversion
Uint32 pixel_to_blackandwhite(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    if((r + g + b)/3 > 100)
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

