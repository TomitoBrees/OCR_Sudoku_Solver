#include "stdio.h"
#include "err.h"

#include "network.h"

#include "SDL2/SDL.h"

const char *ai_path = "../ai/ai_mnist";

static void surface_to_array(SDL_Surface *img, float *out) {
    Uint32 *pixels = img->pixels;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    for (size_t i = 0; i < 28; i++)
        for (size_t j = 0; j < 28; j++) {
            SDL_GetRGB(pixels[i * 28 + j], img->format, &r, &g, &b);
            out[i * 28 + j] = r/255;
        }
}

// images and digits must be same length
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

int main() {
    printf("Hello wolrd!\n");

    struct network net;
    int e = network_new_from_file(&net, ai_path);
    if (e != EXIT_SUCCESS)
        errx(EXIT_FAILURE, "unable to load ai from file");

    

    return 0;
}
