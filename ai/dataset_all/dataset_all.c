#include "err.h"
#include "network.h"
#include "../mnist/mnist.h"
#include "../dataset/dataset.h"


size_t data_all_size;
struct network_item *data_all;

void dataset_all_init() {
    if (mnist_init("mnist/data/") != 0)
        errx(EXIT_FAILURE, "unable to load mnist dataset");
    if (init_dataset("dataset/training_set/") != 0)
        errx(EXIT_FAILURE, "unable to load font dataset");

    data_all = malloc((mnist_trainning_dataset_size + dataset_train_size)
            * sizeof(struct network_item));
    size_t c = 0;
    for (size_t i = 0; i < mnist_trainning_dataset_size; i++) {
        data_all[c++] = mnist_trainning_dataset[i];
    }
    for (size_t i = 0; i < dataset_train_size; i++)
        data_all[c++] = dataset_train[i];
    data_all_size = c;
}
