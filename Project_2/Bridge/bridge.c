#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 10
int un_count_b,un_count_r;   // o arithmos twn amaksiwn kathe xrwmatos pou pernaei th gefyra
int i_blue, i_red;          //o arithmos twn amaksiwn pou dinei kathe fora o xrhsths  

pthread_mutex_t mtx_blue, mtx_red;  //dwsimo seiras ap t main sta threads
pthread_mutex_t swap;                //enallagh seiras blue-red
pthread_mutex_t mtx;                //gia amoivaio apokleismo sta threads
pthread_mutex_t safe_mtx;          //gia na perimenoume na mas eidopoihsei to amaksi oti mphke sth gefyra

void *red(void *num_r);
void *blue(void *num_b);

/* to func pairnei timh 1 gia thn init, 2 lock, 3 unlock, 4 destroy. to check pairnei thn timh poy epestrepse h synarthsh*/
int error_check(int check, int func); 

int main(int argc, char *argv[]){
	char c;
	int i,sum;
	int check;
	int *array_r, *array_b;  //arithmos amaksiou twn red kai blue
	pthread_t *p_red, *p_blue;
	
	/*arxikopoioume ta mutexes*/
	check = pthread_mutex_init(&mtx_red, NULL);   
	error_check(check,1);
	check = pthread_mutex_init(&mtx_blue, NULL); 
	error_check(check,1);
	check = pthread_mutex_init(&swap, NULL);
	error_check(check,1);
	check = pthread_mutex_init(&safe_mtx, NULL);
	error_check(check,1);
	check = pthread_mutex_init(&mtx, NULL);
	error_check(check,1);
	
	/*epanalhyh gia na dwsei o xrhsths oses fores thelei autokinhta*/
	while(1) {
		/*ebodizoume ta threads na ksekinhsoun*/
		check = pthread_mutex_lock(&mtx_red);  
		error_check(check,2);
		check = pthread_mutex_lock(&mtx_blue);  
		error_check(check,2);
		check = pthread_mutex_lock(&safe_mtx);
		error_check(check,2);
		
		/*mhdenizoume tous metrhtes gia kathe kainouria mera*/
		i_blue = 0;
		i_red = 0;
		un_count_b=0;
		un_count_r=0;
		
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
			free(p_red);
			return(2);
		}
		array_r = (int*)malloc(sizeof(int)*i_red);
		if(array_r==NULL){										//elegxos epityxias ths malloc
			printf("Problem with memory allocation\n");
			free(p_red);
			free(p_blue);
			return(2);
		}
		array_b = (int*)malloc(sizeof(int)*i_blue);
		if(array_b==NULL){										//elegxos epityxias ths malloc
			printf("Problem with memory allocation\n");
			free(p_red);
			free(p_blue);
			free(array_r);
			return(2);
		}
		
		/*dhmioyrgoyme ta kokkina oxhmata*/
		for(i=0; i<i_red; i++){
			array_r[i] = i;
			check = pthread_create(&p_red[i], NULL, &red, &array_r[i]);
			if(check!=0) {
				printf("Problem to create %d red thread/car\n", i);
				
				free(p_red);
				free(p_blue);
				free(array_r);
				free(array_b);
				check = pthread_mutex_destroy(&mtx_red);
				error_check(check,4);
				check = pthread_mutex_destroy(&mtx_blue);
				error_check(check,4);
				check = pthread_mutex_destroy(&swap);
				error_check(check,4);
				check = pthread_mutex_destroy(&safe_mtx);
				error_check(check,4);
				check = pthread_mutex_destroy(&mtx);
				error_check(check,4);
				
				return(7);
			}
		}
		
		/*dhmioyrgoyme ta mple oxhmata*/
		for(i=0; i<i_blue; i++){
			array_b[i] = i;
			check = pthread_create(&p_blue[i], NULL, &blue, &array_b[i]);
			if(check!=0) {
				printf("Problem to create %d blue thread/car\n", i);
				
				free(p_red);
				free(p_blue);
				free(array_r);
				free(array_b);
				check = pthread_mutex_destroy(&mtx_red);
				error_check(check,4);
				check = pthread_mutex_destroy(&mtx_blue);
				error_check(check,4);
				check = pthread_mutex_destroy(&swap);
				error_check(check,4);
				check = pthread_mutex_destroy(&safe_mtx);
				error_check(check,4);
				check = pthread_mutex_destroy(&mtx);
				error_check(check,4);
				
				return(7);
			}
		}
		
		printf("red %d, blue %d\n",i_red, i_blue);
		/*epanalhpsh opou eisagoume sth gefyra kathe fora N oxhmata kathe xromatos mexri na eksantlithoun osa perimenoyne*/
		while(1) {
			/*kleidonoyme thn kateythinsh oste na mhn epitrapei sth main meta na ksypnisei oxhmata antitheths kateythinshs*/
			check = pthread_mutex_lock(&swap);
			error_check(check,2);
			if(i_red == un_count_r) {		//elegxos ean teleiosan ta kokkina oxhmata
				check = pthread_mutex_unlock(&swap);  //otan perasoun ola red dwse prosvash sta blue
				error_check(check,3);
			}
			sum = un_count_r;
			for(i=0; i<N && i<(i_red-sum); i++ ) {
				check = pthread_mutex_unlock(&mtx_red); //energopoioume ena amaksi na mpei sth gefyra
				error_check(check,3);
				check = pthread_mutex_lock(&safe_mtx);	//perimenoume na mas eidopoihsei to amaksi oti mphke sth gefyra
				error_check(check,2);
			}
			/*kleidonoyme thn kateythinsh gia na mhn epitrapei sth main na ksypnisei mple oxhmata mexri na bgei kai to teleytaio kokkino amaksi poy mphke sth gefyra*/
			check = pthread_mutex_lock(&swap);
			error_check(check,2);
			if(i_blue == un_count_b) {		//elegxos ean teleiosan ta mple oxhmata
				check = pthread_mutex_unlock(&swap); //otan perasoun ola blue dwse prosvash sta red
				error_check(check,3);
			}
			sum = un_count_b;
			for(i=0; i<N && i<(i_blue-sum); i++ ) {
				check = pthread_mutex_unlock(&mtx_blue);  //energopoioume ena amaksi na mpei sth gefyra
				error_check(check,3);
				check = pthread_mutex_lock(&safe_mtx);	//perimenoume na mas eidopoihsei to amaksi oti mphke sth gefyra
				error_check(check,2);
			}
			
			/*kleidonoyme thn kateythinsh oste na mhn epitrapei sth main na eleksei thn synthiki termatismoy ths while prin teleiosei to teleytaio mple amaksi*/
			check = pthread_mutex_lock(&swap);
			error_check(check,2);
			/*elegxos ean teleiosan ola ta oxhmata tote teleionei h mera*/
			if(un_count_r == i_red && un_count_b == i_blue){	
				check = pthread_mutex_unlock(&swap);
				error_check(check,3);
				check = pthread_mutex_unlock(&mtx_blue);
				error_check(check,3);
				check = pthread_mutex_unlock(&mtx_red);
				error_check(check,3);
				check = pthread_mutex_unlock(&safe_mtx);
				error_check(check,3);
				printf("End of the day\n");
				break;
			}
			check = pthread_mutex_unlock(&swap);
			error_check(check,3);
		}
		free(p_red);
		free(p_blue);
		free(array_r);
		free(array_b);
	}
	
	/*katastrefoyme ta mutexes*/
	check = pthread_mutex_destroy(&mtx_red);
	error_check(check,4);
	check = pthread_mutex_destroy(&mtx_blue);
	error_check(check,4);
	check = pthread_mutex_destroy(&swap);
	error_check(check,4);
	check = pthread_mutex_destroy(&safe_mtx);
	error_check(check,4);
	check = pthread_mutex_destroy(&mtx);
	error_check(check,4);
	
	return(0);
}

