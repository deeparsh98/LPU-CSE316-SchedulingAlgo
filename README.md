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
1) Process Structure : This structure consists of a skeleton of the details associated with a process viz. ProcessID, ProcessState, WaitingTime, Priority, EstimatedRunTime (ERT).
2) Priority incrementer/Assigner : This is a module that assigns priority following the requirements of the algorithm.
3) CPU-State : This structure contains variables which provide information about the current running process, clock value and processor's speed.
4) Clock Setter : Determines the processor speed.
5) Process Queue Organiser: This module maintains a queue that at every instance of time, stores the sequence of processes to be dispatched to the CPU based on the priority.

> -- OPTIONAL MODULES--
6) Estimated Run Time Calculator (If the ERT is not predefined).



