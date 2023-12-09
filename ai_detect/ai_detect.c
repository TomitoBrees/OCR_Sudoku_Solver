#include "err.h"

#include "network.h"
#include "utils.h"
#include "defs.h"

#include "SDL2/SDL.h"

#ifdef DEBUG
#include "SDL2/SDL_image.h"
#endif

#define AI_THRESHOLD 0.5f
#define RESIZE_COEF 45.0f

extern char img_c;
static float get_resize_coef() {
    if (img_c == 't')
        return 70.0f;
    return 45.0f;
}

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

static int calc_offset(SDL_Surface *img, size_t *off_x, size_t *off_y) {
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

    if(n != 0)
    {
        *off_x = xsum / n;
        *off_y = ysum / n;
    } else {
        *off_x = 0;
        *off_y = 0;
    }
    SDL_UnlockSurface(img);
    return (float)n / (float)(img->w * img->h) > 0.10;
}

static SDL_Surface * center_image(SDL_Surface *img/*, int *ok*/) {
    size_t x_off;
    size_t y_off;
    calc_offset(img, &x_off, &y_off);
    /*if (calc_offset(img, &x_off, &y_off) != 0) {
        *ok = 0;
        return NULL;
    }
    *ok = 1;*/
    //PRINTD("offset = (%zi, %zi)\n", x_off, y_off);

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

        img = scale_image(temp, get_resize_coef());
        if (img == NULL)
            errx(EXIT_FAILURE, "affine transformation error");

#ifdef DEBUG
        if (i == 9) {
        int e = IMG_SavePNG(images[i], "output_original.png");
        if (e < 0)
            errx(EXIT_FAILURE, "unable to save image: %s", SDL_GetError());
        e = IMG_SavePNG(temp, "output_center.png");
        if (e < 0)
            errx(EXIT_FAILURE, "unable to save image: %s", SDL_GetError());
        e = IMG_SavePNG(img, "output_scale.png");
        if (e < 0)
            errx(EXIT_FAILURE, "unable to save image: %s", SDL_GetError());
        }
#endif
        size_t a;
        size_t b;
        if (!calc_offset(img, &a, &b)) {
            digits[i] = '.';
            PRINTD("index %zu: not enough pixels\n", i);
            continue;
        }

        surface_to_array(img, input);
        SDL_FreeSurface(img);
        SDL_FreeSurface(temp);

        network_feed_forward(net, input, output);

        size_t c = uarr_max(output, 10);
        float s_max = output[0];
        for (size_t i = 1; i < 10; i++)
            if (i != c && output[i]> s_max)
                s_max = output[i];
        PRINTF(s_max);

        digits[i] = (char)c + '0';
        PRINTD("index %zu: (%zu, %.3f)\n", i, c, output[c]);
        if ( output[c] < AI_THRESHOLD || (output[c] - s_max) < 0.5f || c == 0)
            digits[i] = '.';
        DARR(output, 10);
    }
    //DARR_ALL("%c", digits, image_len);
}

