#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define BUF_SIZE 65536
#define Q 10 // quant

typedef struct Task {
    int taskID;
    int priority;
    int currentBurst;
    int initialBurst;
    struct Task* prev;
    struct Task* next;
} Task;

typedef struct Queue {
    Task* top;
    Task* last;
    int size;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        perror("Failed to create Queue");
        exit(EXIT_FAILURE);
    }

    queue->top = NULL;
    queue->size = 0;
    return queue;
}

Task* createTask(int task[]) {
    Task* newTask = (Task*)malloc(sizeof(Task));
    if (!newTask) {
        perror("Failed to queue task");
        exit(EXIT_FAILURE);
    }

    newTask->taskID = task[0];
    newTask->priority = task[1];
    newTask->currentBurst = task[2];
    newTask->initialBurst = task[3];
    newTask->prev = NULL;
    newTask->next = NULL;
    return newTask;
}

void push(Queue* queue, int task[]) {
    Task* newTask = createTask(task);
    if (queue->top) {
        queue->last->next = newTask;
        newTask->prev = queue->last;
        queue->last = newTask;
    }
    else {
        queue->top = newTask;
        queue->last = newTask; 
    }
    queue->size++;
}

int pop(Queue* queue, Task* task){
    // printf("Popped T%d\n", task->taskID);
    if (task->prev) {
        task->prev->next = task->next;
    }
    else {
        // if we're removing the top element, update top pointer
        queue->top = task->next;
    }

    if (task->next) {
        task->next->prev = task->prev;
    }
    else {
        // if we're removing the last element, update last pointer
        queue->last = task->prev;
    }


    int lastBurst = task->currentBurst;
    free(task);
    queue->size--;
    // printf("\nQueue size: %d\n", queue->size);
    // printf("Done popping!\n");
    return lastBurst;
}

void printQueue(Queue* queue) {
    Task* current = queue->top;
    printf("\nQueue: ");

    while(current) {
        printf("T%d ", current->taskID);
        current = current->next;
    }
    printf("\n");
}

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

    Queue* queue = createQueue();

    int priorityPtr = 0;
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
        // printQueue(queue);

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
                    printf("Running task = [T%d] [%d] [%d] for %d units.\n", qPtr->taskID, qPtr->priority, qPtr->currentBurst, qPtr->currentBurst);
                    timeLine += qPtr->currentBurst;
                    qPtr->currentBurst -= qPtr->currentBurst;
                    // tasks[qPtr->taskID][2] -= qPtr->currentBurst;
                }
                else {
                    printf("Running task = [T%d] [%d] [%d] for 10 units.\n", qPtr->taskID, qPtr->priority, qPtr->currentBurst);
                    timeLine += Q;
                    qPtr->currentBurst -= Q;
                    // tasks[qPtr->taskID][2] -= Q;

                    // printf("Ok!\n");
                }
            }    
            else{
                printf("Running task = [T%d] [%d] [%d] for %d units.\n", qPtr->taskID, qPtr->priority, qPtr->currentBurst, qPtr->currentBurst);
                timeLine += qPtr->currentBurst;  
                qPtr->currentBurst -= qPtr->currentBurst;
                // tasks[qPtr->taskID][2] -= qPtr->currentBurst;

            }



            // this block accesses NULL when a task's burst is 0, which is dangerous
            if (qPtr->currentBurst == 0) {
                // update waiting time
                
                // printf("\n^T%d Waiting Time: %d\n\n", qPtr->taskID, qPtr->initialBurst);
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
    fclose(file);
    return 0;
}