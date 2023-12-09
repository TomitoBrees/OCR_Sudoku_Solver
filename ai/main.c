#include "stdlib.h"
#include "time.h"
#include "stdio.h"

#include "mnist/mnist.h"
#include "xor/xor.h"
#include "dataset/dataset.h"

#include "dataset_all/dataset.h"

#include "defs.h"

#include "network.h"
#include "sgemm.h"

// 784-200-50-10


int test_basic_features() {
    size_t layers[] = {10, 3, 4, 10};
    size_t num_layers = sizeof(layers) / sizeof(size_t);

    struct network net;
    printf("creating new ai...\n");
    network_new(&net, layers, num_layers);

    printf("filling ai with random values...\n");
    network_fill_random(&net);

    printf("writing ai to file 'tests/ai1.txt'...\n");
    if (network_write_to_file(&net, "tests/ai1.txt") != EXIT_SUCCESS) {
        printf("failed to write ai to file!\n");
        return 1;
    }

    printf("freeing ai...\n");
    network_free(&net);

    struct network net2;
    printf("creating new ai2 from file 'tests/ai1.txt'...\n");
    if (network_new_from_file(&net2, "tests/ai1.txt") != EXIT_SUCCESS) {
        printf("failed to create ai from file\n");
        return 1;
    }


    printf("writing ai2 into file 'tests/ai2.txt'..\n");
    if (network_write_to_file(&net2, "tests/ai2.txt") != EXIT_SUCCESS) {
        printf("failed to write ai2 to file!\n");
        return 1;
    }

    printf("freeing ai2...\n");
    network_free(&net2);

    printf("done!\n");
    return 0;
}

int test_matrix_mul() {
    /*
    float t1[] = {
1, 2,
3, 4
    };

    float t2[] = {
5, 6,
7, 8
    };

    float t3[6 * 6] = {0};
    gemm(2, 2, 2, t1, t2, t3);

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            printf("%05f ", t3[j + i * 2]);
        }
        printf("\n");
    }
    printf("\n");

    for (size_t i = 0; i < 4; i++)
        t3[i] = 0;
    gemm_btrans(2, 2, 2, t1, t2, t3);
    

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            printf("%05f ", t3[j + i * 2]);
        }
        printf("\n");
    }
    */

    // both are 6 by 6
    float m1[] = {
2.75800, 2.77813, 3.91709, 4.71389, 0.29063, 3.97301,
6.49224, 0.06555, 9.73906, 0.76130, 1.97712, 8.65483,
6.86652, 7.81747, 3.73782, 6.96679, 2.04967, 8.28670,
6.09005, 3.26225, 1.34548, 2.28342, 0.06307, 7.36111,
9.39896, 7.75119, 1.36641, 3.14165, 9.89499, 8.01300,
0.27012, 2.48120, 0.47794, 8.23694, 3.17899, 0.33676
    };

    float m2[] = {
7.26013, 8.48558, 1.93876, 5.99691, 0.47854, 4.74496,
8.61267, 9.45141, 7.28757, 7.68691, 5.43621, 6.90993,
8.34584, 0.11339, 0.81359, 9.93910, 5.27722, 3.61971,
4.38917, 3.37281, 5.81245, 6.00658, 5.91182, 2.37141,
6.56238, 5.90426, 7.41051, 4.14499, 4.04164, 3.36640,
3.17571, 8.82757, 2.83906, 4.22425, 0.36158, 0.05471
    };

    float m3[6 * 6] = {0};

    double time1 = clock();
    gemm(6, 6, 6, m1, m2, m3);
    double time2 = clock();

    printf("time elapsed: %f s\n\n", (time2 - time1) / CLOCKS_PER_SEC);
    for (size_t i = 0; i < 6; i++) {
        for (size_t j = 0; j < 6; j++) {
            printf("%05f ", m3[j + i * 6]);
        }
        printf("\n");
    }

    return 0;
}

