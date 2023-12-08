#include "stdio.h"
#include "err.h"

#include "utils.h"
#include "defs.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

/*
int main(int argc, char **argv) {
    if (argc != 6)
        errx(EXIT_FAILURE, "Usage: \
main <filename> <output width> <output height> <x offset> <y offset>");

    const float trans[] = {
        1, 0, atoi(argv[4]),
        0, 1, atoi(argv[5]),
        0, 0, 1,
    };

    SDL_Surface *image = load_image(argv[1]);
    if (image == NULL)
        errx(EXIT_FAILURE, "unable to load image: %s", SDL_GetError());
    SDL_Surface *out = utils_affine_trans(image, trans,
            atol(argv[2]), atol(argv[3]));
    if (out == NULL)
        errx(EXIT_FAILURE, "failed to preform transform: %s", SDL_GetError());

    int e = SDL_SaveBMP(out, "test.bmp");
    if (e < 0)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());

    return 0;
}
*/

int main(int argc, char **argv) {
    const char digits[9 * 9] = 
        "123456789"
        "........."
        "987654321"
        ".2.4.6.8."
        "1.3.5.7.9"
        "........."
        "........."
        "684967348"
        "002222222";
    if (utils_digits_to_grid("grid_test", digits) != 0)
        err(EXIT_FAILURE, "unalbe to create grid");
    return 0;
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: main <filename>");

    SDL_Surface *image = load_image(argv[1]);
    if (image == NULL)
        errx(EXIT_FAILURE, "unable to load image: %s", SDL_GetError());

    const float c = 28.0 / image->w;
    const float d = 28.0 / image->h;

    PRINTF(c);
    PRINTF(d);

    const float e = image->w / 2.0f;
    const float f = image->h / 2.0f;

    PRINTF(e);
    PRINTF(f);

    const float a = e * c;
    const float b = f * d;

    const float trans[] = {
        c, 0, 0, // -(c * e) + (c * e), // -(c * e) + a,
        0, d, 0, // -(d * f) + (f * d), // -(d * f) + b,
        0, 0, 1,
    };


    SDL_Surface *out = utils_affine_trans(image, trans, 28, 28);
    if (out == NULL)
        errx(EXIT_FAILURE, "failed to preform transform: %s", SDL_GetError());

    int err = SDL_SaveBMP(out, "test.bmp");
    if (err < 0)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());

    return 0;
}
