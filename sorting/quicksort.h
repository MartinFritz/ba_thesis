#include <iostream>
#include <vector>
#include <execution>
#include <algorithm>
#include <chrono>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <omp.h>
using namespace std;


void swap(std::vector<int> &arr, int l, int r) {
    int temp = arr[l];
    arr[l] = arr[r];
    arr[r] = temp;

}






/* quicksort adapted from https://www.algolist.net/Algorithms/Sorting/Quicksort */




void quicksort_start(std::vector<int> &arr, int start, int end){
    int left = start-1;
    int right = end+1;
    int pivot = arr[start];

    while (left <= right) {
        do left++; while (arr[left] < pivot);
        do right--; while (arr[right] > pivot);
        if (left <= right) {
            swap(arr,left,right);
        }
      }

      if (start < right)
        #pragma omp task shared(arr)
            quicksort_start(arr, start, right);
      if (left < end)
        #pragma omp task shared(arr)
            quicksort_start(arr, left, end);
}

void quicksort_end(std::vector<int> &arr, int start, int end){
    int left = start-1;
    int right = end+1;
    int pivot = arr[end];

    while (left <= right) {
        do left++; while (arr[left] < pivot);
        do right--; while (arr[right] > pivot);
        if (left <= right) {
            swap(arr,left,right);
        }
      }

      if (start < right)
        #pragma omp task shared(arr)
            quicksort_end(arr, start, right);
      if (left < end)
        #pragma omp task shared(arr)
            quicksort_end(arr, left, end);
}

void quicksort_middle(std::vector<int> &arr, int start, int end){
    int left = start-1;
    int right = end+1;
    int pivot = arr[(start+end)/2];

    while (left <= right) {
        do left++; while (arr[left] < pivot);
        do right--; while (arr[right] > pivot);
        if (left <= right) {
            swap(arr,left,right);
        }
      }

      if (start < right)
        #pragma omp task shared(arr)
            quicksort_middle(arr, start, right);
      if (left < end)
        #pragma omp task shared(arr)
            quicksort_middle(arr, left, end);
}