int test_mul2() {
    float m1[] = {
0.080059, 0.005420, 0.031895, 0.034986, 0.057613, 0.005692, 0.028581, 0.063540, 0.082708, 0.098349,
0.046208, 0.025642, 0.046178, 0.074413, 0.071117, 0.033998, 0.044737, 0.089312, 0.050365, 0.057433,
0.091873, 0.092660, 0.014841, 0.014072, 0.050829, 0.091855, 0.001001, 0.068630, 0.055260, 0.052565,
0.061870, 0.061898, 0.033232, 0.089297, 0.007904, 0.050603, 0.000137, 0.049065, 0.068416, 0.085754,
0.036219, 0.041404, 0.014066, 0.007562, 0.014246, 0.017771, 0.031150, 0.085113, 0.093048, 0.053499,
0.088275, 0.078857, 0.042460, 0.035386, 0.086004, 0.054122, 0.015387, 0.008115, 0.030189, 0.023698,
0.032502, 0.082565, 0.019504, 0.047645, 0.095312, 0.059145, 0.087921, 0.068465, 0.093146, 0.069329,
0.092618, 0.009757, 0.019718, 0.088455, 0.088107, 0.016446, 0.064904, 0.022941, 0.077047, 0.064779,
0.097299, 0.025056, 0.078491, 0.020118, 0.003253, 0.038316, 0.004089, 0.070779, 0.097983, 0.026728,
0.082998, 0.015897, 0.074328, 0.090890, 0.025956, 0.035591, 0.004898, 0.024162, 0.069323, 0.021415,
0.063115, 0.012012, 0.089584, 0.095340, 0.078829, 0.027607, 0.028993, 0.095291, 0.094168, 0.026859,
0.022987, 0.055446, 0.035151, 0.095999, 0.069961, 0.059374, 0.064803, 0.054799, 0.034071, 0.094143,
0.033299, 0.068609, 0.021665, 0.083535, 0.090817, 0.072289, 0.021964, 0.028495, 0.078716, 0.059188,
0.052629, 0.032917, 0.064663, 0.038279, 0.000845, 0.042915, 0.037309, 0.012601, 0.000528, 0.077706,
0.026182, 0.082089, 0.036280, 0.080181, 0.062395, 0.023966, 0.014762, 0.065392, 0.045787, 0.053798,
0.094034, 0.094943, 0.034159, 0.020304, 0.011545, 0.055538, 0.020176, 0.037144, 0.001401, 0.069741,
0.011612, 0.004669, 0.041478, 0.017051, 0.080532, 0.024909, 0.021061, 0.093530, 0.097037, 0.062795,
0.069625, 0.087939, 0.013221, 0.056819, 0.037364, 0.002686, 0.012937, 0.032231, 0.068096, 0.097574,
0.093008, 0.019910, 0.055193, 0.084530, 0.091507, 0.033992, 0.021119, 0.051466, 0.028742, 0.011451,
0.011267, 0.043333, 0.076565, 0.066689, 0.042628, 0.078536, 0.021583, 0.064312, 0.098004, 0.028837,
0.038017, 0.063176, 0.006330, 0.040855, 0.035679, 0.034861, 0.099796, 0.027277, 0.098819, 0.009680,
0.088287, 0.005939, 0.081390, 0.053356, 0.041890, 0.014884, 0.074404, 0.027467, 0.021998, 0.081500,
0.010971, 0.036839, 0.051686, 0.073910, 0.024662, 0.016950, 0.065972, 0.012702, 0.026807, 0.064516,
0.032572, 0.071139, 0.018381, 0.064351, 0.083813, 0.041954, 0.002075, 0.015653, 0.050258, 0.098907,
0.010193, 0.010834, 0.036784, 0.031147, 0.013984, 0.069982, 0.031593, 0.078610, 0.030049, 0.084155,
0.088907, 0.046443, 0.089453, 0.070559, 0.075900, 0.026728, 0.085141, 0.092782, 0.024516, 0.005518,
0.063320, 0.049178, 0.078805, 0.083343, 0.074386, 0.082559, 0.078362, 0.087350, 0.049062, 0.014264,
0.068566, 0.003137, 0.015357, 0.055831, 0.078164, 0.004465, 0.021970, 0.001251, 0.054891, 0.008927,
0.083755, 0.053096, 0.099704, 0.037724, 0.014881, 0.085482, 0.090283, 0.007672, 0.065340, 0.097137,
0.034404, 0.033567, 0.026356, 0.057585, 0.043849, 0.068993, 0.020405, 0.099658, 0.069402, 0.063787,
0.041398, 0.061382, 0.087649, 0.066173, 0.060714, 0.030439, 0.080126, 0.047798, 0.064650, 0.094272,
0.095502, 0.079699, 0.038963, 0.048970, 0.035371, 0.037407, 0.066100, 0.070116, 0.054329, 0.063256,
0.050737, 0.098593, 0.014896, 0.064061, 0.039283, 0.044639, 0.087985, 0.072253, 0.048949, 0.063131,
0.039476, 0.048628, 0.071987, 0.003311, 0.093567, 0.054833, 0.070803, 0.086050, 0.013492, 0.073666,
0.029447, 0.011747, 0.090262, 0.005393, 0.077248, 0.052074, 0.080020, 0.018162, 0.050066, 0.028581,
0.080667, 0.080605, 0.004929, 0.031358, 0.088382, 0.054836, 0.053591, 0.080877, 0.024613, 0.077911,
0.030644, 0.086096, 0.074252, 0.009873, 0.061849, 0.001999, 0.025507, 0.076562, 0.048280, 0.039674,
0.052220, 0.085217, 0.031916, 0.032029, 0.029142, 0.041118, 0.058293, 0.008329, 0.020872, 0.043303,
0.001856, 0.080633, 0.035038, 0.087481, 0.069643, 0.095260, 0.080367, 0.054357, 0.048894, 0.052449,
0.020264, 0.011380, 0.014246, 0.047279, 0.062523, 0.006140, 0.060125, 0.035008, 0.075777, 0.079431,
0.073940, 0.073965, 0.073379, 0.076226, 0.015735, 0.049092, 0.055229, 0.082601, 0.019922, 0.029286,
0.048289, 0.066756, 0.084689, 0.061644, 0.036317, 0.052266, 0.064724, 0.055751, 0.019498, 0.001706,
0.032926, 0.061147, 0.094595, 0.038710, 0.026637, 0.091049, 0.096210, 0.086670, 0.044090, 0.095526,
0.089816, 0.052138, 0.006684, 0.037181, 0.057170, 0.031095, 0.052702, 0.072524, 0.064742, 0.066192,
0.078079, 0.092114, 0.055754, 0.033076, 0.089886, 0.022614, 0.056499, 0.083975, 0.088147, 0.043913,
0.037532, 0.026600, 0.041557, 0.024369, 0.020377, 0.066558, 0.072820, 0.027384, 0.070354, 0.091330,
0.019138, 0.092361, 0.066173, 0.024003, 0.083560, 0.043214, 0.086920, 0.024848, 0.042747, 0.027549,
0.007019, 0.068868, 0.026823, 0.039613, 0.039872, 0.077447, 0.036912, 0.030177, 0.020707, 0.032234,
0.016010, 0.056450, 0.069362, 0.049376, 0.038777, 0.075155, 0.023518, 0.042216, 0.035923, 0.031901,
0.083126, 0.050612, 0.018787, 0.049959, 0.056545, 0.064974, 0.019718, 0.071584, 0.049080, 0.023133
    };

    float m2[] = {
0.129719,
0.128701,
0.127910,
0.126398,
0.127229,
-0.124811,
0.128671,
0.126345,
0.128949,
0.126005
    };

    float m3[50];
    //memset(m3, 0, 50 * sizeof(float));

    PRINTZ(sizeof(m1) / sizeof(float));
    PRINTZ(sizeof(m2) / sizeof(float));

    gemm_n(50, 1, 10, m1, m2, m3);

    DARR(m3, 50);

    return 0;
}

