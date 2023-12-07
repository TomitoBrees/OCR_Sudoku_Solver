#include "network.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#include "sgemm.h"
#include "defs.h"

// example layers: [784, 200, 50, 10]
void network_new(struct network *net, const size_t* layers, const size_t num_layers) {
    size_t num_biases = num_layers - 1;

    NETWORK_NUM **biases = malloc(num_biases * sizeof(NETWORK_NUM *));
    for (size_t i = 0; i < num_biases; i++) {
        size_t num = layers[i + 1];
        biases[i] = malloc(num * sizeof(NETWORK_NUM));
    }

    NETWORK_NUM **weights = malloc(num_biases * sizeof(NETWORK_NUM *));
    for (size_t i = 0; i < num_biases; i++) {
        size_t rows = layers[i];
        size_t columns = layers[i + 1];
        weights[i] = malloc(rows * columns * sizeof(NETWORK_NUM)); 
    }

    size_t *layers_copy = malloc(num_layers * sizeof(size_t));
    memcpy(layers_copy, layers, num_layers * sizeof(size_t));

    size_t max_layer_size = 0;
    for (size_t i = 0; i < num_layers; i++)
        if (layers[i] > max_layer_size)
            max_layer_size = layers[i];

    net->weights = weights;
    net->biases = biases;
    net->layers = layers_copy;
    net->num_layers = num_layers;
    net->max_layer_size = max_layer_size;
}

void network_fill_random(struct network *net) {
    const NETWORK_NUM den = (NETWORK_NUM)((RAND_MAX/2)/0.01);
    size_t num_biases = net->num_layers - 1;
    const size_t *layers = net->layers;

    // printf("num_biases: %zu\n", num_biases);
    for (size_t i = 0; i < num_biases; i++) {
        // printf("i: %zu\n", i);
        NETWORK_NUM *arr = net->biases[i];
        size_t layer_size = layers[i + 1];
        // printf("layer_size: %zu\n", layer_size);
        // return;
        for (size_t j = 0; j < layer_size; j++) {
            arr[j] = (NETWORK_NUM)(rand() - (RAND_MAX/2))/den;
        }

        arr = net->weights[i];
        size_t rows = layers[i];
        size_t columns = layers[i + 1];
        // printf("rows: %zu\n", rows);
        // printf("columns: %zu\n", columns);
        for (size_t j = 0; j < columns; j++)
            for (size_t k = 0; k < rows; k++)
            {
                arr[k + j * rows] = (NETWORK_NUM)(rand() - (RAND_MAX/2))/den;
            }
    }
}

void network_free(struct network *net) {
    struct network n = *net;
    size_t num_biases = n.num_layers - 1;

    for (size_t i = 0; i < num_biases; i++)
        free(n.biases[i]);
    free(n.biases);

    for (size_t i = 0; i < num_biases; i++)
        free(n.weights[i]);
    free(n.weights);

    free((size_t*)n.layers);
}

int network_new_from_file(struct network *net, const char *filename) {
    int ret = EXIT_FAILURE;
    // printf("opening file...\n");
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
        return -1;
    // printf("file opened\n");
    size_t num_layers;

    // size_t layers[num_layers]; // not sure why but cannot put it after the
                                  // goto may be because it is not constant size
                                  // so the compiler cannot calculate the
                                  // address offset

    int e = fscanf(f, "%zu", &num_layers);
    // printf("num_layers: %zu\n", num_layers);
    if (e < 0 || num_layers < 1)
        goto err;
    // return 1;
    size_t *layers = malloc(num_layers * sizeof(size_t));
    size_t l_value;
    for (size_t i = 0; i < num_layers; i++) {
        l_value = -1;
        e = fscanf(f, " %zu", &l_value);
        if (e < 0 || l_value < 1)
            goto err;
        layers[i] = l_value;
    }

    network_new(net, layers, num_layers);
    struct network n = *net;

    NETWORK_NUM value;
    for (size_t layer = 0; layer < num_layers - 1; layer++) {
        NETWORK_NUM *biases = n.biases[layer];
        const size_t layer_size = layers[layer + 1];
        for (size_t i = 0; i < layer_size; i++) {
            if (fscanf(f, " %f", &value) != 1)
                goto err;

            biases[i] = value;
        }
    }

    for (size_t layer = 0; layer < num_layers-1; layer++) {
        size_t rows = layers[layer];
        size_t columns = layers[layer + 1];

        NETWORK_NUM *weights = n.weights[layer];
        for (size_t c = 0; c < columns; c++)
            for (size_t r = 0; r < rows; r++) {
                if (fscanf(f, " %f", &value) != 1)
                    goto err;
                weights[r + c * rows] = value;
            }
    }
    free(layers);

   ret = EXIT_SUCCESS;
err:
    fclose(f);
    return ret;
}

