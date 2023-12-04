#include "stdlib.h"
#include "stdio.h"
#include "err.h"

#include "edge_detection.h"
#include "rotate.h"
#include "network.h"

int test() {
    int a = 5;
    if (a == 5)
        err(EXIT_FAILURE, "failed to do stuff (%i)", 54);
    return 0;
}

int main() {
    int i = test();
    edgeDetection(NULL, NULL);
}
