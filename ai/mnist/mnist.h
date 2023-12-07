#pragma once

#include "network.h"

extern size_t mnist_trainning_dataset_size;
extern size_t mnist_test_dataset_size;

extern struct network_item* mnist_trainning_dataset;
extern struct network_item* mnist_test_dataset;

/*
 * seaches for the following files in folderpath:
 *     train-labels-idx1-ubyte
 *     train-images-idx3-ubyte
 *     t10k-labels-idx1-ubyte
 *     t10k-images-idx3-ubyte
 *
 */
int mnist_init(const char* folderpath);

int mnist_init_trainning(const char* labels_path, const char* images_path);
int mnist_init_test(const char* labels_path, const char* images_path);

void mnist_unload();

int mnist_test();

void mnist_print_item(const struct network_item *item);
