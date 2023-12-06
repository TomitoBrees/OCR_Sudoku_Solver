#pragma once

#include "sys/types.h"

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

static inline ssize_t dmax(double *a, size_t l) {
    if (l == 0)
        return -1;
    double max = a[0];
    ssize_t res = 0;
    for (size_t i = 1; i < l; i++)
        if (a[i] > max) {
            max = a[i];
            res = i;
        }
    return res;
}

static inline ssize_t max(float *a, size_t l) {
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

static inline ssize_t dmin(double *a, size_t l) {
    if (l == 0)
        return -1;
    double min = a[0];
    ssize_t res = 0;
    for (size_t i = 1; i < l; i++)
        if (a[i] < min) {
            min = a[i];
            res = i;
        }
    return res;
}

static inline ssize_t min(float *a, size_t l) {
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

#define ufmax(a, l) (size_t)max(a, l)
#define udmax(a, l) (size_t)dmax(am l)
