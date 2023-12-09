#pragma once

#include "sys/types.h"
#include "SDL2/SDL.h"

// returns NULL if the image cannot be loaded
SDL_Surface* load_image_RGB(const char* path);
SDL_Surface* load_image_RGBA(const char* path);

int invert_3x3_matrix(const float *in, float *out);

// creates a new image by applying the 3x3 transformation matrix
// on every pixel of the source image
SDL_Surface * utils_affine_trans(SDL_Surface *image,
        const float *transition_matrix,
        size_t new_width, size_t new_height);

int utils_digits_to_grid(const char *filename, const char values[9 * 9]);

