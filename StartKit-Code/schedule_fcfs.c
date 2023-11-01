#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 65536
// FCFS
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