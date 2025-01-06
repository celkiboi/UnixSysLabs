#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define N 768
#define ARRAY_SIZE N*N
#define THREAD_COUNT 12

typedef struct ThreadParam 
{
    uint64_t beginRow;
    uint64_t endRow;
    float* matrix;
} THREAD_PARAM;

void DoMatrixWriteSingleThreaded(float* matrix) 
{
    for (uint64_t i = 0; i < N; i++) 
    {
        for (uint64_t j = 0; j < N; j++) 
        {
            float value = 0;
            for (uint64_t k = 0; k < i; k++)
                value += k * sinf(j) - j * cosf(k);

            matrix[i * N + j] = value;
        }
    }
}

void* DoMatrixWriteMultiThreaded(void* pThParam) 
{
    THREAD_PARAM* thread_param = (THREAD_PARAM*)pThParam;
    uint64_t beginRow = thread_param->beginRow;
    uint64_t endRow = thread_param->endRow;
    float* matrix = thread_param->matrix;

    for (uint64_t i = beginRow; i < endRow; i++) 
    {
        for (uint64_t j = 0; j < N; j++) 
        {
            float value = 0;
            for (uint64_t k = 0; k < i; k++)
                value += k * sinf(j) - j * cosf(k);

            matrix[i * N + j] = value;
        }
    }

    return NULL;
}

int CheckIfMatrixIsWrittenProperly(float* matrix) 
{
    for (uint64_t i = 0; i < N; i++) 
    {
        for (uint64_t j = 0; j < N; j++) 
        {
            float value = 0;
            for (uint64_t k = 0; k < i; k++)
                value += k * sinf(j) - j * cosf(k);

            if (matrix[i * N + j] != value)
                return 0;
        }
    }
    return 1;
}

double getElapsedTime(struct timespec start, struct timespec end)
{
    if (end.tv_nsec < start.tv_nsec) {
        end.tv_sec -= 1;
        end.tv_nsec += 1000000000;
    }

    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;
}


int main(void) 
{
    struct timespec start, end;

    float* matrix = (float*)malloc(ARRAY_SIZE * sizeof(float));
    if (matrix == NULL)
        exit(EXIT_FAILURE);

    clock_gettime(CLOCK_MONOTONIC, &start);
    DoMatrixWriteSingleThreaded(matrix);
    clock_gettime(CLOCK_MONOTONIC, &end);

    puts("SINGLE THREAD RESULTS: ");
    printf("Time to write: %.2f ms\n", getElapsedTime(start, end));

    if (CheckIfMatrixIsWrittenProperly(matrix))
        puts("Matrix contains right values");
    else
        puts("Matrix contains wrong values");

    free(matrix);

    matrix = (float*)malloc(ARRAY_SIZE * sizeof(float));
    if (matrix == NULL)
        exit(EXIT_FAILURE);

    THREAD_PARAM* thread_params = (THREAD_PARAM*)calloc(THREAD_COUNT, sizeof(THREAD_PARAM));
    if (thread_params == NULL)
        exit(EXIT_FAILURE);

    pthread_t threads[THREAD_COUNT];

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (uint8_t i = 0; i < THREAD_COUNT; i++) 
    {
        thread_params[i].beginRow = i * (N / THREAD_COUNT);
        thread_params[i].endRow = thread_params[i].beginRow + (N / THREAD_COUNT);
        thread_params[i].matrix = matrix;

        if (i == (THREAD_COUNT - 1))
            thread_params[i].endRow += N % THREAD_COUNT;

        if (pthread_create(&threads[i], NULL, DoMatrixWriteMultiThreaded, &thread_params[i]) != 0)
            exit(EXIT_FAILURE);
    }

    for (uint8_t i = 0; i < THREAD_COUNT; i++) 
        pthread_join(threads[i], NULL);


    clock_gettime(CLOCK_MONOTONIC, &end);

    puts("MULTI THREAD RESULTS: ");
    printf("Time to write: %.2f ms\n", getElapsedTime(start, end));

    if (CheckIfMatrixIsWrittenProperly(matrix))
        puts("Matrix contains right values");
    else
        puts("Matrix contains wrong values");

    free(thread_params);
    free(matrix);
    return 0;
}
