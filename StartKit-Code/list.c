/**
 * Various list operations
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"



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