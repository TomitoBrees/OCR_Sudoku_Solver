#include "gemm.h"
#include "defs.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

SDL_Surface* load_image_RGBA(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        return NULL;

    SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(surface);
    if (new_surface == NULL)
        return NULL;

    return new_surface;
}

SDL_Surface* load_image_RGB(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        return NULL;

    SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface);
    if (new_surface == NULL)
        return NULL;

    return new_surface;
}

#define toI(i, j) i * 3 + j
int invert_3x3_matrix(const float *in, float *out) {
    float det = 0;
    for (size_t i = 0; i < 3; i++) {
        det += in[toI(0, i)] * (in[toI(1, (i+1)%3)] * in[toI(2, (i+2)%3)]
                - in[toI(1, (i+2)%3)] * in[toI(2, (i+1)%3)]);
    }

    //PRINTF(det);

    if (fabs(det) < 0.001)
        return 0;

    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 3; j++) {
            out[toI(j, i)] = (
                    in[toI((i+1)%3, (j+1)%3)] * in[toI((i+2)%3, (j+2)%3)] -
                    in[toI((i+1)%3, (j+2)%3)] * in[toI((i+2)%3, (j+1)%3)])/det;
        }

    return 1;
}

SDL_Surface * utils_affine_trans(SDL_Surface *image, const float *trans,
        size_t new_width, size_t new_height) {

    float inv_trans[3 * 3];
    if (invert_3x3_matrix(trans, inv_trans) == 0)
        return NULL;

    /*for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++)
            printf("%f ", inv_trans[i * 3 + j]);
        printf("\n");
    }*/

    ssize_t in_w = image->w;
    ssize_t in_h = image->h;

    unsigned int out_w = new_width;
    unsigned int out_h = new_height;
    SDL_Surface *out = SDL_CreateRGBSurface(0, out_w, out_h, 32, 0, 0, 0, 0);
    if (out == NULL)
        return NULL;

    SDL_FillRect(out, NULL, SDL_MapRGB(out->format, 0, 0, 0));

    SDL_LockSurface(out);
    SDL_LockSurface(image);

    Uint32 *out_pixels = out->pixels;
    Uint32 *in_pixels = image->pixels;

    ssize_t n_i;
    ssize_t n_j;
    float in_coor[3];
    for (size_t i = 0; i < out_h; i++)
        for (size_t j = 0; j < out_w; j++) {
            float out_coor[3] = {(float)j, (float)i, 1.0};
            gemm_n(3, 1, 3, inv_trans, out_coor, in_coor);
            n_i = (ssize_t)(in_coor[1] + 0.5f);
            n_j = (ssize_t)(in_coor[0] + 0.5f);
            if (n_j >= 0 && n_j < in_w &&
                    n_i >= 0 && n_i < in_h)
                out_pixels[i * out_w + j] = in_pixels[n_i* in_w + n_j];
        }


    SDL_UnlockSurface(out);
    SDL_UnlockSurface(image);

    return out;
}
