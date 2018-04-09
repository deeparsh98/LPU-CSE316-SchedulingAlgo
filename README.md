# LPU-CSE316-SchedulingAlgo
A linux based project to implement scheduling algorithm.

Based on the following Problem...
> Consider a scheduling approach which is non pre-emptive similar to shortest job next in
nature. The priority of each job is dependent on its estimated run time, and also the amount of
time it has spent waiting. Jobs gain higher priority the longer they wait, which prevents
indefinite postponement. The jobs that have spent a long time waiting compete against those
estimated to have short run times. The priority can be computed as :
Priority = 1+ Waiting time / Estimated run time.

# Modules/Structures Specification
> --IMPORTANT MODULES-- 
1.  PROCESSOR(Object) //Object is a combined object that contains processes as well as CPU.
/* This function performs all the tasks of a typical processor related to process scheduling.
    • Checks for the queue, and calls dispatcher when the queue just receives a process.
    • Increments waiting time of the processes that are in the ready state and hence updates their priorities by calling                  
      WaitTimeIncrementer() and PriorityAssigner().
    • Calls QScheduler() that re-shuffles the queue according to the updates priorities.
    • Checks for the termination of a process and if it’s done calls dispatcher to assign another process.
    • Increments the clock for the next tick.*/
2.  THROWER (Object) //Object is a combined object that contains processes as well as CPU
/* This function simulates the arrival of the processes by throwing them in the queue and is executed simultaneously with the ‘Processor’ function .
    • User enters PID of the process to show the arrival of the process during the execution at various instances of time 
    • Arrival time is given accordingly (When the process of PID ‘n’ is entered by user)
    • The processes take their respective places in the queue according to their priority
*/
> -- OPTIONAL MODULES--
1. WaitTimeIncrementer() –  This function increments the wait time of all the processes that are in the queue. Takes a structure of the processes and no of processes as parameters.
2. PriorityAssigner() –  This function assigns the priority to all the processes that are in the queue. Takes a structure of the processes and no of processes as parameters.
3. ERTDecrementer() –  This function decrements the runtime of the process passed as a parameter and returns 0 if run time has has become 0.
4. Ins_queue() –  Inserts an item in the queue at its appropriate position that is, the higher is the priority more closer will it be to the front and if the priorities are same then the process with lower run time will get a nearer place. 
5. Del_queue() – Pops the element in the front and returns it.  
6. Dispatcher() – Takes the process from the cpu to the queue (by calling Ins_queue) and assigns the process from the front of the queue to the cpu (by calling Del_queue).  
7. Scheduler() – Re- shuffle the queue according to the changed priorities 
8. ProcessInitialiser()–  Initializes the processes with initial values.
9. ProcessRegister() – Takes the runtime of various processes from the user and calls
10. ProcessInitialiser() to initialize other attributes of those processes. 

Miscellenous Functions
• printStatistics() – This function is called after the simulation and it prints the statistical data extracted from the simulation like Total Wait time, Turnaround time, Efficiency, CPU usage time, CPU usage+ CPU Idle time.

ALONG WITH THESE I HAVE CREATED A LIBRARY FOR A BETTER UI USING FRAMES.
* It contains the following functions
-	makeframe() – Takes l, d, x , y as arguments and makes a frame that is of length l height/depth d, and places it at (x,y) on the screen.
-	border() –  This function takes a frame as an argument and assigns prints border according to the flags (VISIBLE/NO_VISIBLE) and characters given.
• fprint() –  This function prints formatted string to the frame passed as a parameter to the function.   
• fscan() – A function which is used to take input through standard input inside the frame.
• clear() – Takes a frame as an argument and clears its contents.
• delframe() – Takes a frame as an argument and deletes it from memory.



