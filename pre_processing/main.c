#define _GNU_SOURCE // required as asprintf is not yet in the C standart

#include "stdio.h"
#include "string.h"
#include "time.h"
#include "err.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "pre_processing.h"

struct op {
    const char *name;
    void(*f)(SDL_Surface*);
};

/*void *operations[][2] = {
    {"BandW", surface_to_blackandwhite},
};*/

struct op ops[] = {
    {"BandW", surface_to_blackandwhite},
    {"contrast", surface_to_contrast},
    {"grayscale", surface_to_grayscale},
    {"inversion", surface_inversion},
    {"median", surface_to_medianfilter},
    {"medium", surface_to_mediumfilter},
    {"smoothing", surface_to_smoothing_filter},
    {"threshold", surface_to_threshold_filter},
    {"normalisation",surface_to_normalisation_filter},
    {"threshold2",surface_to_adaptative_threshold2},
    {"all",surface_to_all},
};

size_t ops_len = sizeof(ops) / sizeof(ops[0]);

void usage_err() {
    printf("Available operations:\n");
    for (size_t i = 0; i < ops_len; i++)
        printf("  %s\n", ops[i].name);
    errx(EXIT_FAILURE, "Usage: <operation> <image file>");
}

// Loads an image in a surface.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());

    SDL_Surface* surfaceRGB = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    if (surfaceRGB == NULL)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());

    SDL_FreeSurface(surface);
    return surfaceRGB;
}

int main(int argc, char **argv) {
    if (argc < 3)
        usage_err();

    void (*func)(SDL_Surface*) = NULL;

    const char *op = argv[1];
    for (size_t i = 0; i < ops_len; i++)
        if (strcmp(op, ops[i].name) == 0) {
            func = ops[i].f;
            break;
        }

    if (func == NULL) {
        printf("Unknown operation: %s\n", argv[1]);
        usage_err();
    }

    SDL_Surface *image = load_image(argv[2]);
    if (image == NULL)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());

    func(image);

    char *filepath;
    int e = asprintf(&filepath, "output/test-%s-%lu.png",
            argv[1], (unsigned long)time(NULL));
    if (e < 0)
        err(EXIT_FAILURE, "unable to create output filename");

    IMG_SavePNG(image, filepath);
    free(filepath);

    return EXIT_SUCCESS;
}
