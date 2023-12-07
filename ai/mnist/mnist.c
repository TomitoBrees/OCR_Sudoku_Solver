#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "mnist.h"
#include "defs.h"

size_t mnist_trainning_dataset_size = -1;
size_t mnist_test_dataset_size = -1;

struct network_item* mnist_trainning_dataset = NULL;
struct network_item* mnist_test_dataset = NULL;

static const char train_labels_filename[] = "/train-labels-idx1-ubyte";
static const char train_images_filename[] = "/train-images-idx3-ubyte";
static const char test_labels_filename[] = "/t10k-labels-idx1-ubyte";
static const char test_images_filename[] = "/t10k-images-idx3-ubyte";

///////////////////////////////////////////////////////////////////////////////
////////////////////////   INTERNAL FILE LOADERS   ////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static long get_next_uint32(FILE *f) {
    unsigned char v[4];
    if (fread(v, sizeof(char), sizeof(v) / sizeof(char), f) != 4)
        return -1;
    unsigned long res = 0;
    res |= v[0] << 24;
    res |= v[1] << 16;
    res |= v[2] << 8;
    res |= v[3];
    return (long)res;
}

static int mnist_load_idx1(
        const char *path,
        size_t *count,
        unsigned char **labels
        ) {
    FILE *f = fopen(path, "rb");

    if (f == NULL) {
        //printf("file not found\n");
        return -1;
    }

    long v;
    unsigned char *vals = NULL;
    long c;

    if ((v = get_next_uint32(f)) == -1 || v != 0x00000801) {
        //printf("cannot read or invalid magic number (0x%lx)\n", v);
        goto err;
    }

    c = get_next_uint32(f);
    if (c == -1) {
        //printf("unable to read amount of labels\n");
        goto err;
    }

    //printf("c: %li (0x%lX)\n", c, c);
    vals = malloc(c * sizeof(unsigned char));
    if (vals == NULL) {
        goto err;
    }

    unsigned char val;
    for (size_t i = 0; i < (unsigned long)c; i++) {
        if (fread(&val, sizeof(val), 1, f) != 1) {
            //printf("(i: %zu) cannot read char (%c)\n", i, val);
            goto err;
        }
        vals[i] = val;
    }

    fclose(f);
    *count = c;
    *labels = vals;
    return 0;

err:
    fclose(f);
    *count = 0;
    *labels = NULL;
    if (vals != NULL)
        free(vals);
    return -1;

}

static int mnist_load_idx3(const char *path, size_t *count,
        float ***pixels) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }

    long v;
    if ((v = get_next_uint32(f)) == -1 || v != 0x00000803) {
        printf("invalid magic number\n");
        goto err;
    }

    long c = get_next_uint32(f);
    if (c == -1) {
        printf("negative count\n");
        goto err;
    }

    if (get_next_uint32(f) != 28 || get_next_uint32(f) != 28) {
        printf("invalid matrix dimensions");
        goto err;
    }

    size_t l = 0;
    float **vals = malloc(c * sizeof(float*));
    for (size_t i = 0; i < (unsigned long)c; i++) {
        unsigned char val[28 * 28];
        if (fread(val, sizeof(unsigned char), 28 * 28, f) != 28 * 28) {
            goto errp;
        }

        float *image = malloc(28 * 28 * sizeof(float));
        for (size_t j = 0; j < 28 * 28; j++)
            image[j] = (float)((float)(val[j]) / 255.0);

        vals[i] = image;
        l++;
    }

    fclose(f);
    *count = c;
    *pixels = vals;
    return 0;

errp:
    for (size_t i = 0; i < l; i++)
        free(vals[i]);
    free(vals);
err:
    *count = 0;
    *pixels = NULL;
    return -1;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// TESTS AND DEBUGS ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void print_digit(float *pixels) {
    const char alph[] = " ░▒▓█"; // 5 elements
    for (size_t i = 0; i < 28; i++) {
        for (size_t j = 0; j < 28; j++) {
            size_t ii = pixels[i * 28 + j] * 5;
            printf("%c%c", alph[ii], alph[ii]);
        }
        printf("\n");
    }
}

static int test_idx1() {
    const char* file = 
        "mnist/data/train-labels.idx1-ubyte";
    unsigned char* labels;
    size_t count;
    if (mnist_load_idx1(file, &count, &labels) != 0) {
        printf("failed to load labels");
        return -1;
    }

    printf("loaded %zu labels\nprinting first 20 labels...\n", count);
    for (size_t i = 0; i < 20; i++) {
        printf("%2zu: %c\n", i, labels[i] + '0');
    }
    printf("done!\n");

    free(labels);
    return 0;
}

static int test_idx3() {
    const char *file = 
        "C:\\Users\\Luca\\Desktop\\OCR\\ai\\mnist\\data\\train-images.idx3-ubyte";
    float **pixels;
    size_t count;
    if (mnist_load_idx3(file, &count, &pixels) != 0) {
        printf("failed to load pixels\n");
        return -1;
    }

    printf("loaded %zu digits\nprinting first 10...\n", count);
    for (size_t i = 0; i < 10; i++) {
        print_digit(pixels[i]);
        printf("\n");
        free(pixels[i]);
    }

    for (size_t i = 10; i < count; i++)
        free(pixels[i]);

    printf("done!\n");
    free(pixels);
    return 0;
}

