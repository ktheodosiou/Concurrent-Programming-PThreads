#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 10

int i_blue, i_red;          //o arithmos twn amaksiwn pou dinei kathe fora o xrhsths  
int un_count_b, un_count_r;
int count_b, count_r;
int check_blue,check_red;
	int flag_blue, flag_red, flag_main;
	int q_main, q_blue, q_red; 

	
pthread_mutex_t mtx;     //gia amoivaio apokleismo
pthread_mutex_t m_blue, m_red;  
pthread_mutex_t m_main;
pthread_mutex_t cond_blue, cond_red;
pthread_mutex_t cond_main;     //perimenei h main n kanei thn epomenh epanalhyh

void *red(void *num_r);
void *blue(void *num_b);

int main(int argc, char *argv[]){
	char c;
	int i;
	int check;
	int *array_r, *array_b;  //arithmos amaksiou twn red kai blue
	pthread_t *p_red, *p_blue;
	
// 	pthread_mutex_init(&m_ex_red,NULL);
// 	pthread_mutex_init(&m_ex_blue,NULL);
// 	pthread_mutex_init(&mtx_main,NULL);
	pthread_mutex_init(&mtx,NULL);
	
	/*******************************synch***********************************************/
	q_main=0;
	q_red=0;
	q_blue=0;
	flag_main=0;
	flag_red=0;
	flag_blue=0;
	pthread_mutex_init(&m_red, NULL);
	pthread_mutex_init(&m_blue, NULL);
	pthread_mutex_init(&m_main, NULL);
	pthread_mutex_init(&cond_main, NULL);
	pthread_mutex_init(&cond_blue, NULL);
	pthread_mutex_init(&cond_red, NULL);
	pthread_mutex_lock(&cond_blue);
	pthread_mutex_lock(&cond_main);
	pthread_mutex_lock(&cond_red);
	/********************************************/
	
// 	pthread_cond_init(&cond_red,NULL);
// 	pthread_cond_init(&cond_blue,NULL);
// 	pthread_cond_init(&cond_main,NULL);
	/*epanalhyh gia na dwsei o xrhsths oses fores thelei autokinhta*/
	while(1) {
		/*ebodizoume ta threads na ksekinhsoun*/
		/*mhdenizoume tous metrhtes gia kathe kainouria mera*/
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
		
		/*elegxos sthn periptosh opou den dhlothei kanena amaksi eksodos apo to programma */
		if(i_red==0 && i_blue==0){
			return(0);
		}
		printf("Α new day dawned\n");
		
		/*anoigoume theseis gia ola ta oxhmata/nhmata*/
		p_red = (pthread_t*)malloc(sizeof(pthread_t)*i_red);
		if(p_red==NULL){										//elegxos epityxias ths malloc
			printf("Problem with memory allocation\n");
			return(2);
		}
		p_blue = (pthread_t*)malloc(sizeof(pthread_t)*i_blue);
		if(p_blue==NULL){										//elegxos epityxias ths malloc
			printf("Problem with memory allocation\n");
			return(2);
		}
		array_r = (int*)malloc(sizeof(int)*i_red);
		if(array_r==NULL){										//elegxos epityxias ths malloc
			printf("Problem with memory allocation\n");
			return(2);
		}
		array_b = (int*)malloc(sizeof(int)*i_blue);
		if(array_b==NULL){										//elegxos epityxias ths malloc
			printf("Problem with memory allocation\n");
			return(2);
		}
		
		printf("red %d, blue %d\n",i_red, i_blue);
		
		/*dhmioyrgoyme ta kokkina oxhmata*/
		for(i=0; i<i_red; i++){
			array_r[i] = i + 1;
			check = pthread_create(&p_red[i], NULL, &red, &array_r[i]);
			if(check!=0) {
				printf("Problem to create %d thread\n", i);
				return(7);
			}
		}
		
		/*dhmioyrgoyme ta mple oxhmata*/
		for(i=0; i<i_blue; i++){
			array_b[i] = i + 1;
			check = pthread_create(&p_blue[i], NULL, &blue, &array_b[i]);
			if(check!=0) {
				printf("Problem to create %d thread\n", i);
				return(7);
			}
		}
		/*******************************synch_begin***********************************************/
		pthread_mutex_lock(&m_main);
// 		pthread_mutex_lock(&mtx_main);
		/*******************************synch_wait***********************************************/
		
		if(flag_main>0){
			flag_main--;
			q_main--;
			pthread_mutex_unlock(&cond_main);
		}
		
		q_main++;
		pthread_mutex_unlock(&m_main);
		
		pthread_mutex_lock(&cond_main);
// 		pthread_cond_wait(&cond_main,&mtx_main);
// 		pthread_mutex_unlock(&mtx_main);
		/*******************************synch_end***********************************************/
		if(flag_main>0){
			flag_main--;
			q_main--;
			pthread_mutex_unlock(&cond_main);
		}
		else{
			pthread_mutex_unlock(&m_main);
		}
			printf("End of the day\n");
		}
	return(0);
	
}

