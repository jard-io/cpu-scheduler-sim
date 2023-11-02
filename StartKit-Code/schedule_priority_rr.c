#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"
#include "task.h"
#include "sort.h"
#include "schedulers.h"
#include "cpu.h"

#define BUF_SIZE 65536
#define Q 10 // quant


Queue* processQueue;
int lines = 0;


void add(char* name, int priority, int burst) {
    if (!processQueue) {
        processQueue = createQueue();
    }
    int taskID;
    char T;
    sscanf(name, "%c%d", &T, &taskID);

    int task[] = {taskID, priority, burst, burst};
    push(processQueue, task);
    lines++;
}

// Round - Robin -> each process is FCFS but they are given a specific amount of time to be processed (quant)
void schedule() {

    // allocate memory for 2D array
    int **tasks = malloc(lines * sizeof(int *));
    Task* ptr = processQueue->top;

    if (!ptr) {
        perror("Error: scheduling queue is empty.");
        return;
    }

    for (int i = 0; i < lines; i++) {
        tasks[i] = malloc(4 * sizeof(int));
        tasks[i][0] = ptr->taskID;
        tasks[i][1] = ptr->priority;
        tasks[i][2] = ptr->currentBurst;
        tasks[i][3] = ptr->initialBurst;
        ptr=ptr->next;
    }

    mergeSort(tasks, 0, lines- 1);
    // // Print the numbers to verify they were read correctly
    // for (int i = 0; i < lines; i++) {
    //     printf("Running task = [T%d] [%d] [%d] for %d units.\n", tasks[i][0], tasks[i][1], tasks[i][2], tasks[i][2]);
    // }        
    // CALCULATION (RR)

    // we only care about burst at this point
    /* 

    waiting time for Tn: Tn - 1's waiting time + Tn - 1's burst
    avg waiting time: total waiting time / N, where N is the number of tasks

    turnaround time for Tn: Tn-1 burst + Tn burst
    avg turnaround time: total turnaround time / N

    response time of Tn: time at when process is first given CPU - arrival time (in this case, 0)
    
    */ 
    // printf("\n\n");
    // for (int i = 0; i < lines; i++) {
    //     printf("%d ", tasks[i][2]);
    // }
    float waitingTime = 0.0;
    float totalWaitingTime = 0.0;

    float turnaroundTime = 0.0;
    float totalTurnaround = 0.0;

    float responseTime = 0.0;
    float totalResponse = 0.0;

    float currentWaitingTime = 0.0;

    int contextSwitches = 0;
    int timeLine = 0;

    int priorityPtr = 0;

    Queue* queue = createQueue();


    while (priorityPtr < lines) {
        
        // loop through tasks
        int priority = tasks[priorityPtr][1];
        // fill up queue
        while (priorityPtr < lines && priority == tasks[priorityPtr][1]) {
            push(queue, tasks[priorityPtr]);
            // printf("Pushed T%d\n", tasks[priorityPtr][0]);
            priorityPtr++;
        }

        // printQueue(queue);

        Task* qPtr = queue->top;

        int firstResponsePtr = 0;
        int initialQueueSize = queue->size;
        printQueue(queue);

        // start giving CPU
        while (queue->size > 0) {
            if (!qPtr)
                qPtr = queue->top;

            if (firstResponsePtr < initialQueueSize) {
                // printf("\n\nTimeline: %d | T%d \n\n", timeLine, qPtr->taskID);
                totalResponse += timeLine;
                firstResponsePtr++;
            }

            if (qPtr->currentBurst >= Q) {
                if (queue->size == 1) {
                    run(qPtr, qPtr->currentBurst);
                    // printf("Running task = [T%d] [%d] [%d] for %d units.\n", qPtr->taskID, qPtr->priority, qPtr->currentBurst, qPtr->currentBurst);
                    timeLine += qPtr->currentBurst;
                    qPtr->currentBurst -= qPtr->currentBurst;
                    // tasks[qPtr->taskID][2] -= qPtr->currentBurst;
                }
                else {
                    run(qPtr, Q);
                    // printf("Running task = [T%d] [%d] [%d] for 10 units.\n", qPtr->taskID, qPtr->priority, qPtr->currentBurst);
                    timeLine += Q;
                    qPtr->currentBurst -= Q;
                    // tasks[qPtr->taskID][2] -= Q;

                    // printf("Ok!\n");
                }
            }    
            else{
                // printf("Running task = [T%d] [%d] [%d] for %d units.\n", qPtr->taskID, qPtr->priority, qPtr->currentBurst, qPtr->currentBurst);
                run(qPtr, qPtr->currentBurst);
                timeLine += qPtr->currentBurst;  
                qPtr->currentBurst -= qPtr->currentBurst;
                // tasks[qPtr->taskID][2] -= qPtr->currentBurst;

            }

            // this block accesses NULL when a task's burst is 0, which is dangerous
            if (qPtr->currentBurst == 0) {
                // update waiting time
                
                // printf("\n^T%d Waiting Time: %d\n\n", qPtr->taskID, timeLine - qPtr->initialBurst);
                totalWaitingTime += (timeLine - qPtr->initialBurst);
                totalTurnaround += timeLine;
                Task* taskToPop = qPtr;
                // printf("Last operation on T%d\n", qPtr->taskID);
                pop(queue, taskToPop);   
            }
            qPtr = qPtr->next;
        }
        // printf("\n\nPriority pointer: %d\n\n", priorityPtr);

    }

    // printf("\n\n\n Lines: %d \n\n\n", lines);
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
    free(queue);

}