void *red(void *num_r){
	int number;
	int check;
	number = *(int *)(num_r);
	
	check = pthread_mutex_lock(&mtx_red); 	//kleidonoyme to amaksi mexri na to energopoihsei h main gia na mpei sth gefyra
	error_check(check,2);
	check = pthread_mutex_unlock(&safe_mtx); //leme sth main oti to amaksi mphke sth gefyra
	error_check(check,3);
	
	/*apokleismos metaksi ton threads oste na mporei na grapsei kai na afksisei to metrhth to kathena me asfaleia*/
	check = pthread_mutex_lock(&mtx);
		error_check(check,2);
		un_count_r++;
		printf("red %d\n", number);
		if(un_count_r%N == 0 || un_count_r == i_red){
			printf("count_red %d\n", un_count_r);
			check = pthread_mutex_unlock(&swap);
			error_check(check,3);
		}
	check = pthread_mutex_unlock(&mtx);
	error_check(check,3);
	
	return(NULL);
}

void *blue(void *num_b){
	int number;
	int check;
	number = *(int *)(num_b);
	
	check = pthread_mutex_lock(&mtx_blue);  //kleidonoyme to amaksi mexri na to energopoihsei h main gia na mpei sth gefyra
	error_check(check,2);
	check = pthread_mutex_unlock(&safe_mtx);	//leme sth main oti to amaksi mphke sth gefyra
	error_check(check,3);
	
	/*apokleismos metaksi ton threads oste na mporei na grapsei kai na afksisei to metrhth to kathena me asfaleia*/
	check = pthread_mutex_lock(&mtx);
		error_check(check,2);
		un_count_b++;
		printf("blue %d\n", number);
		if(un_count_b%N==0 || i_blue == un_count_b){
			printf("count_blue %d\n", un_count_b);
			check = pthread_mutex_unlock(&swap);
			error_check(check,3);
		}
	check = pthread_mutex_unlock(&mtx);
	error_check(check,3);
	
	return(NULL);
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