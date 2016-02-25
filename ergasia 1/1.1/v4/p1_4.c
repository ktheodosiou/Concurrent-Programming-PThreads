#include <stdio.h>
#include <ctype.h>

#define N 100000

//in1 out1 end_flag ein1ai global

int main(int argc, char *argv[]){
	char l1,l2,array[N];
	int in1,out1,end_flag1;
	
	in1 = 0;
	out1 = N;
	end_flag1 = 0;
	
	do{//bgazoume ta perita kena sthn arxh
		l1=getchar();
		if(l1==EOF){
			end_flag1 = 1;
			return(0);
		}
	}
	while(l1=='\n' || l1==' ');
	
	
	while(end_flag1!=1){
		l2=getchar();
		if(l2==EOF){
			end_flag1 = 1;
			
		}
		
		while(in1 == out1){}//synthiki anamonhs
		
		if(l1!='\n' && l1!=' '){
			array[in1] = l1;
			in1 = (in1+1)%N;
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
				array[in1] = ' ';
				in1 = (in1+1)%N;
			}
			l1 = l2;
		}
	}
	
	array[in1] = '\0';
	printf(" %s",array);
	return(0);
}