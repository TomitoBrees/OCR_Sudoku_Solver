#include "err.h"

#include "network.h"
#include "utils.h"
#include "defs.h"

#include "SDL2/SDL.h"
//#include "SDL2/SDL_image.h"

static void surface_to_array(SDL_Surface *img, float *out) {
    SDL_LockSurface(img);
    Uint32 *pixels = img->pixels;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    for (size_t i = 0; i < 28; i++)
        for (size_t j = 0; j < 28; j++) {
            SDL_GetRGB(pixels[i * 28 + j], img->format, &r, &g, &b);
            out[i * 28 + j] = r/255;
        }
    SDL_UnlockSurface(img);
}

static void calc_offset(SDL_Surface *img, size_t *off_x, size_t *off_y) {
    SDL_LockSurface(img);
    Uint32 *p = img->pixels;
    size_t xsum = 0;
    size_t ysum = 0;
    size_t n = 0;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    for (size_t i = 0; i < (unsigned int)img->h; i++)
        for (size_t j = 0; j < (unsigned int)img->w; j++) {
            SDL_GetRGB(p[i * img->w + j], img->format, &r, &g, &b);
            if (r > 100) {
                n++;
                xsum+=j;
                ysum+=i;
            }
        }
    *off_x = xsum / n;
    *off_y = ysum / n;
    SDL_UnlockSurface(img);
}

static SDL_Surface * center_image(SDL_Surface *img) {
    size_t x_off;
    size_t y_off;
    calc_offset(img, &x_off, &y_off);
    PRINTD("offset = (%zi, %zi)\n", x_off, y_off);

    const float x = x_off-(img->w / 2.0);
    const float y = y_off-(img->h / 2.0);

    const float trans1[] = {
        1, 0, -x,
        0, 1, -y,
        0, 0, 1,
    };
    SDL_Surface *out = utils_affine_trans(img, trans1, img->w, img->h);
    return out;
}

static SDL_Surface * scale_image(SDL_Surface *img, float t) {
    const float z_x = t/img->w;
    const float z_y = t/img->h;

    const float e = img->w / 2.0;
    const float f = img->h / 2.0;

    const float trans[] = {
        z_x, 0, -(z_x * e) + 28.0 / 2,
        0, z_y, -(z_y * f) + 28.0 / 2,
        0, 0, 1,
    };

    return utils_affine_trans(img, trans, 28, 28);
}

// images and digits arrays must be same length
void detect_digits(const struct network *net,
        SDL_Surface **images, char *digits, size_t image_len) {
    float input[28 * 28];
    float output[10];
    SDL_Surface *temp;
    SDL_Surface *img;

    PRINTD("index --: (digit, confidence)\n");
    for (size_t i = 0; i < image_len; i++) {
        temp = center_image(images[i]);
        if (temp == NULL)
            errx(EXIT_FAILURE, "affine tranformation error");

        img = scale_image(temp, 45.0f);
        if (img == NULL)
            errx(EXIT_FAILURE, "affine transformation error");

        /*
        int e = IMG_SavePNG(img, "output.png");
        if (e < 0)
            errx(EXIT_FAILURE, "unable to save image: %s", SDL_GetError());
        */

        surface_to_array(img, input);
        SDL_FreeSurface(img);
        SDL_FreeSurface(temp);

        network_feed_forward(net, input, output);

        digits[i] = (char)uarr_max(output, 10);
        PRINTD("index %zu: (%hhi, %.3f)\n", i, digits[i], output[(size_t)digits[i]]);
        DARR(output, 10);
    }
}

