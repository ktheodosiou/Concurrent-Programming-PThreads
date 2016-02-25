#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

#define N 100

int in1=0;
// int in2=0;
// int out1=0;
// int out2=0;
// 
// int end_flag1=0;
// int end_flag2=0;
// 
// char array1[N];
// char array2[N];

void*  p1_main(void *arg){
	int* k=3;
	printf("hello from thread1 %d\n", k);
	pthread_exit((void*)k);
}

int main(int argc, char *argv[]){
	int check, i;
	pthread_t p1,p2,p3;
	void *status;
	

	
	check = pthread_create(&p1,NULL,&p1_main,NULL);
	if(check!=0){
		//yparxei thema
		return(1);
	}
	
	pthread_join(p1,(void**)&i);
	
	sleep(3);
	
	printf("Hello from main	%d	\n", i);
	return(0);
}








