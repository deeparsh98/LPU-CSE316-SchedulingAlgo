#include<stdio.h>
#include<stdbool.h>
#include<pthread.h>
#include"processor.h"
#include"priority_q.h"
#include<stdlib.h>
#include"viewable.h"

const char ST_ARRAY[][11]={"TERMINATED","WAITING","RUNNING","READY"};
struct P_queue *front=NULL,*rear=NULL;
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

void print_queue(struct i_frame* frame,struct ProcessStructure* p){
	struct P_queue* ptr=front;
	while(ptr!=NULL){
		if(ptr!=front)
			fprint(frame,"\n");
		fprint(frame,"%d    %d    %f",ptr->PID,p[ptr->PID-1].WaitTime,ptr->priority);
		ptr=ptr->next;
	}
}

bool Ins_queue(int pid,float priority, int ert){
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

//MODULES

bool PriorityAssigner(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state==READY){
		//check if waittime is negative
			(p+i)->priority=((p+i)->WaitTime)<0 ? 1+0/((p+i)->ERT) :1+((p+i)->WaitTime)/(float)((p+i)->ERT);
		}
	}
	return 1;
}
bool WaitTimeIncrementer(struct ProcessStructure *p,int n){
	for (int i=0;i<n;i++){
		if((p+i)->state==READY)
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
bool ProcessInitialiser(struct ProcessStructure* p,int PID,int ERT){

	p->PID=PID;
	p->ERT=ERT;
	p->RT=ERT;
	p->ArrivalTime=0;
	p->WaitTime=0;
	//priority
	p->state=WAITING;
	PriorityAssigner(p,1);
	//p.priority=1;

	return 1;

}

struct ProcessStructure* ProcessRegister(struct CPU_State* CPU){
	int ch;
	int temp_ERT;
	struct i_frame* e_frame=makeframe(30,30,100,8),*headingE=makeframe(30,3,100,4);
	border(e_frame,'-','|',VISIBLE);
	fprint(headingE,"\n\nPROCESS ENTRY");
	struct ProcessStructure* P=(struct ProcessStructure*)malloc(sizeof(struct ProcessStructure));
	int i=0; //iterator for initialisation of structures;
	do{
		fprint(e_frame,"ENTER ERT\n");
		fscan(e_frame,"%d",&temp_ERT);
		if(temp_ERT==0){
			fprint(e_frame,"A PROCESS WITH NO RUN TIME IS NOT POSSIBLE. PROCESS DISCARDED!...ANYMORE PROCESS?\n");
			fscan(e_frame,"%d",&ch);
			if(ch)
				continue;
			else
				break;
		}
		ProcessInitialiser((P+i),++(CPU->PROCESS_COUNTER),temp_ERT);
		++(CPU->nop);
		//ALLOCATING MEMORY FOR ANOTHER PROCESS
		fprint(e_frame,"ANYMORE PROCESS?\n");
		fscan(e_frame,"%d",&ch);
		if(ch){
			P=realloc(P,(i+2)*sizeof(struct ProcessStructure));
			i++;
		}
	}while(ch);

	delframe(e_frame);
	delframe(headingE);
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
	//and sets the arrival time when user presses a specific key
	struct ProcessStructure* P=((struct C_P*)Object)->P;
	struct CPU_State* CPU=((struct C_P*)Object)->CPU;
	//struct i_frame *t_frame=makeframe(1,1,1,80); 
	int c;
	for(int i=0;i< (CPU->PROCESS_COUNTER);i++){
	scanf("%d",&c);

	if(c<=0){
		CPU->INTERRUPT=1;
		break;
	}
	if(P[c-1].state!=TERMINATED){
	Ins_queue(c,P[c-1].priority,P[c-1].ERT);
	P[c-1].state=READY;
	P[c-1].ArrivalTime=CPU->CLOCK;
	}
	}
	while(c>0)
		scanf("%d",&c);
	CPU->INTERRUPT=1;
	
	//LOCK=0;
	pthread_exit(NULL);
}

void makeProcessTable(struct i_frame* frame,struct ProcessStructure* p,struct CPU_State* cpu){
	for(int i=0;i< cpu->nop;i++){
		if(p[i].state!=WAITING){
			fprint(frame,"%d      %d      %d      %d      %s\n",p[i].PID,p[i].ERT,p[i].ArrivalTime,p[i].WaitTime,ST_ARRAY[p[i].state]);
		}
		else
			fprint(frame,"\n");
	}

}
void* Processor(void* Object){

	struct ProcessStructure *P=((struct C_P*)Object)->P;
	struct CPU_State* CPU=((struct C_P*)Object)->CPU;
	//while(LOCK==1);

	struct i_frame *a_frame=makeframe(40,CPU->nop+1,50,18),*p_frame=makeframe(25,3,10,8),*timer_frame=makeframe(8,3,38,8),*q_frame=makeframe(25,5,10,18);
	struct i_frame *headingA=makeframe(40,3,50,14),*headingP=makeframe(35,3,10,4),*headingQ=makeframe(25,3,10,14);
	border(a_frame,'-','|',VISIBLE);
	border(p_frame,'-',':',VISIBLE);
	border(timer_frame,'-',':',VISIBLE);
	border(q_frame,'-','|',VISIBLE);
	fprint(headingA,"    PROCESS TABLE\n\nPID   ERT   ARR. T.   WAIT T.   STATE");
	fprint(headingP,"    PROCESSOR\n\n PID   WAIT T.  REM. T.     CLOCK");
	fprint(headingQ,"SCHEDULING QUEUE\n\n PID   WAIT T.  PRIORITY");
	//Dispatcher(P,CPU);
	do{
	//DO WHILE NO PROCESS LEFT...
	makeProcessTable(a_frame,P,CPU);
	fprint(timer_frame,"\n T : %d",CPU->CLOCK);
	if((front!=NULL||CPU->Cur_Process!=-1)&&CPU->Cur_Process!=-1){  //if CPU IS NOT IDLE
		WaitTimeIncrementer(P,CPU->PROCESS_COUNTER);
		PriorityAssigner(P,CPU->PROCESS_COUNTER);
		Schedular(P,CPU->PROCESS_COUNTER);
		fprint(p_frame,"\n %d      %d      %d",CPU->Cur_Process,P[CPU->Cur_Process-1].WaitTime,P[CPU->Cur_Process-1].RT);
		clear(q_frame);
		print_queue(q_frame,P);	
		if(ERTDecrementer(P+CPU->Cur_Process-1)==0)
			Dispatcher(P,CPU);
	}
	else if(CPU->Cur_Process==-1){
		fprint(p_frame,"\n       CPU IS IDLE");
		Dispatcher(P,CPU);
	}

	if(CPU->CLOCK%2==0)
		border(timer_frame,'-',':',NO_VISIBLE);
	else
		border(timer_frame,'-',':',VISIBLE);
	printf("\n");
	usleep((int)CPU->Clock_Speed*1000000);
	CPU->CLOCK++;
	clear(p_frame);
	clear(a_frame);
	clear(timer_frame);
	}while((front!=NULL||CPU->INTERRUPT<=0)&&CPU->INTERRUPT<=0);
	//EXITING THREAD
	seek_cur(50,2);
	//DEALLOCATING MEMORY USED BY THE FRAMES
	delframe(a_frame);
	delframe(p_frame);
	delframe(timer_frame);
	delframe(headingA);
	delframe(headingP);
	delframe(headingQ);

	//FREEINF THE QUEUE IF NOT NULL
	if(front!=NULL){
	truncate();
	}
	printf("THREAD EXIT\n");
	pthread_exit(NULL);
}

bool Simulation(struct C_P* Instance){
	pthread_t Process,Throw;
	init();
	Instance->P=ProcessRegister(Instance->CPU);
	pthread_create(&Process,NULL,&Processor,Instance);
	pthread_create(&Throw,NULL,&Thrower,Instance);
	pthread_join(Process,NULL);
	pthread_join(Throw,NULL);

	free(Instance->P);
	printf("MEMORY FREED\n");
}
bool CPU_Initialiser(struct C_P* instancePtr,float Clock_Speed){
	struct CPU_State* Cptr=instancePtr->CPU;
	Cptr->Cur_Process=-1;
	Cptr->Clock_Speed=Clock_Speed;
	Cptr->CLOCK=0;
	Cptr->INTERRUPT=0;
	Cptr->PROCESS_COUNTER=0;
	Cptr->nop=0;
	return 1;
}

