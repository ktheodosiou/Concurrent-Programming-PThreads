#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define N 10

int q_main,flag_main;
pthread_mutex_t mtx_main;
pthread_mutex_t cond_main;

int q_roll, flag_roll;
pthread_mutex_t mtx_roll;
pthread_mutex_t cond_roll;

int q_pass, flag_pass;
pthread_mutex_t mtx_pass;
pthread_mutex_t cond_pass;

int wait_pas = 0;

void *passenger();
void *roller_coaster();

int main(int argc, char *argv[]) {
	int check, k, i;
	int number;
	char command[10];  //gia tis edoles tou xrhsth
	pthread_t p_rc, *p_pass;
	
	k = 0;
	/******************************synch**************************************************/
	q_main = 0; 
	flag_main = 0;
	pthread_mutex_init(&mtx_main,NULL);
	pthread_mutex_init(&cond_main, NULL);
	pthread_mutex_lock(&cond_main);
	
	q_roll = 0;
	flag_roll = 0;
	pthread_mutex_init(&mtx_roll,NULL);
	pthread_mutex_init(&cond_roll, NULL);
	pthread_mutex_lock(&cond_roll);
	
	q_pass = 0;
	flag_pass = 0;
	pthread_mutex_init(&mtx_pass,NULL);
	pthread_mutex_init(&cond_pass, NULL);
	pthread_mutex_lock(&cond_pass);
	
	check = pthread_create(&p_rc, NULL, &roller_coaster, NULL);
	if(check!=0) {
		printf("Problem to create roller_coaster thread\n");
		
		return(7);
    }
    
    while(1) {
		//zhtame dedomena apo xrhsth
		printf("For new passengers press 'new' and the number of passengers\n");
		printf("Else for exit press 'exit'\n");
		printf("wait_pas: %d\n",wait_pas);
		scanf(" %9s",command);
		
		if(strcmp(command,"new")==0){
			scanf("%d",&number);
			wait_pas = wait_pas + number;    //sunolikos arithmos twn epivatwn meta thn eisagwgh twn kainouriwn 
			
			/*desmeush epivatwn*/
			p_pass = (pthread_t *)malloc(sizeof(pthread_t)*number);
			if(p_pass==NULL){                                        
				printf("Problem with memory allocation\n");

				return(2);
			}
			/*Dhmiourgia toswn thread osoi einai k oi epivates*/
			for(i=0;i<number;i++){
				
				/*blockarei gia na exei kathe thread/epivaths diaforetiko k*/
				/******************************synch_begin(main)**************************************************/
				pthread_mutex_lock(&mtx_main);
// 				check = pthread_mutex_lock(&mtx_pass);
				k++; //o arithmos tou kathe thread/epivath
				
				check = pthread_create(&p_pass[i], NULL, &passenger, &k);
				if(check!=0) {
					printf("Problem to create %d thread\n", i);
					free(p_pass);
					
					return(7);
				}
				/******************************synch_wait(main)**************************************************/
	
				if(flag_main>0){
					flag_main--;
					q_main--;
					pthread_mutex_unlock(&cond_main);
				}
				
				q_main++;
				pthread_mutex_unlock(&mtx_main);
				pthread_mutex_lock(&cond_main);
				/******************************synch_end(main)**************************************************/
				if(flag_main>0){
					flag_main--;
					q_main--;
					pthread_mutex_unlock(&cond_main);
				}
				else{
					pthread_mutex_unlock(&mtx_main);
				}
			}
			free(p_pass);
		}
		else if(strcmp(command,"exit")==0){
			printf("Roller coaster has closed for today!\n"); //FREE THREADS
			break;
		}
		else{ 
			printf("Try again!\n");
			continue;
		}
		printf("wait_pas: %d\n",wait_pas);
		/******************************synch_begin(roller)**************************************************/
		pthread_mutex_lock(&mtx_roll);
// 		pthread_mutex_lock(&mtx_roll);
		/******************************synch_notify(roller)**************************************************/
		if(q_roll>0) {
			flag_roll ++;
		}
// 		pthread_cond_signal(&cond_roll);
		/******************************synch_end(roller)**************************************************/
		if(flag_roll>0){
			flag_roll--;
			q_roll--;
			pthread_mutex_unlock(&cond_roll);
		}
		else{
			pthread_mutex_unlock(&mtx_roll);
		}
// 		pthread_mutex_unlock(&mtx_roll);
		
		
		/******************************synch_begin(main)**************************************************/
		pthread_mutex_lock(&mtx_main);
// 		pthread_mutex_lock(&mtx_main);
		/******************************synch_wait(main)**************************************************/
	
		if(flag_main>0){
			flag_main--;
			q_main--;
			pthread_mutex_unlock(&cond_main);
		}
		
		q_main++;
		pthread_mutex_unlock(&mtx_main);
		pthread_mutex_lock(&cond_main);
// 		pthread_cond_wait(&cond_main, &mtx_main);
		/******************************synch_end(main)**************************************************/
		if(flag_main>0){
			flag_main--;
			q_main--;
			pthread_mutex_unlock(&cond_main);
		}
		else{
			pthread_mutex_unlock(&mtx_main);
		}
// 		pthread_mutex_unlock(&mtx_main);
	}
    
    return (0);
}