int test_feedforward() {
    // size_t layers[] = {784, 200, 50, 10};
    size_t layers[] = {784, 10};
    struct network net;
    network_new(&net, layers, sizeof(layers) / sizeof(size_t));
    network_fill_random(&net);

    NETWORK_NUM input[784];
    for (size_t i = 0; i < 784; i++)
        input[i] = (NETWORK_NUM)rand()/(RAND_MAX/1);
    NETWORK_NUM output[10] = {0};

    printf("input  = { ");
    for (size_t i = 0; i < 14; i++)
        printf("%f ", input[i]);
    printf("... }\n");

    network_feed_forward(&net, input, output);

    printf("output = { ");
    for (size_t i = 0; i < 10; i++)
        printf("%f ", output[i]);
    printf("}\n");

    network_free(&net);

    return 0;
}

int test_mnist() {
    return mnist_test();
}

int test_backprop() {
    const ssize_t test_count = 10;
    const size_t test_offset = 1;
    const size_t iteration_num = 50;

    if (mnist_init("mnist/data/") != 0) {
        printf("failed to load mnist dataset\n");
        return -1;
    }
    const size_t layers[] = {28 * 28, 30, 10};
    struct network net;
    network_new(&net, layers, sizeof(layers) / sizeof(size_t));

    network_fill_random(&net);
    DARR(net.biases[0], 10);

    NETWORK_NUM output[10];

    for (size_t i = 0; i < test_count; i++) {
        mnist_print_item(mnist_trainning_dataset + test_offset + i);
        network_feed_forward(&net, (mnist_trainning_dataset + test_offset + i)->input, output);
        printf("Output (Random):\n{ ");
        for (size_t ii = 0; ii < 10; ii++)
            printf("%f ", output[ii]);
        printf("}\n\n");
    }

    const NETWORK_NUM eta = 3.0;
    printf("learning (eta: %f)...\n", eta);
    for (size_t i = 0; i < iteration_num; i++) {
        network_update_mini_batch(&net, eta, mnist_trainning_dataset + test_offset, test_count);
    }

    size_t score = 0;
    for (size_t t = 0; t < test_count; t++) {
       // print_test(&net, mnist_trainning_dataset + test_offset + t);
        struct network_item *item = mnist_trainning_dataset + test_offset + t;
        mnist_print_item(item);
        network_feed_forward(&net, item->input, output);
        printf("Output: { ");
        for (size_t i = 0; i < 10; i++)
            printf("%f ", output[i]);
        size_t s = uarr_max(output, 10);
        if (s == uarr_max(item->res, 10))
            score++;
        printf("} res: %zi\n", s);
    }

    printf("score: (%zu/%zu)\n", score, test_count);

    printf("eval score: %zu\n", network_evaluate(&net, mnist_trainning_dataset + test_offset, test_count));

    mnist_unload();
    network_free(&net);
    return 0;
}

