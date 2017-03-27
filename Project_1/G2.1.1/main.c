//o kodikas o opoios einai se sxolia einai gia th metrhsh tou xronou

#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
// #include <sys/times.h> 

#define N 1000

int in1=0;       //gia to grayimo ths 1hs apothhkhs
int in2=0;       //gia to grayimo ths 2hs apothhkhs   
int out1=0;      //gia to diavasma ths 1hs apothhkhs
int out2=0;      //gia to diavasma ths 2hs apothhkhs

int end_flag1=0; //flag gia to 1o thread 
int end_flag2=0; //flag gia to 2o thread
int end_flag3=0; //flag gia to 3o thread

char array1[N]; //1h apothhkh
char array2[N]; //2h apothhkh

void * p1_main(void *arg);
void * p2_main(void *arg);
void * p3_main(void *arg);

double t1, t2, t3;

int main(int argc, char *argv[]){
	int check;
// 	double T1, T2, tm;                
// 	struct tms start_time, stop_time;
	pthread_t p1,p2,p3;
	
// 	T1 = times(&start_time);   
	
	/*dhmiourgia threads kai elegxos epituxias tous*/
	
	check = pthread_create(&p1,NULL,&p1_main,NULL);            
	if(check!=0){
		printf("fault pthread 1\n");
		return(1);
	}
	check = pthread_create(&p2,NULL,&p2_main,NULL);
	if(check!=0){
		printf("fault pthread 2\n");
		return(1);
	}
	check = pthread_create(&p3,NULL,&p3_main,NULL);
	if(check!=0){
		printf("fault pthread 3\n");
		return(1);
	}
	
	
	/*anamonh tou end_flag3 na ginei 1 oste na termatisei to programma*/
	while(end_flag3!=1){}  
	
// 	T2 = times(&stop_time);        
// 	tm = (T2-T1)/1000;
// 	tm = tm +t1 +t2 +t3;
// 	printf("Execute time is %f clock ticks\n", tm);          
	
	return(0);
}


void *p1_main(void *arg){
	char l1,l2;
// 	double T1, T2;                
// 	struct tms start_time, stop_time;
	
// 	T1 = times(&start_time);   
	
	/*elegxos leukou xarakthra sthn arxh toy keimenou*/
	do{
		l1=getchar();
		if(l1==EOF){
			end_flag1 = 1;                   
			return(NULL);
		}
	}
	while(l1=='\n' || l1==' ');
	
	
	while(end_flag1!=1){
		
		l2=getchar();
		/*elegxos gia n doume an exei ftasei st telos tou arxeiou*/
		if(l2==EOF){   
			end_flag1 = 1; 
		}
		/*perimenoume thn p2, gia na mhn ksanagrayoume thesh pou den exei diavastei*/
		while((in1+1)%N==out1){}        
		
		/*afairoume peritta kena k \n*/
		if(l1!='\n' && l1!=' '){
			array1[in1] = l1;
			in1 = (in1+1)%N;           //kinoumaste kuklika sthn apothhkh
			l1 = l2;
			continue;
		}
		else{
					
			while(l2=='\n' || l2==' '){
				l2 = getchar();
				if(l2==EOF){
					end_flag1 = 1;    
					break;
				}
			}
			if(end_flag1 != 1) {      
				array1[in1] = ' ';
				in1 = (in1+1)%N;
			}
			l1 = l2;
		}
	}
	
// 	T2 = times(&stop_time);        
// 	t1=(T2-T1)/1000;
	
	return(NULL);
}

void * p2_main(void *arg){
// 	double T1, T2;               
// 	struct tms start_time, stop_time;
	
// 	T1 = times(&start_time);   
	
	while(1){
		/*thetoume to flag ths p2 1 otan exei teleiwsei k h p2 */
		if(end_flag1==1 && in1 == out1){             
			end_flag2 = 1;
			break;
		}
		
		/*elegsos sugxronismou me thn p1(gia na mh diavasoume thesh pou den exei prolavei na grayei h p1)*/
		while(in1==out1 && end_flag1!=1) {}        
		
		/*perimenoume na prolavei na diavasei h p3*/
		while((in2+1)%N==out2){}          
	
		if(in1==out1 && end_flag1==1) {            
			continue;
		}
		
		array2[in2] = toupper(array1[out1]);  //kanoume kefalaia t grammata k t vazoume sth 2h apothhkh
		out1 = (out1+1)%N;
		in2 = (in2+1)%N;
	}
	
// 	T2 = times(&stop_time);
// 	t2=(T2-T1)/1000;
	
	return(NULL);
}

void * p3_main(void *arg){
	int k;
// 	double T1, T2;  
// 	struct tms start_time, stop_time;
	
// 	T1 = times(&start_time);   
	
		while(1){
			
			/*thetoume to flag ths p3 1 otan exei teleiwsei to diavasma apo thn apothhkh */
			if(end_flag2==1 && in2==out2) {           
				end_flag3 = 1;
				break;
			}
			
			/*trexoume kai emfanizoume mia grammh 30 xarakthrwn mexri na teleiwsei h p2*/
			for(k=0; k<30 && (in2!=out2 ||(end_flag2!=1)); k++){                  
				while(in2==out2 && end_flag2!=1) {            
					if(end_flag2 == 1){               
						break;
					}
				}
				
				putchar(array2[out2]);
				out2 = (out2+1)%N;
				fflush(NULL);
			}
			putchar('\n');
		}
		
// 		T2 = times(&stop_time);
// 		t3=(T2-T1)/1000;
	
	return(NULL);
}