int network_write_to_file(const struct network *net, const char *filename) {
    const struct network n = *net;
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
        return -1;

    fprintf(f, "%zu", n.num_layers);
    for (size_t i = 0; i < n.num_layers; i++) {
        fprintf(f, " %zu", n.layers[i]);
    }

    for (size_t layer = 0; layer < n.num_layers - 1; layer++) {
    NETWORK_NUM *biases = n.biases[layer];
    size_t layer_size = n.layers[layer + 1];
    for (size_t i = 0; i < layer_size; i++)
        if (fprintf(f, " %f", biases[i]) < 0)
            goto err;
    }

    for (size_t layer = 0; layer < n.num_layers - 1; layer++) {
        size_t rows = n.layers[layer];
        size_t columns = n.layers[layer + 1];

        NETWORK_NUM *weights = n.weights[layer];
        for (size_t c = 0; c < columns; c++)
            for (size_t r = 0; r < rows; r++)
                if (fprintf(f, " %f", weights[r + c * rows]) < 0)
                    goto err;
    }

    fclose(f);
    return EXIT_SUCCESS;

err:
    fclose(f);
    return EXIT_FAILURE;
}



#if(NETWORK_NUM == float)
#define EXP(a) expf(a)
#else
#define EXP(a) exp(a)
#endif

#define SIGMOID(a) 1.0 / (1.0 + EXP(-a))
#define SIGMOID_PRIME(a) SIGMOID(a) * (1.0 - SIGMOID(a))

static void sigmoid(NETWORK_NUM *arr, NETWORK_NUM *res, size_t len) {
    for (size_t i = 0; i < len; i++) {
        res[i] = SIGMOID(arr[i]);
    }
}

/*
static void sigmoid_prime(NETWORK_NUM *arr, size_t len) {
    for (size_t i = 0; i < len; i++)
        arr[i] = SIGMOID_PRIME(arr[i]);
}
*/

void network_feed_forward(const struct network *net,
        const NETWORK_NUM *input_layer, NETWORK_NUM *output_layer) {
    // a = sigmoid( W * (a-1) + b )
    const struct network n = *net;

    //printf("blabla\n");

    NETWORK_NUM aa[n.max_layer_size];
    NETWORK_NUM bb[n.max_layer_size];

    NETWORK_NUM *a = aa;
    NETWORK_NUM *b = bb;

    memcpy(a, input_layer, n.layers[0] * sizeof(NETWORK_NUM));
    for (size_t i = 1; i < n.num_layers; i++) {

        // DARR(a, n.layers[i]);

        memcpy(b, n.biases[i-1], n.layers[i] * sizeof(NETWORK_NUM));
        gemm(n.layers[i], 1, n.layers[i-1], n.weights[i-1], a, b);
        sigmoid(b, b, n.layers[i]);

        NETWORK_NUM *t = a;
        a = b;
        b = t;
    }

    // DARR(a, n.layers[n.num_layers-1]);

    memcpy(output_layer, a, n.layers[n.num_layers - 1] * sizeof(NETWORK_NUM));
}

static void network_backprop(const struct network *net, const struct network_item *item,
        NETWORK_NUM *nabla_b, NETWORK_NUM *nabla_w) {

    // feedforward
    NETWORK_NUM activations[net->num_layers][net->max_layer_size];
    memcpy(activations[0], item->input, net->layers[0] * sizeof(NETWORK_NUM));

    NETWORK_NUM zs[net->num_layers - 1][net->max_layer_size];

    for (size_t layer = 0; layer < net->num_layers - 1; layer++) {
        memcpy(zs[layer], net->biases[layer], net->layers[layer + 1] *
                sizeof(NETWORK_NUM));

        // DARR(net->biases[layer], net->layers[layer + 1]);
        // PRINTZ(net->layers[layer+1]);
        // DARR(zs[layer], net->layers[layer+1]);

        gemm(net->layers[layer + 1], 1, net->layers[layer],
                net->weights[layer], activations[layer], zs[layer]);

        sigmoid(zs[layer], activations[layer+1], net->layers[layer + 1]);
    }

    // DARR(activations[net->num_layers - 1], net->layers[net->num_layers - 1]);

    // backward pass
    size_t delta_layer_size = net->layers[net->num_layers - 1];
    size_t nabla_b_index;
    size_t nabla_w_index;

    NETWORK_NUM delta[net->max_layer_size];
    for (size_t i = 0; i < delta_layer_size; i++) {
        delta[i] = (activations[net->num_layers - 1][i] - item->res[i]) *
            SIGMOID_PRIME(zs[net->num_layers - 2][i]);
    }

    // DARR(activations[net->num_layers-1], delta_layer_size);
    // DARR(delta, delta_layer_size);

    // DARR(nabla_b, delta_layer_size);

    memcpy(nabla_b, delta, delta_layer_size * sizeof(NETWORK_NUM));
    nabla_b_index = delta_layer_size;

    // DARR(nabla_b, delta_layer_size);

    gemm_n(delta_layer_size, net->layers[net->num_layers - 2], 1,
            delta, activations[net->num_layers - 2], nabla_w);
    nabla_w_index = delta_layer_size * net->layers[net->num_layers - 2];

    // DARR(nabla_w, nabla_w_index);

    // PRINTZ(nabla_b_index);
    for (size_t layer = net->num_layers - 2; layer > 0; layer--) {

        //DARR(net->weights[layer], net->layers[layer] * delta_layer_size);

        NETWORK_NUM res[net->layers[layer]];
        gemm_n(net->layers[layer], 1, delta_layer_size,
                net->weights[layer], delta, res); //////// layer - 1

        delta_layer_size = net->layers[layer];
        memcpy(delta, res, delta_layer_size * sizeof(NETWORK_NUM));

        // PRINTZ(delta_layer_size);
        // DARR(delta, delta_layer_size);

        for (size_t i = 0; i < delta_layer_size; i++) {
            delta[i] *= SIGMOID_PRIME(zs[layer - 1][i]);
        }
        // DARR(delta, delta_layer_size);

        memcpy(nabla_b + nabla_b_index, delta, delta_layer_size *
                sizeof(NETWORK_NUM));
        nabla_b_index += delta_layer_size;

        gemm_n(delta_layer_size, net->layers[layer - 1], 1,
                delta, activations[layer - 1], nabla_w + nabla_w_index); //////
        nabla_w_index += delta_layer_size * net->layers[layer - 1];

        // PRINTZ(nabla_b_index);
    }
}

