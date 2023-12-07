#include "SDL2/SDL.h"

void surface_to_blackandwhite(SDL_Surface* surface);

void surface_to_contrast(SDL_Surface* surface);

void surface_to_grayscale(SDL_Surface* surface);

void surface_inversion(SDL_Surface* surface);

void surface_to_medianfilter(SDL_Surface* surface);

void surface_to_mediumfilter(SDL_Surface* surface);

void surface_to_smoothing_filter(SDL_Surface* surface);

void surface_to_threshold_filter(SDL_Surface* surface);

void surface_to_normalisation_filter(SDL_Surface* surface);

void surface_to_adaptative_threshold2(SDL_Surface* surface);

void surface_otsu_threshold(SDL_Surface* surface);

void surface_to_all(SDL_Surface* surface);
