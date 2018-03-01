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
1) ProcessInitialiser : The module responsible for registering a process as soon as it enters the system.
2) Process Structure : This structure consists of a skeleton of the details associated with a process viz. ProcessID, ProcessState, WaitingTime, Priority, EstimatedRunTime (ERT).
3) Priority incrementer/Assigner : This is a module that assigns priority following the requirements of the algorithm.
4) CPU-State : This structure contains variables which provide information about the current running process, clock value and processor's speed.
5) Clock Setter : Determines the processor speed.
6) Process Queue Organiser: This module maintains a queue that at every instance of time, stores the sequence of processes to be dispatched to the CPU based on the priority.
7) Dispatcher : The one responsible for assigning the CPU, a process for execution.

> -- OPTIONAL MODULES--
8) Estimated Run Time Calculator (If the ERT is not predefined).