void *roller_coaster(){
	int i;
	int count;
	
	count = 0;
	while(1) {
		//perimenei na parei tous epibates apo th main
	/******************************synch_begin(roller)**************************************************/
		pthread_mutex_lock(&mtx_roll);
		/******************************synch_wait(roller)**************************************************/
	
		if(flag_roll>0){
			flag_roll--;
			q_roll--;
			pthread_mutex_unlock(&cond_roll);
		}
		
		q_roll++;
		pthread_mutex_unlock(&mtx_roll);
		pthread_mutex_lock(&cond_roll);
		
// 		pthread_cond_wait(&cond_roll, &mtx_roll);
		/******************************synch_end(roller)**************************************************/
		if(flag_roll>0){
			flag_roll--;
			q_roll--;
			pthread_mutex_unlock(&cond_roll);
		}
		else{
			pthread_mutex_unlock(&mtx_roll);
		}
// 		pthread_mutex_unlock(&mtx_roll);
		
		
		while(wait_pas>=N){
			printf("The roller coaster is ready for new ride!\n");
			for(i=0; i<N; i++) {
				count++;
// 				printf("stelnw shma ston %d\n", count);
				/******************************synch_begin(pass)**************************************************/
				pthread_mutex_lock(&mtx_pass);
				/******************************synch_notify(pass)**************************************************/
				if(q_pass>0) {
					flag_pass ++;
				}
// 				pthread_cond_signal(&cond_pass);
				/******************************synch_end(pass)**************************************************/
				if(flag_pass>0){
					flag_pass--;
					q_pass--;
					pthread_mutex_unlock(&cond_pass);
				}
				else{
					pthread_mutex_unlock(&mtx_pass);
				}
			}
			wait_pas = wait_pas - N;
			sleep(1);
			printf("The roller coaster begin!\n");
			sleep(2);
			printf("Passengers abord the roller_coaster!\n");
			sleep(1);
		}
		/******************************synch_begin(main)**************************************************/
		pthread_mutex_lock(&mtx_main);
		/******************************synch_notify(main)**************************************************/
		if(q_main>0) {
			flag_main ++;
		}
// 		pthread_cond_signal(&cond_main);
		/******************************synch_end(main)**************************************************/
		if(flag_main>0){
			flag_main--;
			q_main--;
			pthread_mutex_unlock(&cond_main);
		}
		else{
			pthread_mutex_unlock(&mtx_main);
		}
	}
	
	return (NULL);
}


void *passenger(void *num_p){
	int k;
	
	//wait na me ksupnhsei to trenaki
	/******************************synch_begin(main)**************************************************/
	pthread_mutex_lock(&mtx_main);
	/*************krisimos kwdikas**********/
	k = *(int *)(num_p);
	/******************************synch_notify(main)**************************************************/
	if(q_main>0) {
			flag_main ++;
	}
	/******************************synch_end(main)**************************************************/
	if(flag_main>0){
		flag_main--;
		q_main--;
		pthread_mutex_unlock(&cond_main);
	}
	else{
		pthread_mutex_unlock(&mtx_main);
	}
	
	/******************************synch_begin(pass)**************************************************/
	pthread_mutex_lock(&mtx_pass);
	
	/******************************synch_wait(pass)**************************************************/
	
	if(flag_pass>0){
		flag_pass--;
		q_pass--;
		pthread_mutex_unlock(&cond_pass);
	}
	
	q_pass++;
	pthread_mutex_unlock(&mtx_pass);
	pthread_mutex_lock(&cond_pass);
	/******************************synch_end(pass)**************************************************/
	if(flag_pass>0){
		flag_pass--;
		q_pass--;
		pthread_mutex_unlock(&cond_pass);
	}
	else{
		pthread_mutex_unlock(&mtx_pass);
	}
	
// 	pthread_mutex_unlock(&mtx_pass);
	printf("i am passenger %d\n", k);
	
	return (NULL);
}