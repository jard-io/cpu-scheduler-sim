#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 65536

#include "list.h"
#include "cpu.h"
#include "task.h"
// FCFS


Queue* queue;
int lines = 0;


void add(char* name, int priority, int burst) {
    if (!queue) {
        queue = createQueue();
    }
    int taskID;
    char T;
    sscanf(name, "%c%d", &T, &taskID);

    int task[] = {taskID, priority, burst, burst};
    push(queue, task);
    lines++;
}

void schedule() {

    // allocate memory for 2D array
    int **tasks = malloc(lines * sizeof(int *));
    Task* qPtr = queue->top;

    if (!qPtr) {
        perror("Error: scheduling queue is empty.");
        return;
    }

    for (int i = 0; i < lines; i++) {
        tasks[i] = malloc(4 * sizeof(int));
        tasks[i][0] = qPtr->taskID;
        tasks[i][1] = qPtr->priority;
        tasks[i][2] = qPtr->currentBurst;
        tasks[i][3] = qPtr->initialBurst;
        qPtr=qPtr->next;
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
}