#include<pthread.h>
int q_main, flag_main;
pthread_mutex_t mtx_main;
pthread_mutex_t cond_main;
 
int q_red, flag_red;
pthread_mutex_t mtx_red;
pthread_mutex_t cond_red;
 
int q_blue, flag_blue;
pthread_mutex_t mtx_blue;
pthread_mutex_t cond_blue;
 
int q_mtx, flag_mtx;
pthread_mutex_t mtx_mtx;
pthread_mutex_t cond_mtx;
 

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 10

int i_blue, i_red;
int un_count_b, un_count_r;
int count_b, count_r;
int check_blue,check_red;

void *red(void *num_r);
void *blue(void *num_b);

int main(int argc, char *argv[]){
	char c;
	int i;
	int check;
	int *array_r, *array_b;
	pthread_t *p_red, *p_blue;
	
	q_main=0;
flag_main=0;
pthread_mutex_init(&mtx_main, NULL);
pthread_mutex_init(&cond_main, NULL);
pthread_mutex_lock(&cond_main);

	
	q_red=0;
flag_red=0;
pthread_mutex_init(&mtx_red, NULL);
pthread_mutex_init(&cond_red, NULL);
pthread_mutex_lock(&cond_red);

	
	q_blue=0;
flag_blue=0;
pthread_mutex_init(&mtx_blue, NULL);
pthread_mutex_init(&cond_blue, NULL);
pthread_mutex_lock(&cond_blue);

	
	q_mtx=0;
flag_mtx=0;
pthread_mutex_init(&mtx_mtx, NULL);
pthread_mutex_init(&cond_mtx, NULL);
pthread_mutex_lock(&cond_mtx);

	
	while(1) {
		i_blue = 0;
		i_red = 0;
		count_b = 0;
		count_r = 0;
		un_count_b = 0;
		un_count_r = 0;
		check_blue = 0;
		check_red = 0;
		
		printf("Press 0 to insert a red car, 1 for blue car or 'q' to exit\n");
		do{
			c = getchar();
			if(c=='0'){
				//red
				i_red++;
			}
			else if(c=='1'){
				//blue
				i_blue++;
			}
			else if(c=='q') {
				return(0);
			}
		}while(c!='\n');
		
		if(i_red==0 && i_blue==0){
			return(0);
		}
		printf("Α new day dawned\n");
		
		p_red = (pthread_t*)malloc(sizeof(pthread_t)*i_red);
		if(p_red==NULL){
			printf("Problem with memory allocation\n");
			return(2);
		}
		p_blue = (pthread_t*)malloc(sizeof(pthread_t)*i_blue);
		if(p_blue==NULL){
			printf("Problem with memory allocation\n");
			return(2);
		}
		array_r = (int*)malloc(sizeof(int)*i_red);
		if(array_r==NULL){
			printf("Problem with memory allocation\n");
			return(2);
		}
		array_b = (int*)malloc(sizeof(int)*i_blue);
		if(array_b==NULL){
			printf("Problem with memory allocation\n");
			return(2);
		}
		
		printf("red %d, blue %d\n",i_red, i_blue);
		
		for(i=0; i<i_red; i++){
			array_r[i] = i + 1;
			check = pthread_create(&p_red[i], NULL, &red, &array_r[i]);
			if(check!=0) {
				printf("Problem to create %d thread\n", i);
				return(7);
			}
		}
		
		for(i=0; i<i_blue; i++){
			array_b[i] = i + 1;
			check = pthread_create(&p_blue[i], NULL, &blue, &array_b[i]);
			if(check!=0) {
				printf("Problem to create %d thread\n", i);
				return(7);
			}
		}
		
		pthread_mutex_lock(&mtx_main);

		
		if(flag_main>0){
	flag_main--;
	q_main--;
	pthread_mutex_unlock(&cond_main);
}

q_main++;
pthread_mutex_unlock(&mtx_main);
pthread_mutex_lock(&cond_main); 

		
		if(flag_main>0){
	flag_main--;
	q_main--;
	pthread_mutex_unlock(&cond_main);
}
else{
	pthread_mutex_unlock(&mtx_main);
} 

		
		printf("End of the day\n");
	}
	
	return(0);
}

