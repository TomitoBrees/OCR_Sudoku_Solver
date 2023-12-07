#include "sys/types.h"
#include "network.h"


/*struct network_item {
    float *input; //28
    float *res;    //10
};*/


extern size_t dataset_train_size;
extern size_t dataset_test_size;

extern struct network_item *dataset_train;
extern struct network_item *dataset_test;

int init_dataset(const char *folder);

int unload_dataset();
