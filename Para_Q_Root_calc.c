#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Global variables
double *partial_sums;           // Array to store each thread's partial sum
pthread_barrier_t barrier;      // Barrier for synchronizing reduction steps

// Structure to pass arguments to each thread
typedef struct {
    int thread_id;      // Thread identifier (0 to m-1)
    int total_threads;  // Total number of threads (m)
    int max_number;     // Maximum number to compute up to (n)
} ThreadArgs;

/**
 * Thread function: computes quadruple roots and performs parallel reduction
 *
 * Phase 1: Each thread computes sum of fourth roots for its assigned range
 * Phase 2: Threads perform tree-based parallel reduction to combine results
 */
void *compute_quadruple_roots(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int tid = args->thread_id;
    int m = args->total_threads;
    int n = args->max_number;

    // ========== PHASE 1: Compute partial sums ==========

    // Calculate the range of numbers this thread will process
    int start = tid * (n / m) + 1;
    int end = (tid + 1) * (n / m);

    // Handle the last thread: ensure it processes any remaining numbers
    if (tid == m - 1) {
        end = n;
    }

    // Compute the sum of quadruple roots for this thread's range
    double local_sum = 0.0;
    for (int i = start; i <= end; i++) {
        local_sum += pow(i, 0.25);  // Fourth root of i
    }

    // Store this thread's result in the shared array
    partial_sums[tid] = local_sum;

    printf("Thread %d (range %d-%d): partial sum = %.6f\n",
           tid, start, end, local_sum);

    // Wait for all threads to finish their initial computation
    pthread_barrier_wait(&barrier);

    // ========== PHASE 2: Parallel reduction ==========

    /*
     * Tree-based reduction: combines results in log(m) steps
     *
     * Example with 8 threads:
     * Step 1 (step=1): 0+=1, 2+=3, 4+=5, 6+=7  (4 active threads)
     * Step 2 (step=2): 0+=2, 4+=6              (2 active threads)
     * Step 3 (step=4): 0+=4                    (1 active thread)
     * Result: Thread 0 has the total sum
     */

    int step = 1;
    while (step < m) {
        // Only threads at positions that are multiples of (2 * step) participate
        if (tid % (2 * step) == 0) {
            // Check if there's a partner thread to add from
            if (tid + step < m) {
                partial_sums[tid] += partial_sums[tid + step];

                #ifdef DEBUG
                printf("Step %d: Thread %d += Thread %d (sum now = %.6f)\n",
                       step, tid, tid + step, partial_sums[tid]);
                #endif
            }
        }

        step *= 2;

        // Synchronize before next reduction step
        pthread_barrier_wait(&barrier);
    }

    // Thread 0 prints the final result
    if (tid == 0) {
        printf("\n========================================\n");
        printf("Total sum of fourth roots: %.6f\n", partial_sums[0]);
        printf("========================================\n");
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    // Validate command-line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_threads> <max_number>\n", argv[0]);
        fprintf(stderr, "  <num_threads>: Number of parallel threads to use\n");
        fprintf(stderr, "  <max_number>:  Compute sum of fourth roots from 1 to this number\n");
        return 1;
    }

    int m = atoi(argv[1]);  // Number of threads
    int n = atoi(argv[2]);  // Maximum number to process

    // Validate input parameters
    if (m <= 0 || n <= 0) {
        fprintf(stderr, "Error: Both arguments must be positive integers\n");
        return 1;
    }

    printf("Computing sum of fourth roots from 1 to %d using %d threads...\n", n, m);
    printf("Using parallel reduction algorithm (O(log n) complexity)\n\n");

    // Allocate memory for partial sums array
    partial_sums = (double *)malloc(m * sizeof(double));
    if (partial_sums == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    // Initialize barrier for m threads
    pthread_barrier_init(&barrier, NULL, m);

    // Allocate memory for threads and their arguments
    pthread_t *threads = malloc(m * sizeof(pthread_t));
    ThreadArgs *thread_args = malloc(m * sizeof(ThreadArgs));

    if (threads == NULL || thread_args == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(partial_sums);
        return 1;
    }

    // Create all worker threads
    for (int i = 0; i < m; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].total_threads = m;
        thread_args[i].max_number = n;

        if (pthread_create(&threads[i], NULL, compute_quadruple_roots, &thread_args[i]) != 0) {
            fprintf(stderr, "Error: Failed to create thread %d\n", i);
            free(partial_sums);
            free(threads);
            free(thread_args);
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up resources
    pthread_barrier_destroy(&barrier);
    free(partial_sums);
    free(threads);
    free(thread_args);

    return 0;
}
