#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

int ID_COUNTER=0,TIMER=0;
int interrupt=0;
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

struct P_queue{
	int PID;
	int ERT;
	int priority;
	struct P_queue* next;
}*front,*rear;


int Del_queue(){
	int id=0;
	struct P_queue* temp;
	if(front==NULL){
		return -1;}
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


bool truncate(){
	while(front!=NULL){
	Del_queue();
	}
	return 1;
}

bool Ins_queue(int pid,int priority, int ert){
	struct P_queue* ele=(struct P_queue*)malloc(sizeof(struct P_queue));
	ele->PID=pid;
	ele->ERT=ert;
	ele->priority=priority;
	struct P_queue* Loc,*Prev=NULL;//SuperPrev=NULL;
	Loc=front;
	if(front==NULL){
		ele->next=NULL;
		front=ele;
		rear=ele;
		return 1;
	}
	while(Loc!=NULL){
		if((Loc->priority)==priority){
			//check for ERT
			if(ert< Loc->ERT)
				break; 
		}
		else 
		if((Loc->priority)<priority){
			break;
		}
		Prev=Loc;
		Loc=Loc->next;
	}

	if(Prev==NULL){
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
		Prev->next=ele;
	}
	return 1;
}


struct CPU_State{
	int Cur_Process;
	int Clock_Speed;
}CPU;

//MODULES

bool PriorityAssigner(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state==READY||(p+i)->state==WAITING){
		//checking below if waittime is negative
			(p+i)->priority=((p+i)->WaitTime)<0 ? 1+0/((p+i)->ERT) :1+((p+i)->WaitTime)/((p+i)->ERT);
		}
	}
	return 1;
}
bool WaitTimeIncrementer(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state==READY||(p+i)->state==WAITING)
			((p+i)->WaitTime)++;
	}
	return 1;
}

bool ERTDecrementer(struct ProcessStructure *p){
	(p->RT)--;
	if(p->RT<=0){
		p->state=TERMINATED;
		return 0;
	}
	return 1;
}
struct ProcessStructure ProcessInitialiser(int PID,int ERT){
	struct ProcessStructure p;

	p.PID=PID;
	p.ERT=ERT;
	p.RT=ERT;
	p.ArrivalTime=PID;
	p.WaitTime=-p.ArrivalTime;
	//priority
	p.state=WAITING;
	PriorityAssigner(&p,1);
	//p.priority=1;

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

	while(i>=0){
		printf("PID : %d\nERT : %d\nWaittime : %d\nPriority : %d\nAT : %d\nState : %s\n",(P+i)->PID,(P+i)->ERT,(P+i)->WaitTime,(P+i)->priority,(P+i)->ArrivalTime,ST_ARRAY[(P+i)->state]);
		i--;
	}
	return P;

}

bool Dispatcher(struct ProcessStructure* P){
	if(CPU.Cur_Process!=-1){     //Check if CPU is having no process. Just in case...
		//if state is not TERMINATED
		if((P+CPU.Cur_Process-1)->state!=TERMINATED){
			Ins_queue(CPU.Cur_Process,(P+CPU.Cur_Process-1)->priority,(P+CPU.Cur_Process-1)->ERT);
			//CHANGE STATE TO READY
			(P+CPU.Cur_Process-1)->state=READY;
		}
	}
	CPU.Cur_Process=Del_queue();
	//CHANGE STATE TO RUNNING
	(P+CPU.Cur_Process-1)->state=RUNNING;
}

//SChEDULAR
bool Schedular(struct ProcessStructure* P,int n){
	if(front!=NULL){
		truncate();
	}

	for(int i=0;i<n;i++){
		if((P+i)->state==READY)
			Ins_queue((P+i)->PID,(P+i)->priority,(P+i)->ERT);
	}
}

bool Processor(struct ProcessStructure* P){

	Dispatcher(P);
/*
	struct P_queue *p=front;
	while(p!=NULL){
	printf("%d ",p->PID);
	p=p->next;
	}
	printf("\n%d",CPU.Cur_Process);*/

	interrupt=-20;
	do{
	//DO WHILE NO PROCESS LEFT...
	printf("TIMER : %d",TIMER);

	if(front!=NULL||(CPU.Cur_Process!=-1)){  //if CPU IS NOT IDLE
		WaitTimeIncrementer(P,ID_COUNTER);
		PriorityAssigner(P,ID_COUNTER);
		Schedular(P,ID_COUNTER);
		printf(" - P%d Running\n",CPU.Cur_Process);
		if(ERTDecrementer(P+CPU.Cur_Process-1)==0)
			Dispatcher(P);
	}
	else if(CPU.Cur_Process==-1)
		printf(" - CPU is IDLE\n");
	sleep(1);
	TIMER++;
	interrupt++;
	}while((front!=NULL||interrupt<=0)&&interrupt<=0);

}

bool Simulation(int timer,int pidCount){
	front=NULL;
	rear=NULL;
	TIMER=timer;
	ID_COUNTER=pidCount;
	struct ProcessStructure* processes=ProcessRegister();
	//pid_t pid=fork();
	int pid=2;
	switch(pid){
	case 0:
		//Calls thrower...
		break;
	default:
		Processor(processes);
		//calls Processing
	}
	
	free(processes);
	printf("PROCESSES FREED\n");
}

int main(){
	CPU.Cur_Process=-1;
	//CPU.Clock_Speed=1;
	//Simulation(0,0);
	front=NULL;
	rear=NULL;
	struct ProcessStructure p[4];
	TIMER=0;
	ID_COUNTER=0;
	Ins_queue(1,1,4);
	Ins_queue(2,1,2);
	Ins_queue(3,1,5);
	Ins_queue(4,1,3);
	//printf("%d %d %d \n",front->PID,front->next->PID,front->next->next->PID);

	p[0].PID=1;
	p[0].ERT=4;
	p[0].priority=4;
	p[0].state=READY;
	p[0].ArrivalTime=0;
	p[0].WaitTime=0;
	p[0].RT=4;

	p[1].PID=2;
	p[1].ERT=2;
	p[1].priority=5;
	p[1].state=READY;
	p[1].WaitTime=0;
	p[1].RT=2;

	p[2].PID=3;
	p[2].ERT=5;
	p[2].priority=6;
	p[2].state=READY;
	p[2].WaitTime=0;
	p[2].RT=5;

	p[3].PID=4;
	p[3].ERT=3;
	p[3].priority=4;
	p[3].state=READY;
	p[3].WaitTime=0;
	p[3].RT=3;
	ID_COUNTER=4;
	//PriorityAssigner(p,4);
	
	/*for(int i=1;i<=5;i++){
	printf("%d %d %d \n",front->PID,front->next->PID,front->next->next->PID);//,front->next->next->next->PID);
	sleep(1);
	WaitTimeIncrementer(p,4);
	PriorityAssigner(p,4);
	Schedular(p,4);
	}*/
	Processor(p);
	truncate();
}



