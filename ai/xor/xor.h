#pragma once

#include "network.h"

#define XOR_DEFAULT_INPUT_SIZE 4

extern size_t xor_dataset_size;

extern struct network_item *xor_dataset;

// count can be zero for default
void xor_init(const size_t input_size);

void xor_unload();

int xor_tests();
