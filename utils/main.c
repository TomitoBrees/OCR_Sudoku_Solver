#include "stdio.h"
#include "err.h"

#include "gemm.h"
#include "utils.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define toI(i, j) i * 3 + j

int invert_3x3_matrix(const double *in, double *out) {
    double d = 0;
    for (size_t i = 0; i < 3; i++) {
        d += in[toI(0, i)] * (in[toI(1, (i+1)%3)] * in[toI(2, (i+2)%3)]
                - in[toI(1, (i+2)%3)] * in[toI(2, (i+1)%3)]);
    }

    printf("det: %f\n", d);

    if (fabs(d) < 0.2)
        errx(EXIT_FAILURE, "invalid determinant: %f", d);

    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 3; j++) {
            out[toI(i, j)] = (
                    in[toI((i+1)%3, (j+1)%3)] * in[toI((i+2)%3, (j+2)%3)] -
                    in[toI((i+1)%3, (j+2)%3)] * in[toI((i+2)%3, (j+1)%3)]) / d;
        }

    return 1;
}

SDL_Surface * utils_affine_trans(SDL_Surface *image, const double *trans) {

    double inv_trans[3 * 3];
    if (invert_3x3_matrix(trans, inv_trans) == 0)
        return NULL;

    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++)
            printf("%f ", inv_trans[i * 3 + j]);
        printf("\n");
    }

    unsigned int out_w = 1000;
    unsigned int out_h = 1000;
    SDL_Surface *out = SDL_CreateRGBSurface(0, out_w, out_h, 32, 0, 0, 0, 0);
    if (out == NULL)
        return NULL;

    SDL_FillRect(out, NULL, SDL_MapRGB(out->format, 0, 0, 0));

    SDL_LockSurface(out);
    SDL_LockSurface(image);

    size_t in_w = (size_t)image->w;
    size_t in_h = (size_t)image->h;

    Uint32 *out_pixels = out->pixels;
    Uint32 *in_pixels = image->pixels;

    double in_coor[3];
    for (size_t i = 0; i < out_h; i++)
        for (size_t j = 0; j < out_w; j++) {
            double out_coor[3] = {(double)i, (double)j, 1.0};
            dgemm_n(3, 1, 3, inv_trans, out_coor, in_coor);
            if (in_coor[0] >= 0 && in_coor[0] <= in_h &&
                    in_coor[1] >= 0 && in_coor[1] <= in_w)
                out_pixels[i * out_w + j] = 
                    in_pixels[(int)in_coor[0] * in_w + (int)in_coor[1]];
        }


    SDL_UnlockSurface(out);
    SDL_UnlockSurface(image);

    return out;
}

int main() {
    const double angle = 0 * M_PI / 180;
    const char *input = "input.png";
    const double x = 500;
    const double y = 500;

    const double trans[] = {
        cos(angle), -sin(angle), /*-x*cos(angle) + y*sin(angle) +*/ x,
        sin(angle), cos(angle), /*-x*sin(angle) - y*cos(angle) +*/ y,
        0, 0, 1,
    };

    SDL_Surface *image = load_image(input);
    if (image == NULL)
        errx(EXIT_FAILURE, "unable to load image: %s", SDL_GetError());
    SDL_Surface *out = utils_affine_trans(image, trans);
    if (out == NULL)
        errx(EXIT_FAILURE, "failed to preform transform: %s", SDL_GetError());

    int e = SDL_SaveBMP(out, "test.bmp");
    if (e < 0)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());


}
