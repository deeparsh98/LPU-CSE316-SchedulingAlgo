#include<stdio.h>
#include<stdbool.h>
#include<pthread.h>
#include"processor.h"
#include"priority_q.h"
#include<stdlib.h>
#include"viewable.h"
struct i_frame* seq_frame;
const char ST_ARRAY[][11]={"TERMINATED","WAITING","RUNNING","READY"};
struct P_queue *front=NULL,*rear=NULL;
//delete from the queue, the very first element
int Del_queue(){
	int id=0;
	struct P_queue* temp;
	if(front==NULL){//If no element
		return -1;}
	temp=front;
	id=(temp->PID);
	if(front==rear){
		//only one element
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

/*Empty the queue*/
bool truncate(){
	while(front!=NULL){
	Del_queue();
	}
	return 1;
}
//print the elements of the queue in the frame passed as argument
void print_queue(struct i_frame* frame,struct ProcessStructure* p){
	struct P_queue* ptr=front;
	while(ptr!=NULL){
		if(ptr!=front)
			fprint(frame,"\n");
		fprint(frame,"%d    %d    %f",ptr->PID,p[ptr->PID-1].WaitTime,ptr->priority);
		ptr=ptr->next;
	}
}


//Inserting elememts in the queue at the appropriate position
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
			(p+i)->priority=1+((p+i)->WaitTime)/((float)(p+i)->ERT);
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
	p->ArrivalTime=-1;
	p->WaitTime=0;
	p->state=WAITING;
	p->priority=1;
	return 1;

}

struct ProcessStructure* ProcessRegister(struct CPU_State* CPU){
	int ch;
	int temp_ERT;
	struct i_frame* e_frame=makeframe(30,30,65,12),*headingE=makeframe(30,3,65,8);
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
		//ALLOCATING MEMORY FOR ANOTHER PROCESS
		fprint(e_frame,"ANYMORE PROCESS?\n");
		fscan(e_frame,"%d",&ch);
		if(ch){
			P=realloc(P,(i+2)*sizeof(struct ProcessStructure));
			i++;
		}
	}while(ch);
	clearscr();
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
	if(CPU->Cur_Process!=-1)
		fprint(seq_frame,"->%d",(P+CPU->Cur_Process-1)->PID);
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
	scanf(" %d",&c);
	if(c<=0){
		CPU->INTERRUPT=1;
		break;
	}
	if(P[c-1].state!=TERMINATED&&c<=CPU->PROCESS_COUNTER){
	Ins_queue(c,P[c-1].priority,P[c-1].ERT);
	P[c-1].state=READY;
	P[c-1].ArrivalTime=CPU->CLOCK;
	}
	else i--;
	}
	while(c>0)
		scanf(" %d",&c);
	CPU->INTERRUPT=1;
	
	//LOCK=0;
	pthread_exit(NULL);
}

