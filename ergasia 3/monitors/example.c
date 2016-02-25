#include <stdio.h>
#include <pthread.h>

pthread_mutex_t p;

int main( int argc, char* argv[]){
	int p;
	
	pthread_mutex_init(&p,NULL);
	
	scanf("%d",&p);
	printf("%d",p);
	
	return(0);
}