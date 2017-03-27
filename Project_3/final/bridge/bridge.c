#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 10

int i_blue, i_red;          //o arithmos twn amaksiwn pou dinei kathe fora o xrhsths  
int un_count_b, un_count_r; //o arithmos twn autokinhtwn pou exoun perasei ap th gefura
int count_b, count_r;       //o arithmos twn amaksiwn pou exoume dwsei thn adeia na perasoun apo th gefyra
int check_blue,check_red;   //o arithmos twn amaksiwn pou perimenoun sthn oura ths gefuras

pthread_mutex_t m_ex_blue, m_ex_red;  //gia th xrhsh twn conditions cond_blue,cond_red
pthread_mutex_t mtx_main;             //gia th xrhsh tou condition cond_main
pthread_mutex_t mtx;     //gia amoivaio apokleismo
pthread_cond_t cond_blue, cond_red;  //h oura anamonhs twn amaksiwn blue kai red adistoixa
pthread_cond_t cond_main;     //perimenei h main n kanei thn epomenh epanalhyh

void *red(void *num_r);
void *blue(void *num_b);

int main(int argc, char *argv[]){
	char c;
	int i;
	int check;
	int *array_r, *array_b;  //arithmos amaksiou twn red kai blue pou dinei o xrhsths
	pthread_t *p_red, *p_blue;
	
	/*arxikopoihsh mutexes kai conditions*/
	pthread_mutex_init(&m_ex_red,NULL);
	pthread_mutex_init(&m_ex_blue,NULL);
	pthread_mutex_init(&mtx_main,NULL);
	pthread_mutex_init(&mtx,NULL);
	
	pthread_cond_init(&cond_red,NULL);
	pthread_cond_init(&cond_blue,NULL);
	pthread_cond_init(&cond_main,NULL);
	
	/*epanalhyh gia na dwsei o xrhsths oses fores thelei autokinhta*/
	while(1) {
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
		/*perimenoume mexri na perasoun ola ta amaksia pou exei dwsei o xrhsths apo th gefura*/
		pthread_mutex_lock(&mtx_main);
		pthread_cond_wait(&cond_main,&mtx_main);
		pthread_mutex_unlock(&mtx_main);
		
		printf("End of the day\n");
	}
	return(0);
	
}

void *red(void *num_r){
	int number;
	int i;
	number = *(int *)(num_r);
	
	/*perimenoun ola ta kokkina amaksia gia na boun sthn oura ths gefuras*/
	pthread_mutex_lock(&m_ex_red);
	
	/*elegxoume an o xrhsths den dwsei katholou blue autokinhta gia na ksekinhsoun ta red*/
	if(i_blue==0){
		check_red++;
		/*elegxoume an exoun bei sthn oura ths gefuras ta kokkina autokinhta kai ksupname to prwto pou bhke sthn oura*/
		if(check_red == i_red) {
			pthread_cond_signal(&cond_red);
		}
		/*an uparxoun perissotera tou 1 kokkina autokinhta, vazoume ta kokkina autokinhta sthn oura ths gefuras*/
		if(i_red!=1){
			pthread_cond_wait(&cond_red,&m_ex_red);
		}
	}
	/*an uparxoun blue autokinhta tha einai auta ta prwta pou tha boun sth gefura*/
	else{
		pthread_cond_wait(&cond_red,&m_ex_red);
	}
	/*kathe Niosto amaksi stelnei shma gia N-1 autokinhta*/
	if(count_r%N==0){
		count_r++;
		/*shma gia ta N-1 autokinhta*/
		for(i=1; i<N && i<=(i_red-count_r); i++) {
			pthread_cond_signal(&cond_red);
		}
	}
	/*an den hmaste sto Niosto autokinhto auksanoume to metrhth*/
	else {
		count_r++;
	}
	pthread_mutex_unlock(&m_ex_red);
	
	printf("red %d\n",number);
	
	/*kleidwnoume to mtx gia na uparxei amoivaios apokleismos metaksu twn kokkinwn autokinhtwn*/
	pthread_mutex_lock(&mtx);
	/*auksanoume to metrhth twn amaksiwn pou bainoun sth gefura*/
	un_count_r++;
	/*to Nosto amaksi pou tha bei sth gefura energopoiei oti krinei aparaithto*/
	if(un_count_r%N == 0 || i_red == un_count_r) {
		printf("count_red %d\n",un_count_r);
		/*energopoiei ta ble an den exoun teleiwsei*/
		if(i_blue != un_count_b) {
			pthread_cond_signal(&cond_blue);
		}
		/*an exoun teleiwsei ta ble*/
		else {
			/*kai exoun teleiwsei kai ta kokkina energopoiei th main*/
			if(i_red == un_count_r) {
				pthread_cond_signal(&cond_main);
			}
			/*kai den exoun teleiwsei ta kokkina energopoiei ta kokkina*/
			else {
				pthread_cond_signal(&cond_red);
			}
		}
	}
	/*stamatame ton amoivaio apokleismo afou teleiwse o krisimos kwdikas*/
	pthread_mutex_unlock(&mtx);
	return(NULL);
}

void *blue(void *num_b){
	int number;
	number = *(int *)(num_b);
	
	/*perimenoun ola ta ble amaksia gia na boun sthn oura ths gefuras*/
	pthread_mutex_lock(&m_ex_blue);
	
	check_blue++;
	if(check_blue>N) {
		pthread_cond_wait(&cond_blue, &m_ex_blue);
		count_b++;
		if(count_b%N!=0 && count_b!=i_blue){
			pthread_cond_signal(&cond_blue);
		}
	}
	else{
		count_b++;
	}
	
	pthread_mutex_unlock(&m_ex_blue);
	
	printf("blue %d\n",number);
	
	/*kleidwnoume to mtx gia na uparxei amoivaios apokleismos metaksu twn ble autokinhtwn*/
	pthread_mutex_lock(&mtx);
	/*auksanoume to metrhth twn amaksiwn pou bainoun sth gefura*/
	un_count_b++;
	if(un_count_b%N == 0 || i_blue == un_count_b) {
		printf("count_blue %d\n",un_count_b);
		/*energopoiei ta kokkina an den exoun teleiwsei*/
		if(i_red != un_count_r) {
			pthread_cond_signal(&cond_red);
		}
		/*an exoun teleiwsei ta kokkina*/
		else {
			/*kai exoun teleiwsei kai ta ble energopoiei th main*/
			if(i_blue == un_count_b) {
				pthread_cond_signal(&cond_main);
			}
			/*kai den exoun teleiwsei ta ble energopoiei ta ble*/
			else {
				do{
					sleep(1);
					pthread_cond_signal(&cond_blue);
				}while(check_blue%N != un_count_b%N && check_blue!=i_blue);
			}
		}
	}
	/*stamatame ton amoivaio apokleismo afou teleiwse o krisimos kwdikas*/
	pthread_mutex_unlock(&mtx);
	
	return(NULL);
}