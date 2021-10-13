#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ve_offload.h>

int main(int argc, char** argv) {
    long begin = get_time();
    size_t size = 5;
    if (argc > 1)
        size = atoi(argv[1]);

    struct veo_proc_handle* proc = veo_proc_create(0);
    uint64_t handle = veo_load_library(proc, "./kernel.so");
    if (handle == 0) {
        perror("[VEO]:veo failed to load the shared library\n");
        return 0;
    }
    struct veo_thr_ctxt* ctx = veo_context_open(proc);

    long problem_bytes = size * sizeof(float);
    float* input1 = (float*)malloc(problem_bytes);
    float* input2 = (float*)malloc(problem_bytes);
    float* alpha = (float*)malloc(problem_bytes);
    float* beta = (float*)malloc(problem_bytes);
    for (size_t i = 0; i < size; i++) {
        input1[i] = (float)(rand()) / (float)(RAND_MAX);
        input2[i] = (float)(rand()) / (float)(RAND_MAX);
        alpha[i] = (float)(rand()) / (float)(RAND_MAX);
        beta[i] = (float)(rand()) / (float)(RAND_MAX);
    }
    float* output = (float*)malloc(problem_bytes);

    uint64_t input1_ptr;
    veo_alloc_mem(proc, &input1_ptr, problem_bytes);
    veo_write_mem(proc, input1_ptr, (void*)input1, problem_bytes);
    uint64_t input2_ptr;
    veo_alloc_mem(proc, &input2_ptr, problem_bytes);
    veo_write_mem(proc, input2_ptr, (void*)input2, problem_bytes);
    uint64_t alpha_ptr;
    veo_alloc_mem(proc, &alpha_ptr, problem_bytes);
    veo_write_mem(proc, alpha_ptr, (void*)alpha, problem_bytes);
    uint64_t beta_ptr;
    veo_alloc_mem(proc, &beta_ptr, problem_bytes);
    veo_write_mem(proc, beta_ptr, (void*)beta, problem_bytes);
    uint64_t output_ptr;
    veo_alloc_mem(proc, &output_ptr, problem_bytes);

    struct veo_args* argp = veo_args_alloc();
    veo_args_set_i64(argp, 0, input1_ptr);
    veo_args_set_i64(argp, 1, input2_ptr);
    veo_args_set_i64(argp, 2, alpha_ptr);
    veo_args_set_i64(argp, 3, beta_ptr);
    veo_args_set_i64(argp, 4, output_ptr);
    veo_args_set_i64(argp, 5, size);

    uint64_t kernel_error = veo_get_sym(proc, handle, "kernel_error");
    uint64_t id = veo_call_async(ctx, kernel_error, argp);
    uint64_t retval;
    veo_call_wait_result(ctx, id, &retval);
    veo_read_mem(proc, (void*)output, output_ptr, sizeof(float));

    veo_free_mem(proc, input1_ptr);
    veo_free_mem(proc, input2_ptr);
    veo_free_mem(proc, alpha_ptr);
    veo_free_mem(proc, beta_ptr);
    veo_free_mem(proc, output_ptr);
    veo_args_free(argp);
    free(input1);
    free(input2);
    free(alpha);
    free(beta);

    veo_context_close(ctx);
    veo_proc_destroy(proc);
    long finish = get_time();
    printf("runtime %ld ms\n", finish - begin);
    return 0;
}