void network_update_mini_batch(struct network *net, NETWORK_NUM eta,
        const struct network_item *items, const size_t item_count) {

    size_t nabla_b_size = 0;
    size_t nabla_w_size = 0;
    for (size_t layer = 1; layer < net->num_layers; layer++) {
        nabla_b_size += net->layers[layer];
        nabla_w_size += net->layers[layer - 1] * net->layers[layer];
    }

    // PRINTZ(nabla_b_size);
    // PRINTZ(nabla_w_size);

    NETWORK_NUM nabla_b[nabla_b_size];
    memset(nabla_b, 0, nabla_b_size * sizeof(NETWORK_NUM));
    NETWORK_NUM nabla_w[nabla_w_size];
    memset(nabla_w, 0, nabla_w_size * sizeof(NETWORK_NUM));

    // PRINTZ(nabla_b_size);
    // PRINTZ(nabla_w_size);

    NETWORK_NUM delta_nabla_b[nabla_b_size];
    NETWORK_NUM delta_nabla_w[nabla_w_size];
    for (size_t i = 0; i < item_count; i++) {
        network_backprop(net, items + i, delta_nabla_b, delta_nabla_w);
        for (size_t j = 0; j < nabla_b_size; j++)
            nabla_b[j] += delta_nabla_b[j];
        for (size_t j = 0; j < nabla_w_size; j++)
            nabla_w[j] += delta_nabla_w[j];
    }

    NETWORK_NUM n_item_count = (NETWORK_NUM)item_count;
    size_t b_index = 0;
    size_t w_index = 0;
    for (ssize_t layer = net->num_layers - 2; layer >= 0; layer--) {
        for (size_t i = 0; i < net->layers[layer + 1]; i++)
            net->biases[layer][i] = net->biases[layer][i]-(eta/n_item_count) *
                nabla_b[b_index++];

        size_t l = net->layers[layer] * net->layers[layer + 1];
        for (size_t i = 0; i < l; i++)
            net->weights[layer][i] = net->weights[layer][i]-(eta/n_item_count) *
                nabla_w[w_index++];
    }
}

void network_debug(const struct network *net) {
    for (size_t layer = 0; layer < net->num_layers - 1; layer++) {
        PRINTZ(layer);
        DARR(net->weights[layer], net->layers[layer] * net->layers[layer + 1]);
        DARR(net->biases[layer], net->layers[layer + 1]);
    }
}

size_t network_evaluate(const struct network *net, const struct network_item *items,
        const size_t item_count) {
    size_t score = 0;
    size_t output_size = net->layers[net->num_layers - 1];
    NETWORK_NUM output[output_size];
    for (size_t i = 0; i < item_count; i++) {
        network_feed_forward(net, items[i].input, output);
        DARR(output, output_size);
        if (uarr_max(output, output_size) == uarr_max(items[i].res, output_size))
            score++;
    }
    return score;

}

static void shuffle(void *array, size_t n, size_t size) {
    char tmp[size];
    char *arr = array;
    size_t stride = size * sizeof(char);

    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t) rand();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}

void network_SGD(struct network *net, struct network_item *items,
        const size_t item_count, const size_t epochs_num,
        const size_t mini_batch_size, const NETWORK_NUM eta,
        const struct network_item *tests, size_t test_count) {

    printf("Epoch %i: %zu/%zu\n", -1,
            network_evaluate(net, tests, test_count), test_count);

    for (size_t epoch = 0; epoch < epochs_num; epoch++) {
        shuffle(items, item_count, sizeof(struct network_item));

        size_t mini_batch_count = item_count/mini_batch_size;
        for (size_t i = 0; i < mini_batch_count; i++) {
            network_update_mini_batch(net, eta,
                    items + i*mini_batch_size, mini_batch_size);
        }
        if (tests == NULL)
            printf("Epoch %zu complete\n", epoch);
        else
            printf("Epoch %zu: %zu/%zu\n", epoch,
                    network_evaluate(net, tests, test_count), test_count);
    }
}
