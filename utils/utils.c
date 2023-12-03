#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        return NULL;

    SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface);
    if (new_surface == NULL)
        return NULL;

    return new_surface;
}
