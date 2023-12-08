#pragma once

#include "SDL2/SDL.h"
#include "network.h"

void detect_digits(const struct network *net,
        SDL_Surface **images, char *digits, size_t image_len);
