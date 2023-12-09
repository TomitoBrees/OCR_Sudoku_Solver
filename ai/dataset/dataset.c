#include "dataset.h"
#include "stdlib.h"
#include "err.h"
#include "sys/types.h"
#include "dirent.h"
#include "stdio.h"

#include "utils.h"
#include "defs.h"

#include "SDL2/SDL.h"

size_t dataset_train_size = 0;
size_t dataset_test_size = 0;

struct network_item *dataset_train = NULL;
struct network_item *dataset_test = NULL;

static int load_item(const char *path, struct network_item *item) {
    //printf("loading new item: %s\n", path);

    size_t len = strlen(path); // ex: blabla.ttf_5.png
    size_t label = path[len - 5] - '0';
    if (label > 9)
        return -1;

    SDL_Surface *img = load_image_RGB(path);
    if (img == NULL)
        return -2;

    if (img->w != 28 || img->h != 28) {
        SDL_FreeSurface(img);
        return -3;
    }
    SDL_LockSurface(img);

    Uint32* pixels = img->pixels;
    Uint8 r;
    Uint8 g;
    Uint8 b;

    float *input = malloc(28 * 28 * sizeof(float));
    if (input == NULL) {
        SDL_FreeSurface(img);
        return -4;
    }
    size_t c = 0;
    for (size_t i = 0; i < 28; i++)
        for (size_t j = 0; j < 28; j++) {
            SDL_GetRGB(pixels[i * 28 + j], img->format, &r, &g, &b);
            input[c++] = (r > 150) ? 1.0f : 0.0f;
        }

    float *res = calloc(28 * 28, sizeof(float));
    if (res == NULL) {
        SDL_FreeSurface(img);
        return -5;
    }

    res[label] = 1.0f;

    item->input = input;
    item->res = res;

    SDL_UnlockSurface(img);
    SDL_FreeSurface(img);
    return 0;
}

int init_dataset(const char *folder) {
    size_t file_num = 0;
    DIR *d = opendir(folder);
    if (d == NULL)
        return -1;
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL)
        if (dir->d_name[0] != '.')
            file_num++;
        //printf("%s\n", dir->d_name);
    closedir(d);

    printf("file_num = %zu, loading...\n", file_num);
    dataset_train = malloc(file_num * sizeof(struct network_item));
    dataset_train_size = file_num;


    char full_name[100];
    strcpy(full_name, folder);
    size_t folder_len = strlen(folder);
    size_t d_len;

    size_t c = 0;
    int e;
    d = opendir(folder);
    if (d == NULL)
        return -1;
    while ((dir = readdir(d)) != NULL)
        if (dir->d_name[0] != '.') {
            d_len = strlen(dir->d_name);
            memcpy(full_name + folder_len, dir->d_name, d_len + 1);
            if ((e = load_item(full_name, dataset_train + (c++))) != 0)
                errx(EXIT_FAILURE, "failed to load item %s (err code: %i)", dir->d_name, e);
        }
    closedir(d);
    dataset_train_size = file_num;
    return 0;
}

static void dataset_print_digit(const struct network_item *item) {
    for (size_t i = 0; i < 28; i++) {
        for (size_t j = 0; j < 28; j++)
            printf("%s", (item->input[i * 28 + j] != 0) ? "##" : "..");
        printf("\n");
    }
    printf("label: %zu\n", uarr_max(item->res, 10));
}

/*int main() {
    printf("loading dataset...\n");
    const char *folder = "training_set/";
    if (init_dataset(folder) != 0)
        errx(EXIT_FAILURE, "unable to load the dataset");
    printf("dataset loaded\n");

    for (size_t i = 0; i < 10; i++) {
        dataset_print_digit((dataset_train + i));
        printf("\n");
    }

}*/