void *red(void *num_r){
	int number;
	int i;
	number = *(int *)(num_r);
	
	/*******************************synch_begin***********************************************/
	pthread_mutex_lock(&m_red);
// 	pthread_mutex_lock(&m_ex_red);
	
	if(i_blue==0){
		check_red++;
		if(check_red == i_red) {
			/*******************************synch_signal(red)***********************************************/
			if(q_red>0) {
				flag_red ++;
			}
			
// 			pthread_cond_signal(&cond_red);
		}
		if(i_red!=1){
			/*******************************synch_wait(red)***********************************************/
			q_red++;
			
			if(flag_red>0){
				flag_red--;
				q_red--;
				pthread_mutex_unlock(&cond_red);
			}
			
			pthread_mutex_unlock(&m_red);
			
			pthread_mutex_lock(&cond_red);
// 			pthread_cond_wait(&cond_red,&m_ex_red);
		}
	}
	else{
		/*******************************synch_wait(red)***********************************************/
		q_red++;
		
		if(flag_red>0){
			flag_red--;
			q_red--;
			pthread_mutex_unlock(&cond_red);
		}
		
		pthread_mutex_unlock(&m_red);
		
		pthread_mutex_lock(&cond_red);
// 		pthread_cond_wait(&cond_red,&m_ex_red);
	}
	if(count_r%N==0){
		count_r++;
		for(i=1; i<N && i<=(i_red-count_r); i++) {
			/*******************************synch_signal(red)***********************************************/
			if(q_red>0) {
				flag_red ++;
			}
// 			pthread_cond_signal(&cond_red);
		}
	}
	else {
		count_r++;
	}
	/*******************************synch_end***********************************************/
	if(flag_red>0){
		flag_red--;
		q_red--;
		pthread_mutex_unlock(&cond_red);
	}
	else{
		pthread_mutex_unlock(&m_red);
	}
	/************************************************/
	
// 	pthread_mutex_unlock(&m_ex_red);
	
	printf("red %d\n",number);
	
	pthread_mutex_lock(&mtx);
	
	un_count_r++;
	if(un_count_r%N == 0 || i_red == un_count_r) {
		printf("count_red %d\n",un_count_r);
		if(i_blue != un_count_b) {
			//ksupna ble
			
			pthread_mutex_lock(&m_blue);
			/*******************************synch_signal(blue)***********************************************/
			if(q_blue>0) {
				flag_blue ++;
			}
			/***********************************************************************/
			if(flag_blue>0){
				flag_blue--;
				q_blue--;
				pthread_mutex_unlock(&cond_blue);
			}
			else{
				pthread_mutex_unlock(&m_blue);
			}
// 			pthread_cond_signal(&cond_blue);
		}
		else {
			if(i_red == un_count_r) {
				//ksupna th main
				pthread_mutex_lock(&m_main);
				/*******************************synch_signal(main)***********************************************/
				if(q_main>0) {
					flag_main ++;
				}
				/***********************************************************************/
				if(flag_main>0){
					flag_main--;
					q_main--;
					pthread_mutex_unlock(&cond_main);
				}
				else{
					pthread_mutex_unlock(&m_main);
				}
// 				pthread_cond_signal(&cond_main);
			}
			else {
				//ksupna kokkino
				
				pthread_mutex_lock(&m_red);
				/*******************************synch_signal(red)***********************************************/
				if(q_red>0) {
					flag_red ++;
				}
				/***********************************************************************/
				if(flag_red>0){
					flag_red--;
					q_red--;
					pthread_mutex_unlock(&cond_red);
				}
				else{
					pthread_mutex_unlock(&m_red);
				}
// 				pthread_cond_signal(&cond_red);
			}
		}
	}
	
	pthread_mutex_unlock(&mtx);
	return(NULL);
}

