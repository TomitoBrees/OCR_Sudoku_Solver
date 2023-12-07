#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "pre_processing.h"


void surface_to_all(SDL_Surface* surface)
{
        SDL_LockSurface(surface);
	surface_to_grayscale(surface);	
        surface_to_contrast(surface);
	surface_to_medianfilter(surface);
	surface_to_adaptative_threshold2(surface);
	surface_to_smoothing_filter(surface);
	surface_inversion(surface);
	SDL_UnlockSurface(surface);
}

