/**
 * list data structure containing the tasks in the system
 */

#include "task.h"

// struct node {
//     Task *task;
//     struct node *next;
// };

typedef struct Queue {
    Task* top;
    Task* last;
    int size;
} Queue;

// // insert and delete operations.
// void insert(struct node **head, Task *task);
// void delete(struct node **head, Task *task);
// void traverse(struct node *head);

Queue* createQueue();
Task* createTask(int task[]);
void push(Queue* queue, int task[]);
int pop(Queue* queue, Task* task);
void printQueue(Queue* queue);