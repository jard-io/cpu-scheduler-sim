## Simulating the Common CPU Scheduling Algorithms
**Start:** Oct 16, 2023, at 09:00am   
**Due:** Nov 12, 2023, at 09:00pm 

This project involves implementing (i.e., simulating) several different process scheduling algorithms. 
The scheduler will be assigned a predefined set of tasks and will schedule the tasks based on the selected scheduling 
algorithm. Each task is assigned a priority and CPU burst. The following scheduling algorithms will be implemented:

* First-come, first-served (FCFS), which schedules tasks in the order in which they request the CPU.
* Shortest-job-first (SJF), which schedules tasks in order of the length of the tasks’ next CPU burst.
* Priority scheduling, which schedules tasks based on priority. 
* Round-robin (rr) scheduling, where each task is run for a time quantum (or for the remainder of its CPU burst).
* Priority with round-robin, which schedules tasks in order of priority and uses round-robin scheduling for tasks with equal priority.

Priorities range from 1 to 10, where a higher numeric value indicates a higher relative priority. 
For rr scheduling, the length of a time `quantum` is `10` milliseconds.




