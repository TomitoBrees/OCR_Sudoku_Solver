#include "xor.h"
#include "string.h"
#include "stdio.h"

#include "defs.h"

size_t xor_dataset_size = 0;

struct network_item *xor_dataset = NULL;

static void add1(NETWORK_NUM *a) {
    size_t i = -1;
    while (a[++i] != 0.0) {
        a[i] = 0.0;
    }
    a[i] = 1.0;
}

static size_t is_true(const NETWORK_NUM *a, const size_t size) {
    size_t count = 0;
    for (size_t i = 0; i < size; i++)
        if (a[i] != 0)
            count++;
    return count % 2 == 0 ? 0 : 1;
}

void xor_init(const size_t input_size) {
    size_t size = XOR_DEFAULT_INPUT_SIZE;
    if (input_size != 0)
        size = 1 << input_size;

    xor_dataset_size = size;
    xor_dataset = malloc(size * sizeof(struct network_item));

    NETWORK_NUM current[input_size];
    memset(current, 0, input_size * sizeof(NETWORK_NUM));

    xor_dataset->input = calloc(input_size, sizeof(NETWORK_NUM));
    xor_dataset->res = calloc(2, sizeof(NETWORK_NUM));
    *(xor_dataset->res) = 1.0;

    for (size_t i = 1; i < size; i++) {
        add1(current);
        xor_dataset[i].res = calloc(2, sizeof(NETWORK_NUM));
        xor_dataset[i].res[is_true(current, input_size)] = 1.0;
        xor_dataset[i].input = malloc(input_size * sizeof(NETWORK_NUM));
        memcpy(xor_dataset[i].input, current, input_size * sizeof(NETWORK_NUM));
    }
}

void xor_unload() {
    if (xor_dataset == NULL)
        return;

    for (size_t i = 0; i < xor_dataset_size; i++) {
        free(xor_dataset[i].input);
        free(xor_dataset[i].res);
    }

    xor_dataset_size = 0;
    free(xor_dataset);
    xor_dataset = NULL;
}

int xor_tests() {
    xor_init(4);

    for (size_t i = 0; i < xor_dataset_size; i++) {
        DARR(xor_dataset[i].input, 4);
        DARR(xor_dataset[i].res, 2);
        printf("\n");
    }

    return 0;
}
