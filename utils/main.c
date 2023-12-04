#include "err.h"
#include "sgemm.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

/*
SDL_Surface * utils_affine_trans(SDL_Surface *image, const float *trans) {
    int out_w = 1000;
    int out_h = 1000;
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

    float out_coor[3];
    for (size_t i = 0; i < in_h; i++)
        for (size_t j = 0; j < in_w; j++) {
            float in_coor[3] = {(float)i, (float)j, 1.0};
            gemm_n(3, 1, 3, trans, in_coor, out_coor);
            if (out_coor[0] >= 0 && out_coor[0] <= out_h &&
                    out_coor[1] >= 0 && out_coor[1] <= out_w)
                out_pixels[(int)out_coor[0] * out_w + (int)out_coor[1]] = 
                    in_pixels[(int)i * in_w + (int)j];
        }


    SDL_UnlockSurface(out);
    SDL_UnlockSurface(image);

    return out;
}
*/

SDL_Surface * utils_affine_trans(SDL_Surface *image, const float *trans) {

    float inv_trans[3 * 3] = {0};
    memcpy(inv_trans, trans, sizeof(inv_trans));
    inv_trans[1] *= -1;
    inv_trans[3] *= -1;

    float cosa = inv_trans[0];
    float sina = inv_trans[1];
    float tx = inv_trans[2];
    float ty = inv_trans[5];
    inv_trans[2] = - tx * cosa - ty * sina;
    inv_trans[5] = -ty * cosa + tx * sina;

    int out_w = 1000;
    int out_h = 1000;
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

    float in_coor[3];
    for (size_t i = 0; i < out_h; i++)
        for (size_t j = 0; j < out_w; j++) {

            float out_coor[3] = {(float)i, (float)j, 1.0};
            gemm_n(3, 1, 3, inv_trans, out_coor, in_coor);

            if (in_coor[0] >= 0 && in_coor[0] <= in_h &&
                    in_coor[1] >= 0 && in_coor[1] <= in_w) {
                out_pixels[(int)i * out_w + (int)j] = 
                    in_pixels[(int)in_coor[0] * in_w + (int)in_coor[1]];
            }
        }


    SDL_UnlockSurface(out);
    SDL_UnlockSurface(image);

    return out;
}

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (!surface)
        errx(EXIT_FAILURE, "INIT SURFACE ERROR: %s", IMG_GetError());

    SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface);
    if (!new_surface)
        errx(EXIT_FAILURE, "RGB SURFACE ERROR: %s", SDL_GetError());

    return new_surface;
}

int main() {
    const float angle = 30 * M_PI / 180;
    const char *input = "input.png";
    const float trans[] = {
        /*cosf(angle)*/1, 0/*-sinf(angle)*/,0,
        /*sinf(angle)*/0, /*cosf(angle)*/1, 0,
        0, 0, 1,
    };

    SDL_Surface *image = load_image(input);
    SDL_Surface *out = utils_affine_trans(image, trans);
    if (out == NULL)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());

    int e = SDL_SaveBMP(out, "test.bmp");
    if (e < 0)
        errx(EXIT_FAILURE, "SDL: %s", SDL_GetError());


}
