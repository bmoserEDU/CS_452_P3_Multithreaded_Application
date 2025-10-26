#include <stdlib.h>
#include <stdio.h>
#include "harness/unity.h"
#include "../src/lab.h"
#include <string.h>

// Helper function to verify array is sorted
static int is_sorted(int arr[], int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[i-1]) return 0;
    }
    return 1;
}

void setUp(void) {
    printf("Setting up test...\n");
}

void tearDown(void) {
    printf("Tearing down test...\n");
}

// Test basic array sorting
void test_basic_sort(void) {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int size = sizeof(arr) / sizeof(arr[0]);
    mergesort_mt(arr, size, 2);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arr, size), "Array should be sorted in ascending order");
    TEST_ASSERT_EQUAL_INT(11, arr[0]);
    TEST_ASSERT_EQUAL_INT(90, arr[size-1]);
}

// Test already sorted array
void test_sorted_array(void) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int size = sizeof(arr) / sizeof(arr[0]);
    mergesort_mt(arr, size, 2);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arr, size), "Array should remain sorted");
    TEST_ASSERT_EQUAL_INT(1, arr[0]);
    TEST_ASSERT_EQUAL_INT(8, arr[size-1]);
}

// Test reverse sorted array
void test_reverse_sorted_array(void) {
    int arr[] = {8, 7, 6, 5, 4, 3, 2, 1};
    int size = sizeof(arr) / sizeof(arr[0]);
    mergesort_mt(arr, size, 2);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arr, size), "Reverse sorted array should be sorted ascending");
    TEST_ASSERT_EQUAL_INT(1, arr[0]);
    TEST_ASSERT_EQUAL_INT(8, arr[size-1]);
}

// Test array with duplicates
void test_array_with_duplicates(void) {
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    int size = sizeof(arr) / sizeof(arr[0]);
    mergesort_mt(arr, size, 3);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arr, size), "Array with duplicates should be sorted");
}

// Test single element array
void test_single_element(void) {
    int arr[] = {42};
    int size = 1;
    mergesort_mt(arr, size, 2);
    TEST_ASSERT_EQUAL_INT(42, arr[0]);
}

//different thread counts
void test_different_thread_counts(void) {
    int arr[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    int size = sizeof(arr) / sizeof(arr[0]);
    
    // 1 thread
    int arr1[size];
    memcpy(arr1, arr, sizeof(arr));
    mergesort_mt(arr1, size, 1);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arr1, size), "Array should be sorted with 1 thread");
    
    // 4 threads
    int arr4[size];
    memcpy(arr4, arr, sizeof(arr));
    mergesort_mt(arr4, size, 4);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arr4, size), "Array should be sorted with 4 threads");
    
    // max threads
    int arrMax[size];
    memcpy(arrMax, arr, sizeof(arr));
    mergesort_mt(arrMax, size, MAX_THREADS);
    TEST_ASSERT_TRUE_MESSAGE(is_sorted(arrMax, size), "Array should be sorted with MAX_THREADS");
    
    for (int i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_INT(arr1[i], arr4[i]);
        TEST_ASSERT_EQUAL_INT(arr1[i], arrMax[i]);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_basic_sort);
    RUN_TEST(test_sorted_array);
    RUN_TEST(test_reverse_sorted_array);
    RUN_TEST(test_array_with_duplicates);
    RUN_TEST(test_single_element);
    RUN_TEST(test_different_thread_counts);
    return UNITY_END();
}
