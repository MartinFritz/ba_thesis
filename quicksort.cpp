#include <iostream>
#include <vector>
#include <execution>
#include <algorithm>
#include <chrono>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <random>
#include <omp.h>
using namespace std;
#define TASK_SIZE 100

void swap(std::vector<int> &arr, int l, int r) {
    int temp = arr[l];
    arr[l] = arr[r];
    arr[r] = temp;

}

void quick_sort_startPivot_new(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int p;
        int pivot = arr[start];
        int left = start-1;
        int right = end+1;
        while (true)  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            } else {
                p = right;
                break;
            }
        } 


        #pragma omp task shared(arr)
            quick_sort_startPivot_new(arr,start,p);
        #pragma omp task shared(arr)
            quick_sort_startPivot_new(arr,p+1,end);
        
    }
    
}

void quick_sort_endPivot_new(std::vector<int> &arr, int start, int end) {


    if (start < end) {
        int p;

        int pivot = arr[end];
        int left = start-1;
        int right = end;
        while (true)  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            } else {
                p = right;
                break;
            }
        } 

        
        #pragma omp task shared(arr)
            quick_sort_endPivot_new(arr,start,p);
        #pragma omp task shared(arr)
            quick_sort_endPivot_new(arr,p+1,end);
        
    }
}

void quick_sort_endPivot(std::vector<int> &arr, int start, int end) {


        int pivot = arr[end];
        int left = start;
        int right = end;
        while (left <= right)  {
            while(arr[left] < pivot) {left++;}
            while(arr[right] > pivot) {right--;}
            if (left <= right) {
                swap(arr,left,right);
                left++;
                right--;
            }
        }
    if (start < right)
        quick_sort_endPivot(arr,start,right);
    if (left < end)
        quick_sort_endPivot(arr,left,end);

    return;
    
}

void quick_sort_startPivot(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        swap(arr,start,end);
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
        #pragma omp task shared(arr)
        quick_sort_endPivot(arr,start,left-1);
        #pragma omp task shared(arr)
        quick_sort_endPivot(arr,left+1,end);
    }
    
}

void quick_sort_randomPivot(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int middle = ((end-start)/2) + start;
        swap(arr,start,end);
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
        #pragma omp task shared(arr)
        quick_sort_endPivot(arr,start,left-1);
        #pragma omp task shared(arr)
        quick_sort_endPivot(arr,left+1,end);
    }
    
}

void quick_sort_randomPivot_new(std::vector<int> &arr, int start, int end) {
    
    if (start < end) {
        int p;
        int rd = (rand()%(end-start+1)) + start;
        int pivot = arr[rd];
        int left = start-1;
        int right = end+1;
        while (true)  {
            do {
                left++;
            } while (arr[left] < pivot);
            do {
                right--;
            } while (arr[right] > pivot && right >= start);
            if (left < right) {
                swap(arr,left,right);
            } else {
                p = right;
                break;
            }
        } 


        #pragma omp task shared(arr)
            quick_sort_randomPivot_new(arr,start,p);
        #pragma omp task shared(arr)
            quick_sort_randomPivot_new(arr,p+1,end);
        
    }
    
}




int main() {

//    std::vector<int> arr(1000000000);
//    for (int i = 0; i < arr.size(); i++) {
//        arr[i] = (rand() % 1000000);
//
//    }
//    quick_sort(arr, 0, arr.size()-1);

    std::vector<int> input(100000000);
    for (int i = 0; i < input.size(); i++) {
        input[i] = (rand() % 100000000); 
    }
    omp_set_dynamic(0);              /** Explicitly disable dynamic teams **/
    omp_set_num_threads(8);
    auto start = std::chrono::high_resolution_clock::now();

    //std::sort(std::execution::par_unseq, input.begin(), input.end());

    #pragma omp parallel
        {
            #pragma omp single
            quick_sort_endPivot(input, 0, input.size()-1);
        }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    // for (int i = 0; i < input.size(); i++) {
    //     std::cout << input[i] << std::endl;
    // }
    cout << elapsed.count() << endl;
    
    return 0;
}
