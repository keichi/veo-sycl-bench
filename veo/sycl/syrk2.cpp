#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include <CL/sycl.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sys/time.h>

REGISTER_KERNEL(syrk2);
using namespace cl::sycl;

void init_arrays(DATA_TYPE* A, DATA_TYPE* B, DATA_TYPE* C, size_t size) {
    const size_t N = size;
    const size_t M = size;

    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            C[i * N + j] = ((DATA_TYPE)i * j + 2) / N;
        }

        for (size_t j = 0; j < M; j++) {
            A[i * N + j] = ((DATA_TYPE)i * j) / N;
            B[i * N + j] = ((DATA_TYPE)i * j + 1) / N;
        }
    }
}
int main(int argc, char** argv) {
    long begin = get_time();
    size_t size = 5;
    if (argc > 1)
        size = atoi(argv[1]);

    accelerator_selector as;
    queue q(as);

    long problem_bytes = size * size * sizeof(DATA_TYPE);
    DATA_TYPE* A = (DATA_TYPE*)malloc(problem_bytes);
    DATA_TYPE* B = (DATA_TYPE*)malloc(problem_bytes);
    DATA_TYPE* C = (DATA_TYPE*)malloc(problem_bytes);
    init_arrays(A, B, C, size);

    buffer<DATA_TYPE> A_buff(A, range<1>(size * size));
    buffer<DATA_TYPE> B_buff(B, range<1>(size * size));
    buffer<DATA_TYPE> C_buff(C, range<1>(size * size));

    q.submit([&](handler& cgh) {
        auto A_access = A_buff.get_access<access::mode::read>(cgh);
        auto B_access = B_buff.get_access<access::mode::read>(cgh);
        auto C_access = C_buff.get_access<access::mode::write>(cgh);
        cgh.parallel_for<class syrk2>(range<1>(size), [=](id<1> i) {
            const size_t N = size;
            const size_t M = size;
            DATA_TYPE alpha = 1;
            DATA_TYPE beta = 1;
            for (size_t i = 0; i < N; i++) {
                for (size_t j = 0; j < N; j++) {
                    C_access[i * N + j] *= beta;
                }
            }
            for (size_t i = 0; i < N; i++) {
                for (size_t j = 0; j < N; j++) {
                    for (size_t k = 0; k < M; k++) {
                        C_access[i * N + j] += alpha * A_access[i * M + k] * B_access[j * M + k];
                        C_access[i * N + j] += alpha * B_access[i * M + k] * A_access[j * M + k];
                    }
                }
            }
        });
    });
    q.wait();

    long finish = get_time();
    printf("runtime %ld ms\n", finish - begin);
    return 0;
}