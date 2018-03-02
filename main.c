#include<stdio.h>
#include<stdbool.h>

int ID_COUNTER=0;

//STRUCTURES AND SKELETONS
enum States{
TERMINATED,WAITING,RUNNING,READY
};
struct ProcessStructure{
	int PID,ArrivalTime;
	int priority;
	int ERT,WaitTime;
	enum States state;
};
struct CPU_State{
	int Cur_Process;
	int P_Speed;
	int TIMER;
}CPU;

//MODULES
bool PriorityAssigner(struct ProcessStructure *p){
	p->priority=1+(p->WaitTime)/(p->ERT);
	return 1;
}
bool WaitTimeIncremener(struct ProcessStructure *p){
	(p->WaitTime)++;
	return 1;
}
struct ProcessStructure ProcessInitialiser(int PID,int ERT){
	struct ProcessStructure p;
	p.WaitTime=0;
	p.PID=PID;
	p.ERT=ERT;
	//priority
	PriorityAssigner(&p);
	p.state=READY;
	p.ArrivalTime=CPU.TIMER;
	return p;
}
int main(){}


