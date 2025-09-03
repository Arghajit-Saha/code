#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int *t = *a;
    *a = *b;
    *b = *t;
}

int conquer(int *arr, int low, int high) {
    int pivot = arr[high];

    int i = low;
    for(int j = low; j < high; j++) {
        if(arr[j] < pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void divide(int *arr, int low, int high) {
    if(low < high) {
        int mid = conquer(arr, low, high);
        divide(arr, low, mid - 1);
        divide(arr, low + 1, high);
    }
}