void makeProcessTable(struct i_frame* frame,struct ProcessStructure* p,struct CPU_State* cpu,int type){
	for(int i=0;i< cpu->PROCESS_COUNTER;i++){
		if(type){
			fprint(frame,"   %d       %d\n",p[i].PID,p[i].ERT);
		}
		else if(p[i].state!=WAITING)
			fprint(frame,"%d      %d      %d      %d      %s\n",p[i].PID,p[i].ERT,p[i].ArrivalTime,p[i].WaitTime,ST_ARRAY[p[i].state]);
		else
			fprint(frame,"\n");
	}

}
void* Processor(void* Object){

	struct ProcessStructure *P=((struct C_P*)Object)->P;
	struct CPU_State* CPU=((struct C_P*)Object)->CPU;
	//while(LOCK==1);
	int temp_flag;//STORES THE RETURNED VALUE OF ERTDecrementer()
	struct i_frame *a_frame=makeframe(45,CPU->PROCESS_COUNTER+1,110,8),*p_frame=makeframe(25,3,10,8),*timer_frame=makeframe(8,3,38,8),*q_frame=makeframe(25,10,10,18);
	struct i_frame *headingA=makeframe(40,3,110,4),*headingP=makeframe(35,3,10,4),*headingQ=makeframe(25,3,10,14);
	border(a_frame,'-','|',VISIBLE);
	border(p_frame,'-',':',VISIBLE);
	border(timer_frame,'-',':',VISIBLE);
	border(q_frame,'-','|',VISIBLE);
	fprint(headingA,"    ACTIVE PROCESS TABLE\n\nPID   ERT   ARR. T.   WAIT T.   STATE");
	fprint(headingP,"    PROCESSOR\n\n PID   WAIT T.  REM. T.     CLOCK");
	fprint(headingQ,"SCHEDULING QUEUE\n\n PID   WAIT T.  PRIORITY");
	//Dispatcher(P,CPU);
	do{
	//DO WHILE NO PROCESS LEFT...
	makeProcessTable(a_frame,P,CPU,ACTIVE);
	fprint(timer_frame,"\n T : %d",CPU->CLOCK);
	printf("\n");
	//sleep to simulate a clock tick
	usleep((int)CPU->Clock_Speed*1000000);
	clear(p_frame);
	clear(a_frame);
	clear(timer_frame);
	//initial condition i.e if the process arrives at t=0
	if(front!=NULL&&CPU->Cur_Process==-1){
		Dispatcher(P,CPU);}
	if(CPU->Cur_Process!=-1){//If CPU is not idle
		WaitTimeIncrementer(P,CPU->PROCESS_COUNTER); //increment wait time
		PriorityAssigner(P,CPU->PROCESS_COUNTER);//assign priorities
		Schedular(P,CPU->PROCESS_COUNTER);//re shuffle the queue acc to updates priorities dynamically
		temp_flag=ERTDecrementer(P+CPU->Cur_Process-1);
		if(temp_flag==0){
			Dispatcher(P,CPU);// dispatch if the process has been finished
		}
		if(CPU->Cur_Process!=-1)//print cpu state if processor not IDLE
			fprint(p_frame,"\n %d      %d      %d",CPU->Cur_Process,P[CPU->Cur_Process-1].WaitTime,P[CPU->Cur_Process-1].RT);
		clear(q_frame);
		print_queue(q_frame,P);//print the queue

	}
	if(CPU->Cur_Process==-1){
		Dispatcher(P,CPU);
			fprint(p_frame,"\n       CPU IS IDLE");
		CPU->idleFor++;
	}

	CPU->CLOCK++;// increment the clock

	}while(CPU->INTERRUPT<=0);
	//EXITING THREAD
	//DEALLOCATING MEMORY USED BY THE FRAMES
	makeProcessTable(a_frame,P,CPU,ACTIVE);
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
	CPU->idleFor--;
	CPU->CLOCK--;
	pthread_exit(NULL);
}
void printStatistics(struct ProcessStructure* p,struct CPU_State* cpu){
	struct i_frame *s_frame=makeframe(10,10,30,32), *headingS=makeframe(19,10,10,32);
	border(headingS,'-','|',VISIBLE);
	fprint(headingS,"STATISTICS\n\nNO. OF PROCESSES\nTOTAL WAIT TIME\nTURNAROUND TIME\nCPU USE TIME\nUSAGE+IDLE TIME\nEFFICIENCY");
	border(s_frame,'-',':',VISIBLE);
	int rt=0,wt=0,usage=0;
	for(int i=0;i< cpu->PROCESS_COUNTER;i++){
		rt+=p[i].ERT;
		wt+=p[i].WaitTime;
	}
	if(cpu->PROCESS_COUNTER==0)
		usage=0;
	else
		usage=cpu->CLOCK-cpu->idleFor+1;
	fprint(s_frame,"  VALUES\n\n  %d\n  %d\n  %d\n  %d\n  %d\n  %0.4f",cpu->PROCESS_COUNTER,wt,wt+usage,usage,cpu->CLOCK,100.0*((float)usage/cpu->CLOCK));
	delframe(headingS);
	delframe(s_frame);
}

bool Simulation(struct C_P* Instance){
	pthread_t Process,Throw;
	init();
	Instance->P=ProcessRegister(Instance->CPU);
	seq_frame=makeframe(70,3,50,32);
	fprint(seq_frame,"SEQUENCE\n");
	struct i_frame *d_frame=makeframe(20,Instance->CPU->PROCESS_COUNTER+1,70,8), *headingD=makeframe(20,3,70,4), *headingM=makeframe(130,2,20,2);
	border(d_frame,'-','|',VISIBLE);
	fprint(headingM,"                               SJF NON-PREEMPTIVE SCHEDULING (PREVENTING STARVATION)");
	fprint(headingD,"PROCESS DETAILS\n\n  PID     ERT");
	makeProcessTable(d_frame,Instance->P,Instance->CPU,INITIAL);
	printf("\n");
	pthread_create(&Process,NULL,&Processor,Instance);
	pthread_create(&Throw,NULL,&Thrower,Instance);
	pthread_join(Process,NULL);
	pthread_join(Throw,NULL);
	printStatistics(Instance->P,Instance->CPU);
	seek_cur(45,2);
	delframe(seq_frame);
	delframe(d_frame);
	delframe(headingD);
	delframe(headingM);
	free(Instance->P);
}
bool CPU_Initialiser(struct C_P* instancePtr,float Clock_Speed){
	struct CPU_State* Cptr=instancePtr->CPU;
	Cptr->Cur_Process=-1;
	Cptr->Clock_Speed=Clock_Speed;
	Cptr->CLOCK=0;
	Cptr->INTERRUPT=0;
	Cptr->PROCESS_COUNTER=0;
	Cptr->idleFor=0;
	return 1;
}

