#include "stdio.h"
#include "err.h"

#include "network.h"
#include "utils.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

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

// images and digits arrays must be same length
void detect_digits(const struct network *net,
        SDL_Surface **images, char *digits, size_t image_len) {
    float input[28 * 28];
    float output[10];

    printf("index --: (digit, confidence)\n");
    for (size_t i = 0; i < image_len; i++) {
        surface_to_array(images[i], input);
        network_feed_forward(net, input, output);

        float m = output[0];
        size_t res = 0;
        for (size_t d = 0; d < 10; d++)
            if (output[d] > m) {
                m = output[d];
                res = d;
            }

        digits[i] = res;
        printf("index %zu: (%zu, %.3f)\n", i, res, m);
    }
}

void calc_offset(SDL_Surface *img, size_t *off_x, size_t *off_y) {
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

void center_image(SDL_Surface **img) {
    size_t x_off;
    size_t y_off;
    calc_offset(*img, &x_off, &y_off);
    printf("offset = (%zi, %zi)\n", x_off, y_off);

    const float x = x_off-((*img)->w / 2.0);
    const float y = y_off-((*img)->h / 2.0);

    const float trans1[] = {
        1, 0, -x,
        0, 1, -y,
        0, 0, 1,
    };
    SDL_Surface *out = utils_affine_trans(*img, trans1, (*img)->w, (*img)->h);
    SDL_FreeSurface(*img);
    *img = out;
}

int main() {
    printf("Hello wolrd!\n");

    SDL_Surface *img = load_image("digit_9.bmp");
    if (img == NULL)
        errx(EXIT_FAILURE, "failed to load image: %s", SDL_GetError());

    //IMG_SavePNG(img, "output2.png");
    center_image(&img);

    const float trans[] = {
        35.0/(float)img->w, 0, -4,
        0, 35.0/(float)img->h, -4,
        0, 0, 1,
    };

    //IMG_SavePNG(img, "output1.png");
    SDL_Surface *out = utils_affine_trans(img, trans, 28, 28);
    if (out == NULL)
        errx(EXIT_FAILURE, "affine transformation error");

    int e = IMG_SavePNG(out, "output.png");
    if (e < 0)
        errx(EXIT_FAILURE, "unable to save image: %s", SDL_GetError());

    const char *ai_path = "../ai/mnist_ai";

    struct network net;
    e = network_new_from_file(&net, ai_path);
    if (e != EXIT_SUCCESS)
        errx(EXIT_FAILURE, "unable to load ai from file");

    char digit;
    detect_digits(&net, &out, &digit, 1);

    return 0;
}
