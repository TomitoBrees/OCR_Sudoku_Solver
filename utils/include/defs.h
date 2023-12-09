#pragma once

#include "sys/types.h"

#ifdef DEBUG

#include "stdio.h"

#define DARR(a, len) printf("[DEBUG] " #a ": { "); for (size_t iii = 0; iii < len; iii++) printf("%f ", a[iii]); printf("}\n");
#define DARR_ALL(format, a, len) printf("[DEBUG] "#a": { ");\
    for (size_t iii = 0; iii < len; iii++) printf(format " ", a[iii]);\
    printf("}\n");

#define PRINTZ(a) printf("[DEBUG] "#a": %zu\n", a);
#define PRINTF(a) printf("[DEBUG] "#a": %f\n", a);
#define PRINTD(...) printf("[DEBUG] ");printf(__VA_ARGS__);

#else

#define DARR(a, len)
#define DARR_ALL(format, a, len)
#define PRINTZ(a)
#define PRINTF(a)
#define PRINTD(...)

#endif

static inline ssize_t arr_max(float *a, size_t l) {
    if (l == 0)
        return -1;
    float max = a[0];
    ssize_t res = 0;
    for (size_t i = 1; i < l; i++)
        if (a[i] > max) {
            max = a[i];
            res = i;
        }
    return res;
}

static inline ssize_t arr_min(float *a, size_t l) {
    if (l == 0)
        return -1;
    float min = a[0];
    ssize_t res = 0;
    for (size_t i = 1; i < l; i++)
        if (a[i] > min) {
            min = a[i];
            res = i;
        }
    return res;
}

#define uarr_max(a, l) (size_t)arr_max(a, l)
