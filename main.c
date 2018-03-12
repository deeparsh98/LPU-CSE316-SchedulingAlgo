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
	int ERT,RT,WaitTime;
	enum States state;
};
struct CPU_State{
	int Cur_Process;
	int P_Speed;
}CPU;
struct P_queue{
	int PID;
	int priority;
	struct P_queue* next;
}front*,rear*;

//MODULES
bool truncate(){
	while(front!=NULL){
	Del_queue();
	}
	return 1;
}

int Del_queue(){
	int id=0;
	struct P_queue temp*;
	if(front==NULL){return -1;}
	temp=front;
	id=(temp->PID);
	if(front==rear){
		free(front);
		front=NULL;
		rear=NULL;
	}
	else{
		front=front->next;
		free(temp);
	}
	return id;
}
bool Ins_queue(int pid,int priority){
	struct P_queue ele*=(struct P_queue*)malloc(sizeof(struct P_queue));
	ele->PID=item;
	ele->priority=priority;
	struct P_queue* Loc,*Prev=NULL;
	Loc=front;
	while(Loc!=NUL){
		if((Loc->priority)<priority)
			break;
		Prev=Loc;
		Loc=Loc->next;
	}
	if(front==NULL){
		ele->next=NULL;
		front=ele;
		rear=ele;
	}
	else if(Prev==NULL){
		ele->next=front;
		front=ele;
	}
	else if(Prev==rear){
		ele->next=NULL;
		rear->next=ele;
		rear=ele;
	}
	else{
		ele->next=Loc;
		Prev=ele;
	}
	return 1;
}


bool PriorityAssigner(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state!=RUNNING&&(p+i)->state!=TERMINATED)
			(p+i)->priority=1+((p+i)->WaitTime)/((p+i)->ERT);
		}
	return 1;
}
bool WaitTimeIncremener(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state!=RUNNING&&(p+i)->state!=TERMINATED)
			((p+i)->WaitTime)++;
	}
	return 1;
}
bool ERTDecrementer(struct ProcessStructure *p){
	(p->RT)--;
	if(p->RT==0)
		p->state=TERMINATED;
	return 1;
}

struct ProcessStructure ProcessInitialiser(int PID,int ERT){
	struct ProcessStructure p;
	p.WaitTime=0;
	p.PID=PID;
	p.ERT=ERT;
	p.RT=ERT;
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
bool Dispatcher(struct ProcessStructure* P){
	if(CPU.Cur_Process!=-1){     //Check if CPU is having no process. Just in case...
		//if state is not TERMINATED
		Ins_queue(CPU.Cur_Process)
		//CHANGE STATE TO READY
	}
	CPU.Cur_Process=Del_queue();
	//CHANGE STATE TO RUNNING
}

//SChEDULAR
bool Schedular(struct ProcessStructure* P,int n){
	if(front!=NULL){
		for(int i=0;i<n-1;i++){
			truncate();
		}
	}
	for(int i=0;i<n;i++){
		if((P+i)->state!=RUNNING&&(P+i)->state!=TERMINATED)
			Ins_queue((P+i)->PID,(P+i)->priority);
	}
}

bool Processor(struct ProcessStructure* P){
	//DO WHILE NO PROCESS LEFT...
	//Call sleep(1)
	TIMER++;
	Dispatcher();
	WaitTimeIncrementer(P,ID_COUNTER,CPU.Cur_Process);
	//if((P+CPU.Cur_Process-1)->ERT==0)
		//Dispatcher(P+CPU.Cur_Process-1); 

}
bool Simulation(int timer,int pidCount){
	TIMER=timer;
	ID_COUNTER=pidCount;
	struct ProcessStructure* processes=ProcessRegister();
	pid_t pid=fork();
	switch(pid){
	case 0:
		//Calls Process thrower
		break;
	default:
		Processor();
		//Calls Processor
	}

	free(processes);

}

int main(){
	Simulation(0,0);
}
