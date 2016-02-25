#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 10
//elegxoys malloc init mutex
//na kanoume destroy


int un_count_b=0,un_count_r=0, i_blue, i_red;         // metraei ton arithmo twn amaksiwn kathe xrwmatos pou boroun na perasoun ap thn gefyra

pthread_mutex_t m_ex_blue, m_ex_red;  //dwsimo seiras ap t main sta threads
pthread_mutex_t red5;                //enallagh seiras blue-red
pthread_mutex_t mtx;                //gia amoivaio apokleismo sta threads
pthread_mutex_t safe_mtx;          //gia na mhn paei strafh h unlock

void *red(void *num_r);
void *blue(void *num_b);

int main(int argc, char *argv[]){
	char c;
	int i, *array_r, *array_b;
	int count_b, count_r;
	pthread_t *p_red, *p_blue;
	
	i_blue = 0;
	i_red = 0;
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
		
	}while(c!='\n');
	
	if(i_red==0 && i_blue==0){
		return(0);
	}
	
	p_red = (pthread_t*)malloc(sizeof(pthread_t)*i_red);
	p_blue = (pthread_t*)malloc(sizeof(pthread_t)*i_blue);
// 	m_ex_red = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*i_red);  //*******************************
// 	m_ex_blue = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*i_blue); //************************
	array_r = (int*)malloc(sizeof(int)*i_red);
	array_b = (int*)malloc(sizeof(int)*i_blue);
	
	pthread_mutex_init(&m_ex_red, NULL);   //***************************
	pthread_mutex_lock(&m_ex_red);   //**************************
	for(i=0; i<i_red; i++){
		array_r[i] = i;
		pthread_create(&p_red[i], NULL, &red, &array_r[i]);
	}
	
	pthread_mutex_init(&m_ex_blue, NULL);   //***********************
	pthread_mutex_lock(&m_ex_blue);  //*********************
	for(i=0; i<i_blue; i++){
		array_b[i] = i;
		pthread_create(&p_blue[i], NULL, &blue, &array_b[i]);
	}
	
	count_r = 0;
	count_b = 0;
	
	pthread_mutex_init(&red5, NULL);
	pthread_mutex_init(&mtx, NULL);
	pthread_mutex_init(&safe_mtx, NULL);
	
	printf("red %d, blue %d\n",i_red, i_blue);
	pthread_mutex_lock(&safe_mtx);
	while(1) {
		pthread_mutex_lock(&red5);
		if(i_red == count_r) {
			pthread_mutex_unlock(&red5);  //otan perasoun ola red dwse prosvash sta blue
		}
		for(i=count_r; i<(count_r+N) && i<i_red; i++ ) {
			pthread_mutex_unlock(&m_ex_red);   //*********************
			pthread_mutex_lock(&safe_mtx);
			
		}
		count_r = i;
		
		pthread_mutex_lock(&red5);
		if(i_blue == count_b) {
			pthread_mutex_unlock(&red5); //otan perasoun ola blue dwse prosvash sta red
		}
		for(i=count_b; i<(count_b+N) && i<i_blue; i++ ) {
			pthread_mutex_unlock(&m_ex_blue);  //******************
			pthread_mutex_lock(&safe_mtx);
		}
		count_b = i;
		
		pthread_mutex_lock(&red5);
		if(count_r == i_red && count_b == i_blue){
			break;
		}
		pthread_mutex_unlock(&red5);
	}
	
	
	return(0);
}

void *red(void *num_r){   //****************
	int number;
	number = *(int *)(num_r); //***********
	
	pthread_mutex_lock(&m_ex_red);  //***************
	
	pthread_mutex_unlock(&safe_mtx);
	
	pthread_mutex_lock(&mtx);
	un_count_r++;
	printf("red %d\n", number);
	if(un_count_r%N == 0 || un_count_r == i_red){
		printf("count_red %d\n", un_count_r);
// 		count = 0;
		pthread_mutex_unlock(&red5);
	}
	pthread_mutex_unlock(&mtx);
	
	return(NULL);
}

void *blue(void *num_b){  //****************
	int number;
	number = *(int *)(num_b);
	
	pthread_mutex_lock(&m_ex_blue);  //*************
	
	pthread_mutex_unlock(&safe_mtx);
	
	pthread_mutex_lock(&mtx);
	un_count_b++;
	printf("blue %d\n", number);
	if(un_count_b%N==0 || i_blue == un_count_b){
		printf("count_blue %d\n", un_count_b);
// 		count = 0;
		pthread_mutex_unlock(&red5);
	}
	pthread_mutex_unlock(&mtx);
	
	return(NULL);
}