static int test_init() {
    const char *folderpath = "C:\\Users\\Luca\\Desktop\\OCR\\ai\\mnist\\data";
    if (mnist_init(folderpath) != 0) {
        printf("failed to load files\n");
        return -1;
    }

    printf("successfully loaded %zu trainning and %zu test examples\n",
            mnist_trainning_dataset_size,
            mnist_test_dataset_size);

    printf("printing first 10 trainning exmaples...\n");
    for (size_t i = 0; i < 10; i++) {
        printf("label: %zi\nimage:\n", arr_max(mnist_trainning_dataset[i].res, 10));
        print_digit(mnist_trainning_dataset[i].input);
    }

    printf("=======================================================\n\
printing first 10 test examples...\n");
    for (size_t i = 0; i < 10; i++) {
        printf("label: %zi\nimages:\n", arr_max(mnist_test_dataset[i].res, 10));
        print_digit(mnist_test_dataset[i].input);
    }

    printf("=======================================================\n\
printing last 10 test examples...\n");
    for (size_t i = mnist_test_dataset_size - 11; i < mnist_test_dataset_size; i++) {
        printf("label: %zi\nimages:\n", arr_max(mnist_test_dataset[i].res, 10));
        print_digit(mnist_test_dataset[i].input);
    }
    printf("unloading data...\n");
    mnist_unload();
    printf("done!\n");
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// EXPORTED FUNCTIONS //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int mnist_init(const char* folderpath) {
    size_t l = strlen(folderpath);

    char train_lab[l + sizeof(train_labels_filename)];
    char train_img[l + sizeof(train_images_filename)];

    char test_lab[l + sizeof(test_labels_filename)];
    char test_img[l + sizeof(test_images_filename)];

    strcpy(train_lab, folderpath);
    strcpy(train_img, folderpath);
    strcpy(test_lab, folderpath);
    strcpy(test_img, folderpath);

    strcat(train_lab, train_labels_filename);
    strcat(train_img, train_images_filename);
    strcat(test_lab, test_labels_filename);
    strcat(test_img, test_images_filename);

    //printf("train_lab: %s\n", train_lab);
    //printf("train_img: %s\n", train_img);
    //printf("test_lab: %s\n", test_lab);
    //printf("test_img: %s\n", test_img);

    int res = mnist_init_trainning(train_lab, train_img);
    if (res != 0)
        return res;
    return mnist_init_test(test_lab, test_img);
}

int mnist_init_trainning(const char *labels_path, const char *images_path) {
    size_t lab_count;
    unsigned char *labels;

    size_t img_count;
    float **images;

    int res = mnist_load_idx1(labels_path, &lab_count, &labels);
    if (res != 0)
        return res;

    res = mnist_load_idx3(images_path, &img_count, &images);
    if (res != 0) {
        free(labels);
        return res;
    }

    if (lab_count != img_count) {
        //printf("mismatching label and image count (%zu labels and %zu images)\n",
        //        lab_count, img_count);
        free(labels);
        for (size_t i = 0; i < img_count; i++)
            free(images[i]);
        free(images);
        return -1;
    }


    mnist_trainning_dataset = malloc(lab_count * sizeof(struct network_item));
    mnist_trainning_dataset_size = lab_count;
    for (size_t i = 0; i < lab_count; i++) {
        float *labs = calloc(10, sizeof(float));
        labs[labels[i]] = 1.0;
        mnist_trainning_dataset[i].res = labs;
        mnist_trainning_dataset[i].input = images[i];
    }
    free(labels);
    free(images);
    return 0;
}

int mnist_init_test(const char *labels_path, const char *images_path) {
    size_t lab_count;
    unsigned char *labels;

    size_t img_count;
    float **images;

    int res = mnist_load_idx1(labels_path, &lab_count, &labels);
    if (res != 0)
        return res;

    res = mnist_load_idx3(images_path, &img_count, &images);
    if (res != 0) {
        free(labels);
        return res;
    }

    if (lab_count != img_count) {
        //printf("mismatching label and image count (%zu labels and %zu images)\n",
        //        lab_count, img_count);
        free(labels);
        for (size_t i = 0; i < img_count; i++)
            free(images[i]);
        free(images);
        return -1;
    }


    mnist_test_dataset = malloc(lab_count * sizeof(struct network_item));
    mnist_test_dataset_size = lab_count;
    for (size_t i = 0; i < lab_count; i++) {
        float *labs = calloc(10, sizeof(float));
        labs[labels[i]] = 1.0;
        mnist_test_dataset[i].res = labs;
        mnist_test_dataset[i].input = images[i];
    }
    free(labels);
    free(images);
    return 0;
}

void mnist_unload() {
    if (mnist_trainning_dataset != NULL) {
        for (size_t i = 0; i < mnist_trainning_dataset_size; i++) {
            free(mnist_trainning_dataset[i].input);
            free(mnist_trainning_dataset[i].res);
        }
        mnist_trainning_dataset_size = 0;
        free(mnist_trainning_dataset);
        mnist_trainning_dataset = NULL;
    }

    if (mnist_test_dataset != NULL) {
        for (size_t i = 0; i < mnist_test_dataset_size; i++) {
            free(mnist_test_dataset[i].input);
            free(mnist_test_dataset[i].res);
        }
        mnist_test_dataset_size = 0;
        free(mnist_test_dataset);
        mnist_test_dataset = NULL;
    }
}

int mnist_test() {
    int i = 0;
    if (i == 0)
        return test_idx1();
    else if (i == 1)
        return test_idx3();
    else if (i == 2)
        return test_init();
    return 1;
}

void mnist_print_item(const struct network_item *item) {
    printf("label: %zu\nimages:\n", arr_max(item->res, 10));
    print_digit(item->input);
}
