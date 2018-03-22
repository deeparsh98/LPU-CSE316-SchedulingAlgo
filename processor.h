#ifndef __processes_h__
#define __processes_h__
#include<stdbool.h>
#include<stdio.h>

enum States{
TERMINATED,WAITING,RUNNING,READY
};//enumeration to represent states of a process

struct CPU_State{
	int Cur_Process;
	int Clock_Speed;
	int PROCESS_COUNTER;
	int CLOCK;
	int INTERRUPT;
};//Structure for CPU instances.

struct C_P{
	struct CPU_State* CPU;
	struct ProcessStructure* P;
};//combined structure for cpu and the processes.

struct ProcessStructure{
	int PID,ArrivalTime;
	int priority;
	int ERT,RT,WaitTime;
	enum States state;
};//structure definition that stores processes with their details.

struct P_queue{
	int PID;
	int ERT;
	int priority;
	struct P_queue* next;
};
/* A node of struct type for the processor queue */

bool PriorityAssigner(struct ProcessStructure *p,int n);
bool WaitTimeIncrementer(struct ProcessStructure *p,int n);
bool ERTDecrementer(struct ProcessStructure *p);
bool ProcessInitialiser(struct ProcessStructure* p,int PID,int ERT);
struct ProcessStructure* ProcessRegister(int* ID_COUNTER);
bool Dispatcher(struct ProcessStructure* P,struct CPU_State* CPU);
bool Schedular(struct ProcessStructure* P,int n);
void* Thrower(void* Object);
void* Processor(void* Object);
bool Simulation(struct C_P* Instance);
bool CPU_Initialiser(struct C_P* instancePtr,int Clock_Speed);


#endif
