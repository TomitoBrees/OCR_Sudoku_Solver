#include "stdlib.h"
#include "stdio.h"
//#include "cross_err.h"

#include "edge_detection.h"

int test() {
    int a = 5;
    if (a == 5)
        err(EXIT_FAILURE, "failed to do stuff (%i)", 54);
    return 0;
}

int main() {
    return test();
}
