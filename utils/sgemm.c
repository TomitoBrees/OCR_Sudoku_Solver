#include "sgemm.h"

// C := alpha * A * B + beta * C
void sgemm(const int M, const int N, const int K, const float alpha,
        const float *A, const float *B, const float beta, float *C)
{
    float value;
    for (int m = 0; m < M; m++)
        for (int n = 0; n < N; n++) {
            value = 0;
            for (int k = 0; k < K; k++) {
                value += 
                    A[m * K + k] *
                    B[k * N + n];
            }
            C[n + m * N] = alpha * value + beta * C[n + m * N];
        }
}