void *blue(void *num_b){
	int number;
	int i;
	number = *(int *)(num_b);
	
	/*******************************synch_begin***********************************************/
	pthread_mutex_lock(&m_blue);
// 	pthread_mutex_lock(&m_ex_blue);
	
	check_blue++;
	if(check_blue == i_blue) {
		/*******************************synch_signal(blue)***********************************************/
		if(q_blue>0) {
			flag_blue ++;
		}
// 		pthread_cond_signal(&cond_blue);
	}
	if(i_blue!=1){
		/*******************************synch_wait(blue)***********************************************/
		q_blue++;
		
		if(flag_blue>0){
			flag_blue--;
			q_blue--;
			pthread_mutex_unlock(&cond_blue);
		}
		
		pthread_mutex_unlock(&m_blue);
		
		pthread_mutex_lock(&cond_blue);
		
// 		pthread_cond_wait(&cond_blue,&m_ex_blue);
	}
	if(count_b%N==0){
		count_b++;
		for(i=1; i<N && i<=(i_blue-count_b); i++) {
			/*******************************synch_signal(blue)***********************************************/
			if(q_blue>0) {
				flag_blue ++;
			}
	
// 			pthread_cond_signal(&cond_blue);
		}
	}
	else {
		count_b++;
	}
	/*******************************synch_end***********************************************/
	if(flag_blue>0){
		flag_blue--;
		q_blue--;
		pthread_mutex_unlock(&cond_blue);
	}
	else{
		pthread_mutex_unlock(&m_blue);
	}
	
// 	pthread_mutex_unlock(&m_ex_blue);
	
	printf("blue %d\n",number);
	
	pthread_mutex_lock(&mtx);
	
	un_count_b++;
	if(un_count_b%N == 0 || i_blue == un_count_b) {
		printf("count_blue %d\n",un_count_b);
		if(i_red != un_count_r) {
			//ksupna kokkino
			
			pthread_mutex_lock(&m_red);
			/*******************************synch_signal(red)***********************************************/
			if(q_red>0) {
				flag_red ++;
			}
			/***********************************************************************/
			if(flag_red>0){
				flag_red--;
				q_red--;
				pthread_mutex_unlock(&cond_red);
			}
			else{
				pthread_mutex_unlock(&m_red);
			}
// 			pthread_cond_signal(&cond_red);
		}
		else {
			if(i_blue == un_count_b) {
				//ksupna th main
				pthread_mutex_lock(&m_main);
				/*******************************synch_signal(main)***********************************************/
				if(q_main>0) {
					flag_main ++;
				}
				/***********************************************************************/
				if(flag_main>0){
					flag_main--;
					q_main--;
					pthread_mutex_unlock(&cond_main);
				}
				else{
					pthread_mutex_unlock(&m_main);
				}
// 				pthread_cond_signal(&cond_main);
			}
			else {
				//ksupna ble
				
				pthread_mutex_lock(&m_blue);
				/*******************************synch_signal(ble)***********************************************/
				if(q_blue>0) {
					flag_blue ++;
				}
				/***********************************************************************/
				if(flag_blue>0){
					flag_blue--;
					q_blue--;
					pthread_mutex_unlock(&cond_blue);
				}
				else{
					pthread_mutex_unlock(&m_blue);
				}
// 				pthread_cond_signal(&cond_blue);
			}
		}
	}
	pthread_mutex_unlock(&mtx);
	
	return(NULL);
}




