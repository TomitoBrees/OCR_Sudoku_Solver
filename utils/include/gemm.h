#pragma once

#ifdef BLAS_EXIST

#include "openblas/cblas.h"

#else

void sgemm(const int M, const int N, const int K, const float alpha,
        const float *A, const float *B,
        const float beta, float *C);

void dgemm(const int M, const int N, const int K, const double alpha,
        const double *A, const double *B, const double beta, double *C);
#endif

/* M: number of rows of op(A)
 * N: number of columns of op(B)
 * K: number of columns of op(A) and the number of rows of op(B)
 */
static inline void gemm(const int M, const int N, const int K, const float *A,
        const float *B, float *C) {
#ifdef BLAS_EXIST
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1,
            A, K,
            B, N,
            1, C, N);
#else
    sgemm(M, N, K, 1, A, B, 1, C);
#endif
}

static inline void gemm_n(const int M, const int N, const int K, const float *A,
        const float *B, float *C) {
#ifdef BLAS_EXIST
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1,
            A, K,
            B, N,
            0, C, N);
#else
    sgemm(M, N, K, 1, A, B, 0, C);
#endif
}

static inline void dgemm_n(const int M, const int N, const int K, const double *A,
        const double *B, double *C) {
#ifdef BLAS_EXIST
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1,
            A, K,
            B, N,
            0, C, N);
#else
    dgemm(M, N, K, 1, A, B, 0, C);
#endif
}

