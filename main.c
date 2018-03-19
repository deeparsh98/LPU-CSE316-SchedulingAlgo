#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<pthread.h>

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
}*front=NULL,*rear=NULL;


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
	int PROCESS_COUNTER;
	int CLOCK;
	int INTERRUPT;
};

struct C_P{
	struct CPU_State* CPU;
	struct ProcessStructure* P;
};
//MODULES

bool PriorityAssigner(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state==READY||(p+i)->state==WAITING){
		//check if waittime is negative
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
	p.ArrivalTime=0;
	p.WaitTime=0;
	//priority
	p.state=WAITING;
	PriorityAssigner(&p,1);
	//p.priority=1;

	return p;

}

struct ProcessStructure* ProcessRegister(int* ID_COUNTER){
	int ch;
	int temp_ERT;
	struct ProcessStructure* P=(struct ProcessStructure*)malloc(sizeof(struct ProcessStructure));
	int i=0; //iterator for initialisation of structures;
	do{
		printf("ENTER ERT\n");
		scanf("%d",&temp_ERT);
		*(P+i)=ProcessInitialiser(++(*ID_COUNTER),temp_ERT);
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

bool Dispatcher(struct ProcessStructure* P,struct CPU_State* CPU){
	int CurP=CPU->Cur_Process;
	if(CurP!=-1){     //Check if CPU is having no process. Just in case...
		//if state is not TERMINATED
		if((P+CurP-1)->state!=TERMINATED){
			Ins_queue(CurP,(P+CurP-1)->priority,(P+CurP-1)->ERT);
			//CHANGE STATE TO READY
			(P+CurP-1)->state=READY;
		}
	}
	CPU->Cur_Process=Del_queue();
	//CHANGE STATE TO RUNNING
	(P+CPU->Cur_Process-1)->state=RUNNING;
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
void* Thrower(void* Object){
	//This function throws processes in the queue
	//and sets the arrival time when user presses a specific key.
	struct ProcessStructure* P=((struct C_P*)Object)->P;
	struct CPU_State* CPU=((struct C_P*)Object)->CPU;
	int c;
	for(int i=0;i< (CPU->PROCESS_COUNTER);i++){
	scanf(" %d",&c);
	printf("INPUT ENTERED\n");
	if(c<=0){
		CPU->INTERRUPT=1;
		break;
	}
	if(P[c-1].state!=TERMINATED){
	Ins_queue(c,P[c-1].priority,P[c-1].ERT);
	P[c-1].state=READY;
	}
	}
	while(c>0)
		scanf(" %d",&c);
	CPU->INTERRUPT=1;
	//LOCK=0;
	pthread_exit(NULL);
}

void* Processor(void* Object){

struct ProcessStructure *P=((struct C_P*)Object)->P;
struct CPU_State* CPU=((struct C_P*)Object)->CPU;
	//while(LOCK==1);
	Dispatcher(P,CPU);
	do{
	//DO WHILE NO PROCESS LEFT...
	printf("TIMER : %d",CPU->CLOCK);
	if((front!=NULL||CPU->Cur_Process!=-1)&&CPU->Cur_Process!=-1){  //if CPU IS NOT IDLE
		WaitTimeIncrementer(P,CPU->PROCESS_COUNTER);
		PriorityAssigner(P,CPU->PROCESS_COUNTER);
		Schedular(P,CPU->PROCESS_COUNTER);
		printf(" - P%d Running\n",CPU->Cur_Process);
		if(ERTDecrementer(P+CPU->Cur_Process-1)==0)
			Dispatcher(P,CPU);
	}
	else if(CPU->Cur_Process==-1){
		printf(" - CPU is IDLE\n");
		Dispatcher(P,CPU);
	}
	sleep(1);
	CPU->CLOCK++;
	}while((front!=NULL||CPU->INTERRUPT<=0)&&CPU->INTERRUPT<=0);

	//EXITING THREAD
	printf("THREAD EXIT\n");
	pthread_exit(NULL);
}

bool Simulation(struct C_P* Instance){
	pthread_t Process,Throw;

	Instance->P=ProcessRegister(&Instance->CPU->PROCESS_COUNTER);
	
	pthread_create(&Process,NULL,&Processor,Instance);
	pthread_create(&Throw,NULL,&Thrower,Instance);
	pthread_join(Process,NULL);
	pthread_join(Throw,NULL);

	free(Instance->P);
	printf("MEMORY FREED\n");
}
bool CPU_Initialiser(struct C_P* instancePtr,int Clock_Speed){
	struct CPU_State* Cptr=instancePtr->CPU;
	Cptr->Cur_Process=-1;
	Cptr->Clock_Speed=Clock_Speed;
	Cptr->CLOCK=0;
	Cptr->INTERRUPT=0;
	Cptr->PROCESS_COUNTER=0;
	return 1;
}
int main(){
	struct CPU_State cpu;
	struct C_P an_instance;
	an_instance.CPU=&cpu;
	an_instance.P=NULL;//avoidance of the wild pointer...
	if(!CPU_Initialiser(&an_instance,1))
		printf("Error in initialising...!!");
	Simulation(&an_instance);
	truncate();
}