int test_xor() {
    return xor_tests();
}

int test_xor_backprop() {
    const size_t xor_size = 2;
    xor_init(xor_size);

    struct network_item tt = xor_dataset[1];
    xor_dataset[1] = xor_dataset[3];
    xor_dataset[3] = tt;

    size_t test_count = 2;
    const size_t test_offset = 0;
    const size_t iteration_num = 100;
    const NETWORK_NUM eta = 10.0;

    const size_t layers[] = {xor_size, 4, 2};
    struct network net;
    network_new(&net, layers, sizeof(layers)/sizeof(size_t));

    network_fill_random(&net);

    NETWORK_NUM output[2];

    for (size_t i = 0; i < test_count; i++) {
        DARR((xor_dataset + test_offset + i)->input, xor_size);
        network_feed_forward(&net, (xor_dataset + test_offset + i)->input, output);
        printf("Output (Random): { %f %f }\n", output[0], output[1]);
    }


    //DARR(net.biases[0], 2);
    //DARR(net.weights[0], 2 * 2); 

    printf("learning (eta: %f)...\n", eta);
    for (size_t i = 0; i < iteration_num; i++) {
        network_update_mini_batch(&net, eta, xor_dataset + test_offset, test_count);
        //network_debug(&net);
        //printf("\n");
    }

    printf("score: %zu\n", network_evaluate(&net, xor_dataset + test_offset, test_count));

    test_count = 4;
    printf("learning (eta: %f)...\n", eta);
    for (size_t i = 0; i < iteration_num; i++) {
        network_update_mini_batch(&net, eta, xor_dataset + test_offset, test_count);
        //network_debug(&net);
        //printf("\n");
    }

    printf("score: %zu\n", network_evaluate(&net, xor_dataset + test_offset, test_count));

    test_count = 4;
    printf("learning (eta: %f)...\n", eta);
    for (size_t i = 0; i < iteration_num; i++) {
        network_update_mini_batch(&net, eta, xor_dataset + test_offset, test_count);
        //network_debug(&net);
        //printf("\n");
    }

    printf("score: %zu\n", network_evaluate(&net, xor_dataset + test_offset, test_count));

    size_t score = 0;
    for (size_t i = 0; i < test_count; i++) {
        struct network_item *item = xor_dataset + test_offset + i;
        DARR(item->input, xor_size);
        network_feed_forward(&net, item->input, output);
        size_t s = uarr_max(output, 2);
        if (s == uarr_max(item->res, 2))
            score++;
        printf("Output: { %f %f } %zu\n", output[0], output[1], s);
    }

    printf("score: (%zu/%zu)\n", score, test_count);
    xor_unload();
    network_free(&net);
    return 0;
}

