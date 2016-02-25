#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define N 10

//elegxos toy unlock klp
//elegxos toy unlock ride gia thn periptosh opoy ginoyn dyo synexomena
pthread_mutex_t ride,roller;
pthread_mutex_t mtx_pass,mtx_rc;
pthread_mutex_t msg_pass,msg_rc;

void *passenger();
void *roller_coaster();
int count=0;

int main(int argc, char *argv[]) {
	int number, wait_pas,i; 
	char command[10];
	pthread_t p_rc, *p_pass;
	
	wait_pas = 0; //oi epivates pou menoun meta thn epivivash
	pthread_mutex_init(&ride,NULL);
	pthread_mutex_init(&roller,NULL);
	pthread_mutex_init(&mtx_rc,NULL);
	pthread_mutex_init(&mtx_pass,NULL);
	pthread_mutex_init(&msg_pass,NULL);
	pthread_mutex_init(&msg_rc,NULL);
	
	pthread_mutex_lock(&roller);
	pthread_mutex_lock(&ride);
	pthread_mutex_lock(&mtx_rc);
	pthread_mutex_lock(&mtx_pass);
	pthread_mutex_lock(&msg_rc);
	
	pthread_create(&p_rc, NULL, &roller_coaster, NULL);
	
	while(1){

		printf("For new passengers press 'new' and the number of passengers\n");
		printf("Else for exit press 'exit'\n");
		scanf(" %9s",command);
		if(strcmp(command,"new")==0){
			scanf("%d",&number);
			wait_pas = wait_pas + number;
			
			p_pass = (pthread_t *)malloc(sizeof(pthread_t)*number);
			for(i=0;i<number;i++){
				pthread_create(&p_pass[i], NULL, &passenger, NULL);
			}
			free(p_pass);
		}
		else if(strcmp(command,"exit")==0){
			printf("Roller coaster has closed for today!\n"); //FREE THREADS
			break;
		}
		else{ 
			printf("Try again asshole!!!\n");
			continue;
		}

		while(wait_pas>=N){
			pthread_mutex_unlock(&roller);
			pthread_mutex_lock(&mtx_rc);
			for(i=0;i<N;i++){
				pthread_mutex_unlock(&ride);
				pthread_mutex_lock(&mtx_pass);
			}
			pthread_mutex_lock(&mtx_rc);
			wait_pas = wait_pas - N;
			
// 			printf("\n%d\n",count);
		}
		
		
	}
	
	return (0);
}

void *roller_coaster(){
	while(1){
		pthread_mutex_lock(&roller);
		sleep(3);
		printf("The roller coaster begin!\n");
		pthread_mutex_unlock(&mtx_rc);
		pthread_mutex_lock(&msg_rc);
		printf("Passengers abord the roller_coaster!\n");
		sleep(1);
		printf("The roller coaster is ready for new ride!\n");
		pthread_mutex_unlock(&mtx_rc);
	}
	return (NULL);
}

void *passenger(){
	
	pthread_mutex_lock(&ride);
	pthread_mutex_unlock(&mtx_pass);
	
	pthread_mutex_lock(&msg_pass);
	count++;
	printf("passenger : %d \n", count);
	if(count==N){pthread_mutex_unlock(&msg_rc);}
	pthread_mutex_unlock(&msg_pass);
	return (NULL);
}