#pragma once

#include "stdlib.h"

#define NETWORK_NUM float

struct network {
    NETWORK_NUM** weights;
    NETWORK_NUM** biases;

    size_t *layers;
    size_t num_layers;

    size_t max_layer_size;
};

struct network_item {
    NETWORK_NUM *input;
    NETWORK_NUM *res;
};

//////////////////////////////////
/////// Creation and Saves ///////
//////////////////////////////////

// example layers: [784, 200, 50, 10]
void network_new(struct network *net,
        const size_t* layers, const size_t num_layers);

void network_free(struct network *n);

void network_fill_random(struct network *net);

int network_new_from_file(struct network *net, const char *filename);

int network_write_to_file(const struct network *net, const char *filename);

///////////////////////
/////// GETTERS ///////
///////////////////////

inline size_t network_get_weights_row(const struct network net, const size_t layer) {
    return net.layers[layer - 1];
}

inline size_t network_get_weights_columns(const struct network net, const size_t layer) {
    return net.layers[layer];
}

inline size_t network_get_input_layer_size(const struct network *net) {
    return net->layers[0];
}

inline size_t network_get_output_layer_size(const struct network *net) {
    return net->layers[net->num_layers - 1];
}

///////////////////////////
/////// AI Functions //////
///////////////////////////

void network_feed_forward(const struct network *net,
        const NETWORK_NUM *input_layer, NETWORK_NUM *output_layer);

void network_update_mini_batch(struct network *net, NETWORK_NUM eta,
        const struct network_item *items, const size_t item_count);

void network_debug(const struct network *net);

size_t network_evaluate(const struct network *net, const struct network_item *items,
        const size_t item_count);

void network_SGD(struct network *net, struct network_item *items,
        const size_t item_count, const size_t epochs_num,
        const size_t mini_batch_size, const NETWORK_NUM eta,
        const struct network_item *tests, size_t test_count);

