#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

SDL_Surface* load_image(const char* path);

SDL_Surface* rotation(SDL_Surface* surface, int degree);
