#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define N 10

//elegxos toy unlock klp
//elegxos toy unlock ride gia thn periptosh opoy ginoyn dyo synexomena
pthread_mutex_t ride;    //energopoiei kathe epivath gia na arxisei th volta tou
pthread_mutex_t mtx_pass;  //to mtx_pass einai upeuthuno gia na pei sth main oti o epivaths epivivasthke
pthread_mutex_t roller;  // energopoiei to trenaki sthn arxh kathe voltas
pthread_mutex_t mtx_rc;   //eidopoiei th main oti to treno exei teleiwsei thn koursa
pthread_mutex_t msg_pass; //gia ton amoivaio apokleismo metaksu twn passenger
pthread_mutex_t mtx;    //gia ton amoivaio apokleismo metaksu ths main kai tou passenger
pthread_mutex_t msg_rc;  //dhlwnei o teleutaios epivaths kathe koursas sto roller oti epivivasthke

int count=0;     //metrhths epivatwn 

void *passenger();
void *roller_coaster();
int error_check(int check, int func);

int main(int argc, char *argv[]) {
	int number, wait_pas,i,k=0; 
	int check;
	char command[10];  //gia tis edoles tou xrhsth
	pthread_t p_rc, *p_pass;
	
	wait_pas = 0; //oi epivates pou menoun meta thn kathe epivivash
	
	/*orismos mutexes*/
	check = pthread_mutex_init(&ride,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&roller,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&mtx_rc,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&mtx_pass,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&msg_pass,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&msg_rc,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&mtx,NULL);
	error_check(check,1);
	
	/*arxikopoihsh mutexes*/
	check = pthread_mutex_lock(&roller);
	error_check(check,2);
	check = pthread_mutex_lock(&ride);
	error_check(check,2);
	check = pthread_mutex_lock(&mtx_rc);
	error_check(check,2);
	check = pthread_mutex_lock(&mtx_pass);
	error_check(check,2);
	check = pthread_mutex_lock(&msg_rc);
	error_check(check,2);
	check = pthread_mutex_lock(&mtx);
	error_check(check,2);
	
	/*dhmiourgia roller_coaster/nhma*/
	check = pthread_create(&p_rc, NULL, &roller_coaster, NULL);
	if(check!=0) {
		printf("Problem to create roller_coaster thread\n");
		return(7);
    }
    /*gia epanalamvaomenh eisagwgh stoixeiwn apo to xrhsth*/
	while(1){

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
				k++; //o arithmos tou kathe thread/epivath
				check = pthread_create(&p_pass[i], NULL, &passenger, &k);
				if(check!=0) {
					printf("Problem to create %d thread\n", i);
					free(p_pass);
					return(7);
				}
				/*blockarei gia na exei kathe thread/epivaths diaforetiko k*/
				check = pthread_mutex_lock(&mtx);
				error_check(check,2);
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
		/*epanalhyh koursas*/
		while(wait_pas>=N){
			/*kseblockarei to trenaki gia na boroun na epivivastoun epivates*/
			check = pthread_mutex_unlock(&roller);
			error_check(check,3);	

			/*eisagwgh N epivatwn sto roller*/
			for(i=0;i<N;i++){
				check = pthread_mutex_unlock(&ride);
				error_check(check,3);
				check = pthread_mutex_lock(&mtx_pass);
				error_check(check,2);
			}
			/*perimenei na teleiwsei h koursa*/
			check = pthread_mutex_lock(&mtx_rc);
			error_check(check,2);
			
			wait_pas = wait_pas - N;    //afairoume tous epivates pou exoun kanei hdh thn koursa tous
			
		}
	}
	
	
	return (0);
}

void *roller_coaster(){
	int check;
	while(1){
		/*perimenoume shma apo th main oti to trenaki einai etoimo gia epivivash*/
		check = pthread_mutex_lock(&roller);
		error_check(check,2);
		/*gia na ksekinhsei to treno meta thn epivivash*/
		check = pthread_mutex_lock(&msg_rc);
		error_check(check,2);
		sleep(1);
		printf("The roller coaster begin!\n");
		sleep(2);
		printf("Passengers abord the roller_coaster!\n");
		sleep(1);
		printf("The roller coaster is ready for new ride!\n");
		sleep(1);
		/*dinei shma sth main oti teleiwse h koursa*/
		check = pthread_mutex_unlock(&mtx_rc);
		error_check(check,3);
	}
	return (NULL);
}

void *passenger(void *num_p){
	int check,k;
	k = *(int *)(num_p);
	check = pthread_mutex_unlock(&mtx);
	error_check(check,3);
	/*perimenei o epivaths sth seira gia th volta*/
	check = pthread_mutex_lock(&ride);
	error_check(check,2);
	check = pthread_mutex_unlock(&mtx_pass);
	error_check(check,3);
	
	/*amoivaios apokleismos metaksu twn epivatwn*/
	check = pthread_mutex_lock(&msg_pass);
	error_check(check,2);
	count++;  //o arithmos tou epivath pou exei anevei sth koursa
	printf("Passenger %d boarded!\n", k);
	/*elegxos gia na stamathsei h epivivash an exei gemisei to trenaki*/
	if(count%N==0){
		check = pthread_mutex_unlock(&msg_rc);
		error_check(check,3);
	}
	check = pthread_mutex_unlock(&msg_pass);
	error_check(check,3);
	return (NULL);
}

int error_check(int check, int func){
    if (check!=0){
        if(func==1){
            printf("Problem with pthread_mutex_init\n");
        }
        else if (func==2){
            printf("Problem with pthread_mutex_lock\n");
        }
        else if (func==3){
            printf("Problem with pthread_mutex_unlock\n");
        }
        else{
            printf("Problem with pthread_mutex_destroy\n");
        }
        exit(1);
    }
    else{
        return(0);
    }
}