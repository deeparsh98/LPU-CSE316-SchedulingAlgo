#include<stdio.h>
#include<stdlib.h>
#include"processor.h"
#include"priority_q.h"

//STRUCTURES AND SKELETONS
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



