#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define BUF_SIZE 65536
#define Q 10 // quant

// This function will check if all numbers in the array are 0
bool allZero(int **arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i][2] != 0) {
            return false;
        }
    }
    return true;
}

// Round - Robin -> each process is FCFS but they are given a specific amount of time to be processed (quant)
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
        tasks[i] = malloc(4 * sizeof(int)); // assume there are always 3 values

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
        tasks[i][3] = tasks[i][2]; // initial burst time
    }


    // CALCULATION (RR)

    // we only care about burst at this point
    /* 

    waiting time for Tn: Tn - 1's waiting time + Tn - 1's burst
    avg waiting time: total waiting time / N, where N is the number of tasks

    turnaround time for Tn: Tn-1 burst + Tn burst
    avg turnaround time: total turnaround time / N

    response time of Tn: time at when process is first given CPU - arrival time (in this case, 0)
    
    */ 

    float waitingTime = 0.0;
    float totalWaitingTime = 0.0;

    float turnaroundTime = 0.0;
    float totalTurnaround = 0.0;

    float responseTime = 0.0;
    float totalResponse = 0.0;

    float currentWaitingTime = 0.0;

    int contextSwitches = 0;
    int timeLine = 0;
    while (!allZero(tasks, lines)) {
        // loop through tasks
        for (int i = 0; i < lines; i++) {
            if (contextSwitches < lines)
                totalResponse += timeLine;
            if (tasks[i][2] == 0)
                continue; // skip
            // printf("T%d wait time = %d\n", tasks[i][0], timeLine);

            if (tasks[i][2] >= Q) {
                printf("Running task = [T%d] [%d] [%d] for 10 units.\n", tasks[i][0], tasks[i][1], tasks[i][2], tasks[i][2]);
                timeLine += Q;
                tasks[i][2] = tasks[i][2] - Q;
                
            }    
            else if (tasks[i][2] > 0) {
                printf("Running task = [T%d] [%d] [%d] for %d units.\n", tasks[i][0], tasks[i][1], tasks[i][2], tasks[i][2]);
                timeLine += tasks[i][2];
                tasks[i][2] -= tasks[i][2];
                
            }

            if (tasks[i][2] == 0) {
                // update waiting time
                totalWaitingTime += (timeLine - tasks[i][3]);
                totalTurnaround += timeLine;
            }            

            
            contextSwitches++;        
        }
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