void *red(void *num_r){
	int number;
	int i,local_r;
	number = *(int *)(num_r);
	
	pthread_mutex_lock(&mtx_red);

	
	if(i_blue==0){
		check_red++;
		if(check_red == i_red) {
			if(q_red>0) {
	flag_red ++;
}

		}
		if(i_red!=1){
			if(flag_red>0){
	flag_red--;
	q_red--;
	pthread_mutex_unlock(&cond_red);
}

q_red++;
pthread_mutex_unlock(&mtx_red);
pthread_mutex_lock(&cond_red); 

		}
	}
	else{
		if(flag_red>0){
	flag_red--;
	q_red--;
	pthread_mutex_unlock(&cond_red);
}

q_red++;
pthread_mutex_unlock(&mtx_red);
pthread_mutex_lock(&cond_red); 

	}
	if(count_r%N==0){
		count_r++;
		for(i=1; i<N && i<=(i_red-count_r); i++) {
			if(q_red>0) {
	flag_red ++;
}

		}
	}
	else {
		count_r++;
	}
	
	if(flag_red>0){
	flag_red--;
	q_red--;
	pthread_mutex_unlock(&cond_red);
}
else{
	pthread_mutex_unlock(&mtx_red);
} 

	
	printf("red %d\n",number);
	
	pthread_mutex_lock(&mtx_mtx);

	
	un_count_r++;
	local_r = un_count_r;
	
	if(flag_mtx>0){
	flag_mtx--;
	q_mtx--;
	pthread_mutex_unlock(&cond_mtx);
}
else{
	pthread_mutex_unlock(&mtx_mtx);
} 

	
	if(local_r%N == 0 || i_red == local_r) {
		printf("count_red %d\n",local_r);
		if(i_blue != un_count_b) {
			pthread_mutex_lock(&mtx_blue);

			
			if(q_blue>0) {
	flag_blue ++;
}

			
			if(flag_blue>0){
	flag_blue--;
	q_blue--;
	pthread_mutex_unlock(&cond_blue);
}
else{
	pthread_mutex_unlock(&mtx_blue);
} 

		}
		else {
			if(i_red == local_r) {
				pthread_mutex_lock(&mtx_main);

				
				if(q_main>0) {
	flag_main ++;
}

				
				if(flag_main>0){
	flag_main--;
	q_main--;
	pthread_mutex_unlock(&cond_main);
}
else{
	pthread_mutex_unlock(&mtx_main);
} 

			}
			else {
				pthread_mutex_lock(&mtx_red);

				
				if(q_red>0) {
	flag_red ++;
}

				
				if(flag_red>0){
	flag_red--;
	q_red--;
	pthread_mutex_unlock(&cond_red);
}
else{
	pthread_mutex_unlock(&mtx_red);
} 

			}
		}
	}
	
	return(NULL);
}

void *blue(void *num_b){
	int number;
	int i,local_b;
	number = *(int *)(num_b);
	
	pthread_mutex_lock(&mtx_blue);

	
	check_blue++;
	if(check_blue == i_blue) {
		if(q_blue>0) {
	flag_blue ++;
}

	}
	if(i_blue!=1){
		if(flag_blue>0){
	flag_blue--;
	q_blue--;
	pthread_mutex_unlock(&cond_blue);
}

q_blue++;
pthread_mutex_unlock(&mtx_blue);
pthread_mutex_lock(&cond_blue); 

	}
	if(count_b%N==0){
		count_b++;
		for(i=1; i<N && i<=(i_blue-count_b); i++) {
			if(q_blue>0) {
	flag_blue ++;
}

		}
	}
	else {
		count_b++;
	}
	
	if(flag_blue>0){
	flag_blue--;
	q_blue--;
	pthread_mutex_unlock(&cond_blue);
}
else{
	pthread_mutex_unlock(&mtx_blue);
} 

	
	printf("blue %d\n",number);
	
	pthread_mutex_lock(&mtx_mtx);

	
	un_count_b++;
	local_b = un_count_b;
	
	if(flag_mtx>0){
	flag_mtx--;
	q_mtx--;
	pthread_mutex_unlock(&cond_mtx);
}
else{
	pthread_mutex_unlock(&mtx_mtx);
} 

	
	if(local_b%N == 0 || i_blue == local_b) {
		printf("count_blue %d\n",local_b);
		if(i_red != un_count_r) {
			pthread_mutex_lock(&mtx_red);

			
			if(q_red>0) {
	flag_red ++;
}

			
			if(flag_red>0){
	flag_red--;
	q_red--;
	pthread_mutex_unlock(&cond_red);
}
else{
	pthread_mutex_unlock(&mtx_red);
} 

		}
		else {
			if(i_blue == local_b) {
				pthread_mutex_lock(&mtx_main);

				
				if(q_main>0) {
	flag_main ++;
}

				
				if(flag_main>0){
	flag_main--;
	q_main--;
	pthread_mutex_unlock(&cond_main);
}
else{
	pthread_mutex_unlock(&mtx_main);
} 

			}
			else {
				pthread_mutex_lock(&mtx_blue);

				
				if(q_blue>0) {
	flag_blue ++;
}

				
				if(flag_blue>0){
	flag_blue--;
	q_blue--;
	pthread_mutex_unlock(&cond_blue);
}
else{
	pthread_mutex_unlock(&mtx_blue);
} 

			}
		}
	}
	return(NULL);
}
