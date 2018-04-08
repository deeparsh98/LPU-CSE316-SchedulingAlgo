#include<stdio.h>
#include<stdlib.h>
#include"processor.h"
#include"priority_q.h"
#include"viewable.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
//STRUCTURES AND SKELETONS


void printFile(struct i_frame* frame,char* s){
	FILE *f=fopen(s,"r");
	char ch;
	//printf("\n");
	while(!feof(f)){
		fread(&ch,1,1,f);
		fprint(frame,"%c",ch);
	}
	fclose(f);
}

//prints the main menu for the aplication
void main_menu(struct i_frame* heading,struct i_frame*frame,float* tick){
	//border(frame,'-','|',VISIBLE);
	border(heading,'=','|',VISIBLE);
	fprint(heading,"\n                               SJF NON-PREEMPTIVE SHEDULING (PREVENTING STARVATION)");
	fprint(frame,"\nWELCOME ! to the simulation based program for implementing Shortest job first (non-preemptive) scheduling that prevents starvation\n.");
	fprint(frame,"THIS MENU WILL GUIDE YOU FOR USING THE SIMULATION.\n");
 	fprint(frame,"1. Start the Simulation with default values (CLOCK TICK = 1 second).\n2. Change the processor's clock tick value.\n3. Introduction");
	fprint(frame,"\nPLEASE ENTER YOUR CHOICE");
	int ch;
	fscan(frame,"%d",&ch);
	switch(ch){
		case 1:break;
		case 2:
			clear(frame);
			fprint(frame,"\nCLOCK UPDATION\nEnter the required value (can be float)\n");
			fscan(frame,"%f",tick);
			fprint(frame,"CLOCK TICK UPDATED! CONTINUING IN 2 sec");
			printf("\n");
			usleep(2*1000000);
			break;
		case 3:
			clear(frame);
			printFile(frame,"intro.txt");
			fprint(frame,"\nENTER ANY VALUE TO CONTINUE");
			scanf("%d",&ch);
			break;
		default: ;
	}

}

int main(){
	clearscr();
	struct i_frame *main_frame=makeframe(130,30,20,8), *headingM=makeframe(130,3,20,2);
	struct CPU_State cpu;
	struct C_P an_instance;
	an_instance.CPU=&cpu;
	an_instance.P=NULL;//avoidance of the wild pointer...
	if(!CPU_Initialiser(&an_instance,1))
		printf("Error in initialising...!!");
	//PRINTING MAIN MENU
	main_menu(headingM,main_frame,&(an_instance.CPU->Clock_Speed));
	clear(main_frame);
	//PRINTING IMPORTANT INSTRUCTIONS
	printFile(main_frame,"instructions.txt");
	fprint(main_frame,"\nENTER ANY THING TO CONTINUE..!");
	int t;
	scanf("%d",&t);
	clearscr();
	delframe(main_frame);
	border(headingM,'=','|',VISIBLE);
	fprint(headingM,"");
	Simulation(&an_instance);
	delframe(headingM);
	//truncate();
}



