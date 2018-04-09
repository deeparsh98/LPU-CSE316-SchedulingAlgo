#ifndef __processes_h__
#define __processes_h__
#include<stdbool.h>
#include<stdio.h>
#define ACTIVE 0
#define INITIAL 1
enum States{
TERMINATED,WAITING,RUNNING,READY
};//enumeration to represent states of a process

struct CPU_State{
	int Cur_Process;
	float Clock_Speed;
	int PROCESS_COUNTER;
	int CLOCK;
	int INTERRUPT;
	int idleFor;
};//Structure for CPU instances.

struct C_P{
	struct CPU_State* CPU;
	struct ProcessStructure* P;
};//combined structure for cpu and the processes.

struct ProcessStructure{
	int PID,ArrivalTime;
	float priority;
	int ERT,RT,WaitTime;
	enum States state;
};//structure definition that stores processes with their details.

struct P_queue{
	int PID;
	int ERT;
	float priority;
	struct P_queue* next;
};
/* A node of struct type for the processor queue */

bool PriorityAssigner(struct ProcessStructure *p,int n);
bool WaitTimeIncrementer(struct ProcessStructure *p,int n);
bool ERTDecrementer(struct ProcessStructure *p);
bool ProcessInitialiser(struct ProcessStructure* p,int PID,int ERT);
struct ProcessStructure* ProcessRegister(struct CPU_State* CPU);
bool Dispatcher(struct ProcessStructure* P,struct CPU_State* CPU);
bool Schedular(struct ProcessStructure* P,int n);
void* Thrower(void* Object);
void* Processor(void* Object);
bool Simulation(struct C_P* Instance);
bool CPU_Initialiser(struct C_P* instancePtr,float Clock_Speed);
void printStatistics(struct ProcessStructure* p,struct CPU_State* cpu); 

#endif

