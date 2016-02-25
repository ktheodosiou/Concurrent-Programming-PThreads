#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

#define N 10

int in1=-1;
int in2=-1;
int out1=-1;
int out2=-1;

int end_flag1=0;
int end_flag2=0;

char array1[N];
char array2[N];

int end=0;

void*  p1_main(void *arg);
void*  p2_main(void *arg);

int main(int argc, char *argv[]){
	int check;
	pthread_t p1,p2,p3;
	
	check = pthread_create(&p1,NULL,&p1_main,NULL);
	if(check!=0){
		printf("thread2 : error! \n");               //yparxei thema
		return(1);
	}
		
	check = pthread_create(&p2,NULL,&p2_main,NULL);
	if(check!=0){
		printf("thread2 : error! \n");              //yparxei thema
		return(1);
	}
	
	while(end_flag2!= 1) {}
	
	return(0);
}

void*  p1_main(void *arg) {
	char l1,l2;
	
	do{//bgazoume ta perita kena sthn arxh
		l1=getchar();
		if(l1==EOF){
			end_flag1 = 1;
			return(NULL);
		}
	}
	while(l1=='\n' || l1==' ');
	
	
	while(end_flag1!=1){
		l2=getchar();
		if(l2==EOF){
			end_flag1 = 1;
			printf("eimai h p1 k molis teleiwsa!!!!!!!!!!!!!!!!!!!!!!!!\n");
		}

		
		while((in1+1)%N == out1 ){}//synthiki anamonhs
		printf("kollhse p1 \n");
		
		
		if(l1!='\n' && l1!=' '){
			array1[in1] = l1;
			in1 = (in1+1)%N;
			l1 = l2;
			continue;
		}
		printf("kollhse p1 \n");
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
// 	printf("eimai h p1 k molis teleiwsa!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return(NULL);
}

void*  p2_main(void *arg)  {
	
	while(1){
		if(end_flag1==1 ){
			if(in1 == out1){
// 				putchar('*');
				end_flag2 = 1;
				break;
			}
		}
		
		while(in1 == (out1+1)%N) {}//einai out+1 giati ayth einai h thesh poy theloyme na diabasoyme
		
		printf("kollhse p2.1 \n");
// 		while(in2 == out2){}
		
		array2[in2] = toupper(array1[out1]);
		out1 = (out1+1)%N;
		printf("kollhse p \n");
  		putchar(array2[in2]);
		fflush(NULL);
		
		in2 = (in2+1)%N;
	}
	
	printf("eimai h p2 k molis teleiwsa!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return(NULL);
}