int test_SGD() {
    if (mnist_init("mnist/data/") != 0) {
        printf("failed to load mnist dataset\n");
        return -1;
    }

    size_t layers[] = {28 * 28, 30, 10};
    struct network net;
    network_new(&net, layers, sizeof(layers)/sizeof(size_t));
    network_fill_random(&net);

    const size_t epochs_num = 20 ; // 100
    const size_t mini_batch_size = 100; // 100
    const NETWORK_NUM eta = 3.0; // 1.0;


    // network_SGD(&net, mnist_trainning_dataset, 60000, epochs_num, mini_batch_size,
    //         eta, NULL, 0);
    network_SGD(&net, mnist_trainning_dataset, 60000, epochs_num, mini_batch_size,
            eta, mnist_test_dataset, mnist_test_dataset_size);
   /*for (size_t i = 0; i < epochs_num; i++) {
        network_update_mini_batch(&net, eta, mnist_trainning_dataset, 100);
        printf("score: %zu\n", network_evaluate(&net, mnist_trainning_dataset, 100));
    }*/

    printf("score (trainning dataset): %zu\n",
            network_evaluate(&net, mnist_trainning_dataset, 60000));
    printf("score (test dataset): %zu\n",
            network_evaluate(&net, mnist_test_dataset, 10000));

    if (network_write_to_file(&net, "mnist_ai") != 0) {
        printf("failed to write ai\n");
    }

    mnist_unload();
    return 0;
}

int test_SGD_xor() {
    xor_init(2);

    size_t layers[] = {2, 4, 2};
    struct network net;
    network_new(&net, layers, sizeof(layers)/sizeof(size_t));
    network_fill_random(&net);

    const size_t epochs_num = 20 ; // 100
    const size_t mini_batch_size = 4; // 100
    const NETWORK_NUM eta = 15.0; // 1.0;


    network_SGD(&net, xor_dataset, xor_dataset_size - 2, epochs_num, mini_batch_size,
            eta, xor_dataset, xor_dataset_size - 2);

    printf("score: %zu\n",
            network_evaluate(&net, xor_dataset, xor_dataset_size));

    network_write_to_file(&net, "xor_ai.txt");

    xor_unload();
    return 0;
}

