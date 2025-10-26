
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> /* for gettimeofday system call */
#include <string.h>
#include <pthread.h>
#include "lab.h"

// Global mutex for synchronizing merge operations
static pthread_mutex_t merge_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t sort_barrier;

/**
 * @brief Standard insertion sort that is faster than merge sort for small array's
 *
 * @param A The array to sort
 * @param p The starting index
 * @param r The ending index
 */
static void insertion_sort(int A[], int p, int r)
{
  int j;

  for (j = p + 1; j <= r; j++)
    {
      int key = A[j];
      int i = j - 1;
      while ((i > p - 1) && (A[i] > key))
        {
	  A[i + 1] = A[i];
	  i--;
        }
      A[i + 1] = key;
    }
}

/**
 * @brief Sorts an array of ints into ascending order using the constant
 * INSERTION_SORT_THRESHOLD internally
 *
 * @param A A pointer to the start of the array
 * @param p The starting index
 * @param r The ending index
 */
void mergesort_s(int A[], int p, int r)
{
  if (r - p + 1 <=  INSERTION_SORT_THRESHOLD)
    {
      insertion_sort(A, p, r);
    }
  else
    {
      int q = (p + r) / 2;
      mergesort_s(A, p, q);
      mergesort_s(A, q + 1, r);
      merge_s(A, p, q, r);
    }

}

/**
 * @brief Merge two sorted sequences A[p..q] and A[q+1..r] and place merged
 *              output back in array A. Uses extra space proportional to
 *              A[p..r].
 *
 * @param A The array to merge into
 * @param p The starting index of the first half
 * @param q The middle
 * @param r The ending index of the second half
 */
void merge_s(int A[], int p, int q, int r)
{
  // converting to size_t to fix warnings
  size_t size = (size_t)(r - p + 1); 
  int *B = (int *)malloc(sizeof(int) * size);

  int i = p;
  int j = q + 1;
  int k = 0;
  int l;

  /* as long as both lists have unexamined elements */
  /*  this loop keeps executing. */
  while ((i <= q) && (j <= r))
    {
      if (A[i] < A[j])
        {
	  B[k] = A[i];
	  i++;
        }
      else
        {
	  B[k] = A[j];
	  j++;
        }
      k++;
    }

  /* now only at most one list has unprocessed elements. */
  if (i <= q)
    {
      /* copy remaining elements from the first list */
      for (l = i; l <= q; l++)
        {
	  B[k] = A[l];
	  k++;
        }
    }
  else
    {
      /* copy remaining elements from the second list */
      for (l = j; l <= r; l++)
        {
	  B[k] = A[l];
	  k++;
        }
    }

  /* copy merged output from array B back to array A */
  k = 0;
  for (l = p; l <= r; l++)
    {
      A[l] = B[k];
      k++;
    }

  free(B);
}

/**
 * @brief The function that is called by each thread to sort their chunk
 *
 * @param args see struct parallel_args
 * @return void* always NULL
 * AI Use: Assisted by Claude
 */
void *parallel_mergesort(void *args)
{
    struct parallel_args *pargs = (struct parallel_args *)args;
    size_t start = pargs->start;
    size_t end = pargs->end;
    
    if (start < end) {
        mergesort_s(pargs->A, (int)start, (int)end);
    }
    
    //Using barrier to sync threads after each sort, threads wait here until all have sorted their array portion before proceeding
    pthread_barrier_wait(&sort_barrier);
    
    return NULL;
}

/**
 * @brief Sorts an array of ints into ascending order using multiple
 * threads
 *
 * @param A A pointer to the start of the array
 * @param n The size of the array
 * @param num_threads The number of threads to use.
 * AI Use: Assisted by Claude
 */
void mergesort_mt(int *A, size_t n, unsigned int num_threads)
{
    int barrier_initialized = 0;
    int ret = 0;

    // Handling egde case where array size < num threads
    if (n < num_threads) {
        // If array is smaller than thread count, use only as many threads as elements
        num_threads = (unsigned int)n;
    }
    
    if (num_threads > MAX_THREADS) {
        // forcing to max threads if necessary
        num_threads = MAX_THREADS;
    }
    
    if (num_threads <= 1 || n <= INSERTION_SORT_THRESHOLD) {
        mergesort_s(A, 0, (int)(n - 1));
        return;
    }
    
    // Initializing a barrier for my threads 
    ret = pthread_barrier_init(&sort_barrier, NULL, num_threads);
    if (ret != 0) {
        fprintf(stderr, "Failed to initialize barrier: %s\n", strerror(ret));
        return;
    }
    barrier_initialized = 1;

    // Create thread arguments array
    struct parallel_args *thread_args = malloc(sizeof(struct parallel_args) * num_threads);
    if (thread_args == NULL) {
        fprintf(stderr, "Memory allocation failed for thread arguments\n");
        return;
    }

    // Calculate chunk size for each thread
    // Using second approach style from class with added remainder handling
    size_t chunk_size = n / num_threads;
    size_t remainder = n % num_threads;
    size_t current_pos = 0;

    // Create threads and assign work
    size_t threads_created = 0;
    for (size_t i = 0; i < num_threads; i++) {
        thread_args[i].A = A;
        thread_args[i].start = current_pos;
        thread_args[i].end = current_pos + chunk_size - 1;
        
        // Add remainder elements to last thread
        if (i == num_threads - 1) {
            thread_args[i].end += remainder;
        }
        
        ret = pthread_create(&thread_args[i].tid, NULL, parallel_mergesort, &thread_args[i]);
        if (ret != 0) {
            fprintf(stderr, "Failed to create thread %zu: %s\n", i, strerror(ret));
            // Clean up already created threads
            for (size_t j = 0; j < threads_created; j++) {
                pthread_join(thread_args[j].tid, NULL);
            }
            if (barrier_initialized) {
                pthread_barrier_destroy(&sort_barrier);
            }
            free(thread_args);
            return;
        }
        threads_created++;
        current_pos += chunk_size;
    }

    for (unsigned int i = 0; i < num_threads; i++) {
        ret = pthread_join(thread_args[i].tid, NULL);
        if (ret != 0) {
            fprintf(stderr, "Failed to join thread %d: %s\n", i, strerror(ret));
        }
    }
    
    // locking for main thread safety
    pthread_mutex_lock(&merge_mutex);

        // Merge sorted chunks
        // This is a very critical section!
    for (size_t size = chunk_size; size < n; size = size * 2) {
        for (size_t i = 0; i < n - size; i += size * 2) {
            int mid = (int)(i + size - 1);
            int right_end = (int)((i + size * 2 - 1) < (n - 1) ? (i + size * 2 - 1) : (n - 1));
            merge_s(A, (int)i, mid, right_end);
        }
    }
    pthread_mutex_unlock(&merge_mutex);

    // Check for barrier
    if (barrier_initialized) {
        ret = pthread_barrier_destroy(&sort_barrier);
        if (ret != 0) {
            fprintf(stderr, "Failed to destroy barrier: %s\n", strerror(ret));
        }
    }
    free(thread_args);
}

/**
 * @brief retuns the current time as milliseconds
 * @return the number of milliseconds
 */
double getMilliSeconds() {
    struct timeval now;
    gettimeofday(&now, (struct timezone *)0);
    return (double)now.tv_sec * 1000.0 + (double)now.tv_usec / 1000.0;
}
