#include "SDL2/SDL.h"

// rotates the surface by 90 degrees leaving the original image unmodified.
// Returns NULL if the new rotated image cannot be created
//
// NOTE: locks (and then unlocked) the surface
SDL_Surface* rotation90(SDL_Surface* surface);

// rotates the surface by **degree** leaving the original unchanged.
// Returns NULL if the new image cannot be created
//
// NOTE: locks (and then unlocks) the surface
SDL_Surface* rotation(SDL_Surface* surface, int degree);
