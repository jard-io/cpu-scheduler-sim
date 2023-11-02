#include <stdio.h>
#include <stdlib.h>

#include "sort.h"

void merge(int **arr, int left, int middle, int right) {
    int i, j, k;
    int n1 = middle  - left + 1;
    int  n2 = right - middle;

    // create temp arrays
    int **L = (int **)malloc(n1 * sizeof(int *));
    int **R = (int **)malloc(n2 * sizeof(int *));


    for (i = 0; i < n1; i++)
        L[i] = (int *)malloc(3 * sizeof(int));
    for (i = 0; i < n2; i++)
        R[i] = (int *)malloc(3 * sizeof(int));

    // copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        for (j = 0; j < 3; j++)
            L[i][j] = arr[left + i][j];
    for (i = 0; i < n2; i++)
        for (j = 0; j < 3; j++)
            R[i][j] = arr[middle + 1 + i][j];

            
    // merge the temp arrays back into arr[l..r]
    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (L[i][1] >= R[j][1]) {
            for (int z = 0; z < 3; z++)
                arr[k][z] = L[i][z];
            i++;
        } else {
            for (int z = 0; z < 3; z++)
                arr[k][z] = R[j][z];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        for (int z = 0; z < 3; z++)
            arr[k][z] = L[i][z];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        for (int z = 0; z < 3; z++)
            arr[k][z] = R[j][z];
        j++;
        k++;
    }
    // Free the memory allocated for the temp arrays
    for (i = 0; i < n1; i++)
        free(L[i]);
    free(L);
    for (i = 0; i < n2; i++)
        free(R[i]);
    free(R);
}

void mergeSort(int **arr, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;

        // Sort first and second halves
        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);

        // Merge the sorted halves
        merge(arr, left, middle, right);
    }
}

