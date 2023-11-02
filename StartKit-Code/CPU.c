/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h>

#include "task.h"

// run this task for the specified time slice
void run(Task *task, int slice) {
    printf("Running task = [T%d] [%d] [%d] for %d units.\n",task->taskID, task->priority, task->currentBurst, slice);
}
