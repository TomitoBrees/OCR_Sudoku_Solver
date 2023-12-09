#include "stdio.h"
#include "err.h"

#include "network.h"
#include "utils.h"

#include "SDL2/SDL.h"

#include "ai_detect.h"

/*
int main(int argc, char **argv) {
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: main <filename>");
    printf("Hello wolrd!\n");

    SDL_Surface *img = load_image_RGB(argv[1]);
    if (img == NULL)
        errx(EXIT_FAILURE, "failed to load image: %s", SDL_GetError());

    const char *ai_path = "../ai/dataset_ai";
    struct network net;
    int e = network_new_from_file(&net, ai_path);
    if (e != EXIT_SUCCESS)
        errx(EXIT_FAILURE, "unable to load ai from file");

    char digit;
    detect_digits(&net, &img, &digit, 1);

    return 0;
}
 */