int test_ilan() {
    if (mnist_init("mnist/data") != 0) {
        printf("unable to load mnist dataset\n");
        return 1;
    }

    size_t layers[] = {784, 30, 10};
    struct network net;
    network_new(&net, layers, sizeof(layers)/sizeof(size_t));
    network_fill_random(&net);

    for (size_t i = 0; i < net.num_layers - 1; i++) {
        DARR(net.weights[i], net.layers[i] * net.layers[i+1]);
        printf("\n");
    }

    printf("\n\n\n\n");
    for (size_t i = 0; i < net.num_layers - 1; i++) {
        DARR(net.biases[i], net.layers[i+1]);
        printf("\n");
    }
    return 0;
}

int test_SGD_font() {
    if (init_dataset("dataset/training_set/") != 0) {
        printf("ERR: unable to load dataset\n");
        return -1;
    }

    size_t layers[] = {28 * 28, 30, 10};
    struct network net;
    network_new(&net, layers, sizeof(layers)/sizeof(size_t));
    network_fill_random(&net);

    const size_t epochs_num = 20 ; // 100
    const size_t mini_batch_size = 100; // 100
    const NETWORK_NUM eta = 3.0; // 1.0;


    network_SGD(&net, dataset_train, dataset_train_size, epochs_num, mini_batch_size,
             eta, NULL, 0);
    //network_SGD(&net, dataset_train, dataset_train_size, epochs_num, mini_batch_size,
    //        eta, dataset_test, dataset_test_size);

    printf("score (trainning dataset): %zu\n",
            network_evaluate(&net, dataset_train, dataset_train_size));
    printf("score (test dataset): %zu\n",
            network_evaluate(&net, dataset_test, dataset_test_size));

    if (network_write_to_file(&net, "dataset_ai") != 0) {
        printf("failed to write ai\n");
    }

    mnist_unload();

    return 0;
}

int test_SGD_all() {
    dataset_all_init();

    size_t layers[] = {28 * 28, 30, 10};
    struct network net;
    network_new(&net, layers, sizeof(layers)/sizeof(size_t));
    network_fill_random(&net);

    const size_t epochs_num = 20 ; // 100
    const size_t mini_batch_size = 100; // 100
    const NETWORK_NUM eta = 3.0; // 1.0;

    network_SGD(&net, data_all, data_all_size, epochs_num, mini_batch_size,
             eta, NULL, 0);
    //network_SGD(&net, dataset_train, dataset_train_size, epochs_num, mini_batch_size,
    //        eta, dataset_test, dataset_test_size);

    printf("score (trainning dataset): %zu\n",
            network_evaluate(&net, data_all, data_all_size));
    //printf("score (test dataset): %zu\n",
    //        network_evaluate(&net, dataset_test, dataset_test_size));

    if (network_write_to_file(&net, "dataset_all_ai") != 0) {
        printf("failed to write ai\n");
    }

    return 0;
}

int main() {
    //srand(time(NULL));
    srand(0);

    printf("Hello world!\n");
    int res;
    double time1 = clock();


    // return test_basic_features();
    // return test_matrix_mul();
    // return test_feedforward();
    // return test_mnist();
    // return test_mul2();

    // return test_backprop();
    // return test_xor();
    // return test_xor_backprop();

    // res = test_SGD();
    // res = test_SGD_font();
    res = test_SGD_all();
    // return test_SGD_xor();

    // res = test_ilan();

    double time2 = clock();
    printf("time elapsed: %f s\n\n", (time2 - time1) / CLOCKS_PER_SEC);
    return res;
}
