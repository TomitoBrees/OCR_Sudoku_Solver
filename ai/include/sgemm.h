#pragma once

#include "defs.h"

#ifdef BLAS_EXIST

#include "openblas/cblas.h"

#else

void sgemm(const int M, const int N, const int K, const float alpha,
        const float *A, const float *B,
        const float beta, float *C);

#endif

/* M: number of rows of op(A)
 * N: number of columns of op(B)
 * K: number of columns of op(A) and the number of rows of op(B)
 */
static inline void gemm(const int M, const int N, const int K, const NETWORK_NUM *A,
        const NETWORK_NUM *B, NETWORK_NUM *C) {
#ifdef BLAS_EXIST
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1,
            A, K,
            B, N,
            1, C, N);
#else
    sgemm(M, N, K, 1, A, B, 1, C);
#endif
}

static inline void gemm_n(const int M, const int N, const int K, const NETWORK_NUM *A,
        const NETWORK_NUM *B, NETWORK_NUM *C) {
#ifdef BLAS_EXIST
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, 1,
            A, K,
            B, N,
            0, C, N);
#else
    sgemm(M, N, K, 1, A, B, 0, C);
#endif
}

