#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
int ID_COUNTER=0,TIMER=0;
const char ST_ARRAY[][11]={"TERMINATED","WAITING","RUNNING","READY"};
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
	p.ArrivalTime=TIMER;
	return p;
}

struct ProcessStructure* ProcessRegister(){
	int ch;
	int temp_ERT;
	struct ProcessStructure* P=(struct ProcessStructure*)malloc(sizeof(struct ProcessStructure));
	int i=0; //iterator for initialisation of structures;
	do{
		printf("ENTER ERT\n");
		scanf("%d",&temp_ERT);
		*(P+i)=ProcessInitialiser(++ID_COUNTER,temp_ERT);
		//ALLOCATING MEMORY FOR ANOTHER PROCESS
		printf("ANYMORE PROCESS?\n");
		scanf("%d",&ch);
		if(ch){
			P=realloc(P,(i+2)*sizeof(struct ProcessStructure));
			i++;
		}
	}while(ch);
	//PRINTING FOR DEBUGING...
	while(i>=0){
		printf("PID : %d\nERT : %d\nWaittime : %d\nPriority : %d\nAT : %d\nState : %s\n",(P+i)->PID,(P+i)->ERT,(P+i)->WaitTime,(P+i)->priority,(P+i)->ArrivalTime,ST_ARRAY[(P+i)->state]);
		i--;
	}
	return P;

}
bool Simulation(int timer,int pidCount){
	TIMER=timer;
	ID_COUNTER=pidCount;
	struct ProcessStructure* processes=ProcessRegister();
	pid_t pid=fork();
	switch(pid){
	case 0:
		while(1){
			sleep(1);
			TIMER++;
		}
		break;
	default:
	}

	free(processes);

}

int main(){
	Simulation(0,0);
}
