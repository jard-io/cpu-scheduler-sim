/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H

// representation of a task
typedef struct Task {
    int taskID;
    int priority;
    int currentBurst;
    int initialBurst;
    struct Task* prev;
    struct Task* next;
} Task;

#endif
