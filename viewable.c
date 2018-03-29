#include<stdio.h>
#include<stdlib.h>
#include"viewable.h"
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>
#include<pthread.h>
pthread_mutex_t lock;
void init(){
	clearscr();
	pthread_mutex_init(&lock,NULL);
}
struct i_frame* makeframe(int l,int d,int x,int y){
	struct i_frame* frame=(struct i_frame*)malloc(sizeof(struct i_frame)); //dynamically allocated memory for the structure
	frame->f_array=(char*)calloc(1,l*d);//dynamically allocated array
	frame->x=x;
	frame->y=y;
	frame->l=l;
	frame->d=d;
	frame->t_array=frame->f_array;
	frame->cursor_x=0;
	frame->cursor_y=0;
	return frame;
};

void delframe(struct i_frame* frame){
	free(frame->f_array);//freeing the space for a dynamic data item of the frame
	free(frame);//deallocating the frame itself;
}
void border(struct i_frame* frame,char top,char side,int visibility){
	pthread_mutex_lock(&lock);
	for(int i=-1;i< frame->d+1;i++){
		printf("%c[%d;%df",0x1B,frame->y+i,frame->x-1);
		for(int j=-1;j< frame->l+1;j++){
			if((i==-1||i==frame->d)){//if the top and bottom side of the border then print <top>
				if(!visibility)
					printf("%c",' ');
				else 
					printf("%c",top);
			}
			else if((j==-1||j==frame->l)){//if its the side of the frame then print the <side>
				if(!visibility)
					printf("%c",' ');
				else
					printf("%c",side);
			}
			else{	  //move the printing cursor
				printf("%c[%d;%df",0x1B,frame->y+i,frame->x+j+1);
			}
		}
	}
	printf("%c[%d;%df",0x1B,frame->y+frame->cursor_y,frame->x+frame->cursor_x);
	pthread_mutex_unlock(&lock);
}
int fprint(struct i_frame* myFrame,char *s,...){
	pthread_mutex_lock(&lock);
	int temp=0,k=0,flag=0;
	va_list list;
	va_start(list,s);
	int size=vsprintf(myFrame->t_array,s,list);
	//vprintf(s,list);
	va_end(list);
	myFrame->t_array=myFrame->t_array+size;//MOVING THE BUFFER POINTER to apropriate pos after the current fprint();

	//sprintf(myFrame->f_array,"adfjkhdsjkfhdsjkkhs");
	//PRINTING THE BUFFER ARRAY..
	for(int i=0;i< myFrame->d;i++){
		printf("%c[%d;%df",0x1B,myFrame->y+i+temp,myFrame->x);//Navigate the cursor to (x,y) fro ith row
		for(int j=0;j< myFrame->l;j++){//printing individual row of the buffer.
			if(*(myFrame->f_array+i*myFrame->l+j)=='\0'){//Till the buffer contains null
				myFrame->cursor_x=j;
				myFrame->cursor_y=i;
				size=i*myFrame->l+j;
				flag=1;
				break;
			}
			if(*(myFrame->f_array+i*myFrame->l+j)=='\n'){
				//myFrame->l*myFrame->d-(i*myFrame->l+j-1)
				memmove(myFrame->f_array+(i+1)*myFrame->l,myFrame->f_array+i*myFrame->l+j+1,myFrame->l*myFrame->d-(i+1)*myFrame->l);
				memset(myFrame->f_array+i*myFrame->l+j,(int)' ',myFrame->l-j);
				myFrame->t_array=myFrame->t_array+myFrame->l-j-1;
				continue;
			}
			/*	temp++;
				for(k=i*myFrame->l;(k< myFrame->l*myFrame->d-j-1)&&*(myFrame->f_array+k)!='\0';k++)
					*(myFrame->f_array+k)=*(myFrame->f_array+k+j+1);
				*(myFrame->f_array+k)='\0';
				i--;
				break;
			}*/
			printf("%c",*(myFrame->f_array+i*myFrame->l+j));
		}
		if(flag==1)
			break;
	}
	if(flag==0){
		size=myFrame->l*myFrame->d;
		myFrame->cursor_x=0;
		myFrame->cursor_y=0;
	}
	flag=0;
	//memset(myFrame->f_array,0,myFrame->l*myFrame->d);
	//printf("%c[%d;%df",0x1B,myFrame->y+myFrame->d,myFrame->x+myFrame->l);
	pthread_mutex_unlock(&lock);
	return size;
}

void seek_cur(int y,int x){
	pthread_mutex_lock(&lock);
	printf("%c[%d;%df",0x1B,y,x);
	pthread_mutex_unlock(&lock);
}
void clear(struct i_frame* frame){
	memset(frame->f_array,(int)' ',frame->l*frame->d);
	fprint(frame,"",0);
	seek_cur(frame->y,frame->x);
	frame->t_array=frame->f_array;
	frame->cursor_x=0;
	frame->cursor_y=0;
}

int fscan(struct i_frame* frame,char* s,...){
	seek_cur(frame->y+frame->cursor_y,frame->x+frame->cursor_x);
	va_list list;
	va_start(list,s);
	int size=vscanf(s,list);
	frame->cursor_y+=1;//to display next character in next line on the terminal
	frame->cursor_x=0;
	/*
		adding the entered string in the frame buffer so that it is also printable inside the frame
	*/
	void* temp;
	switch(s[1]){
	case 'd':	temp=va_arg(list,int*);
			frame->t_array+=sprintf(frame->t_array,s,*(int*)temp);
			break;
	case 's':	temp=va_arg(list,char*);
			frame->t_array+=sprintf(frame->t_array,s,(char*)temp);
			break;
	case 'f':	temp=va_arg(list,float*);
			frame->t_array+=sprintf(frame->t_array,s,*(float*)temp);
			break;
	case 'c':	temp=va_arg(list,char*);
			frame->t_array+=sprintf(frame->t_array,s,*(char*)temp);
			break;
	case '%':	if(s[2]=='c'){
				temp=va_arg(list,char*);
				frame->t_array+=sprintf(frame->t_array,s+1,*(char*)temp);
				break;
			}
	default :	;
	}
	va_end(list);
	seek_cur(frame->cursor_y+frame->y,frame->x+frame->cursor_x);//seeking the cursor to above values on the terminal
	/*printing next line in buffer so that fprint prints on appropriate position i.e in the beginning of the next line*/ 
	*(frame->t_array)='\n';
	frame->t_array+=1;
	
	return size;

}

/*
int main(){
	init();
	struct i_frame* Frame=makeframe(30,30,10,10);
	border(Frame,'.','.',VISIBLE);
	int x;
	fprint(Frame,"ENTER AGE...\n");
	fscan(Frame,"%d",&x);
	fprint(Frame,"YOUR AGE IS %d\n",x);
	delframe(Frame);
	printf("\n");
	return 0;

}*/

