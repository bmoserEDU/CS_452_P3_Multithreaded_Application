#include <stdio.h>
#include <stdlib.h>
#include "lab.h"


#ifndef TEST
int main(int argc, char **argv)
{

  if (argc < 3)
    {
      printf("usage: %s <array_size> <num_threads>", argv[0]);
      return 1;
    }
  // parsing command line args
  int temp_size = atoi(argv[1]);
  if (temp_size <= 0) {
      fprintf(stderr, "Array size must be positive\n");
      return 1;
  }
  size_t listSize = (size_t)temp_size;
  
  int temp_threads = atoi(argv[2]);
  if (temp_threads <= 0) {
      fprintf(stderr, "Num threads must be positive\n");
      return 1;
  }
  size_t num_threads = (size_t)temp_threads;
  int *A = (int *)malloc(sizeof(int) * listSize);
  if (A == NULL)
    {
      fprintf(stderr, "Memory allocation failed\n");
      return 1;
    }
  for (size_t i = 0; i < listSize; i++)
    {
      A[i] = rand() % 1000; // Fill with random numbers
    }
  double start = getMilliSeconds();
  mergesort_mt(A, listSize, num_threads);
  double end = getMilliSeconds();
  free(A);

  printf("%zu %f\n", num_threads, end - start);

  return 0;
}
#endif