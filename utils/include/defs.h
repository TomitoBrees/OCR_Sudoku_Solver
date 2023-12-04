#pragma once

#include "stdlib.h"

#define NETWORK_NUM float

#ifdef DEBUG

#include "stdio.h"

#define DARR(a, len) printf("[DEBUG] " #a ": { "); for (size_t iii = 0; iii < len; iii++) printf("%f ", a[iii]); printf("}\n");

#define PRINTZ(a) printf("[DEBUG] "#a": %zu\n", a);
#define PRINTF(a) printf("[DEBUG] "#a": %f\n", a);

#else

#define DARR(a, len)
#define PRINTZ(a)
#define PRINTF(a)

#endif

static inline ssize_t max(NETWORK_NUM *a, NETWORK_NUM l) {
    if (l == 0)
        return -1;
    NETWORK_NUM max = a[0];
    ssize_t res = 0;
    for (size_t i = 1; i < l; i++)
        if (a[i] > max) {
            max = a[i];
            res = i;
        }
    return res;
}

#define umax(a, l) (size_t)max(a, l)

