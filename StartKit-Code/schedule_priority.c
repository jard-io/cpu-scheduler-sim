#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 65536
// Priority - sort using a stable sorting algorithm, meaning if two are equal, then the first that comes up is put first
// merge sort is a stable sorting algo

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

int main(int argc, char* argv[]) {

    // open file
    FILE *file = fopen("sample-schedule.txt", "r");
    if (!file) {
        perror("Failed to open file.");
        return 1;
    }


    char buf[BUF_SIZE];
    int lines = 0;

    // counts lines efficiently (fgets is too slow for large text files)
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE, file);
        if (ferror(file))
            return -1;

        int i;
        for(i = 0; i < res; i++)
            if (buf[i] == '\n')
                lines++;

        if (feof(file)) {
            break;
        }
            
    }

    // printf("Lines: %d\n", lines);

    // allocate memory for 2D array

    int **tasks = malloc(lines * sizeof(int *));

    // dump for task name
    char taskID[3];

    if (!tasks) {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }

    fseek(file, 0, SEEK_SET);

    // read file
    for (int i = 0; i < lines; i++) {
        tasks[i] = malloc(3 * sizeof(int)); // assume there are always 3 values

        // error checking
        if (!tasks[i]) {
            perror("Error allocating memory.");
            fclose(file);
            for (int j = 0; j < i; j++) {
                free(tasks[j]);
            }
            free(tasks);
            return 1;
        }
        // store order in which task arrives
        tasks[i][0] = i;
        if (fscanf(file, "%99[^,], %d, %d\n", taskID, &tasks[i][1], &tasks[i][2]) != 3) {
            fprintf(stderr, " Error reading line %d\n", i + 1);
            fclose(file);
            for (int j = 0; j < i; j++) {
                free(tasks[j]);
            }
            free(tasks);
            return 1;
        }
    }

    mergeSort(tasks, 0, lines- 1);

    // Print the numbers to verify they were read correctly
    for (int i = 0; i < lines; i++) {
        printf("Running task = [T%d] [%d] [%d] for %d units.\n", tasks[i][0], tasks[i][1], tasks[i][2], tasks[i][2]);
    }

    // CALCULATION (FCFS)

    // we only care about burst at this point
    /* 

    waiting time for Tn: Tn - 1's waiting time + Tn - 1's burst
    avg waiting time: total waiting time / N, where N is the number of tasks

    turnaround time for Tn: Tn-1 burst + Tn burst
    avg turnaround time: total turnaround time / N

    response time of Tn: sum of waiting time of previous T's - arrival time of Tn
    avg response time: total response time / N
    
    */ 


    float waitingTime = 0.0;
    float totalWaitingTime = 0.0;

    float turnaroundTime = 0.0;
    float totalTurnaround = 0.0;

    float responseTime = 0.0;
    float totalResponse = 0.0;

    float currentWaitingTime = 0.0;
    // loop through tasks

    for (int i = 0; i < lines; i++) {
        if (i != 0)
            waitingTime += tasks[i - 1][2];

        // turnaround
        turnaroundTime += tasks[i][2];
        totalTurnaround += turnaroundTime;

        // response time
        responseTime = waitingTime - 0; // arrival time of Tn
        totalResponse += responseTime;
        // waiting time
        totalWaitingTime+=waitingTime;
    }


    float avgWaitingTime = totalWaitingTime / lines;
    float avgTurnaroundTime = totalTurnaround / lines;
    float avgResponseTime = totalResponse / lines;

    printf("\nAverage waiting time = %.2f\n", avgWaitingTime);
    printf("Average turnaround time = %.2f\n", avgTurnaroundTime);
    printf("Average response time = %.2f\n", avgResponseTime);




    // Free the allocated memory
    for (int i = 0; i < lines; i++) {
        free(tasks[i]);
    }
    free(tasks);

    fclose(file);
    return 0;
}