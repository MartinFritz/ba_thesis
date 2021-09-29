#include <iostream>
#include <vector>
#include <execution>
#include <algorithm>
#include <chrono>
#include <sys/types.h>
#include <sys/sysctl.h>
using namespace std;


void swap(std::vector<int> &arr, int l, int r) {
    int temp = arr[l];
    arr[l] = arr[r];
    arr[r] = temp;

}

void quick_sort(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int pivot = arr[end];
        int left = start-1;
        int right = end;
        do  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            }
        } while (left < right);
        swap(arr, left, end);
        quick_sort(arr,start,left-1);
        quick_sort(arr,left+1,end);
    }
    
}

void quick_sort_startPivot(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int pivot = arr[start];
        int left = start-1;
        int right = end;
        do  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            }
        } while (left < right);
        swap(arr, left, end);
        quick_sort_startPivot(arr,start,left-1);
        quick_sort_startPivot(arr,left+1,end);
    }
    
}

void quick_sort_endPivot(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int pivot = arr[end];
        int left = start-1;
        int right = end;
        do  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            }
        } while (left < right);
        swap(arr, left, end);
        quick_sort_endPivot(arr,start,left-1);
        quick_sort_endPivot(arr,left+1,end);
    }
}

void quick_sort_randomPivot(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int rd = (rand()%(end-start+1)) + start;
        int pivot = arr[rd];
        int left = start-1;
        int right = end;
        do  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            }
        } while (left < right);
        swap(arr, left, end);
        quick_sort_randomPivot(arr,start,left-1);
        quick_sort_randomPivot(arr,left+1,end);
    }
    
}




void quicksort_start(std::vector<int> &arr, int start, int end){
    int left = start;
    int right = end;
    int pivot = arr[start];

    while (left <= right) {
        while (arr[left] < pivot)
                  left++;
            while (arr[right] > pivot)
                  right--;
            if (left <= right) {
                swap(arr,left,right);
                left++;
                right--;
            }
      }

      /* recursion */
      if (start < right)
        #pragma omp task shared(arr)
            quicksort_start(arr, start, right);
      if (left < end)
        #pragma omp task shared(arr)
            quicksort_start(arr, left, end);
}

void quicksort_end(std::vector<int> &arr, int start, int end){
    int left = start;
    int right = end;
    int pivot = arr[end];

    while (left <= right) {
        while (arr[left] < pivot)
                  left++;
            while (arr[right] > pivot)
                  right--;
            if (left <= right) {
                swap(arr,left,right);
                left++;
                right--;
            }
      }

      /* recursion */
      if (start < right)
        #pragma omp task shared(arr)
            quicksort_end(arr, start, right);
      if (left < end)
        #pragma omp task shared(arr)
            quicksort_end(arr, left, end);
}

void quicksort_middle(std::vector<int> &arr, int start, int end){
    int left = start;
    int right = end;
    int pivot = arr[(start+end)/2];

    while (left <= right) {
        while (arr[left] < pivot)
                  left++;
            while (arr[right] > pivot)
                  right--;
            if (left <= right) {
                swap(arr,left,right);
                left++;
                right--;
            }
      }

      if (start < right)
        #pragma omp task shared(arr)
            quicksort_middle(arr, start, right);
      if (left < end)
        #pragma omp task shared(arr)
            quicksort_middle(arr, left, end);
}
// int main() {

// //    std::vector<int> arr(1000000000);
// //    for (int i = 0; i < arr.size(); i++) {
// //        arr[i] = (rand() % 1000000);
// //
// //    }
// //    quick_sort(arr, 0, arr.size()-1);
//     cout << processIsTranslated() << std::endl;;
//     std::vector<int> input(1000000000);
//     for (int i = 0; i < input.size(); i++) {
//         input[i] = (rand() % 1000000); 
//     }
//     auto start = std::chrono::high_resolution_clock::now();
//     //std::sort(std::execution::par_unseq, input.begin(), input.end());
//     quick_sort(input, 0, input.size()-1);
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> elapsed = end - start;
//     cout << elapsed.count() << endl;
    
//     return 0;
// }