#include "stdio.h"
#include "err.h"

#include "utils.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

int main(int argc, char **argv) {
    if (argc != 6)
        errx(EXIT_FAILURE, "Usage: \
main <filename> <output width> <output height> <x offset> <y offset>");

    /*
    const float angle = 30 * M_PI / 180;
    const float x = 500;
    const float y = 500;
    const float trans[] = {
        cos(angle), -sin(angle), -x*cos(angle) + y*sin(angle) + x,
        sin(angle), cos(angle), -x*sin(angle) - y*cos(angle) + y,
        0, 0, 1,
    };
    */

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
