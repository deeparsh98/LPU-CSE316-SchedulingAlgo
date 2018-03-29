#ifndef __viewable_h__
#define __viewable_h__
#define clearscr() printf("\033[H\033[J")
#define VISIBLE 1
#define NO_VISIBLE 0
//Structure of the frame
struct i_frame{
	int x,y,l,d;//(x,y)coordinates, length and width respectively
	char* f_array;//Buffer
	char* t_array;//pointer to the current cursor in the buffer
	int cursor_x;//x ordinate of the  cursor
	int cursor_y;//y ordinate of the cursor
	pthread_mutex_t lock;
};

/* A function that returns the pointer to the frame built according to the passed arguments
	l = length b = breadth x = x-ordinate y = y-ordinate
*/
void seek_cur(int x,int y);

struct i_frame* makeframe(int l,int b,int x,int y);

/*Function to release the memory occupied by a frame*/
void delframe(struct i_frame* frame);

/*Function for formatted output inside the frame on the terminal, takes the pointer to a frame,and the formatted var args*/
int fprint(struct i_frame* frame,char* s,...);

/*function to clear the specified frame*/
void clear(struct i_frame* frame);

/*Function to spawn the border of the frame*/
void border(struct i_frame* frame,char top,char side,int visibility);

/*Function to take formatted input from user through the specified frame.*/
int fscan(struct i_frame* frame,char* s,...);

void init();
#endif


