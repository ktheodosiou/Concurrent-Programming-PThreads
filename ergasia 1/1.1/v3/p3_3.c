#include <stdio.h>
#define N 1000


//in out end_flag einai global

int main(int argc, char *argv[]){
	int i,j,k;	
	char array[N],l1;
	
	i = 0;
	while(1){
		l1=getchar();
		if(l1==EOF){
// 			end_flag = 1;
			break;
		}
		array[i] = l1;
		i++;
	}
	
	j = 0;
	
	while(j<=i){
		
		
		for(k=0;k<30 && j<=i;k++){
			while(in2 == out2+1){}
			out2 = (out2+1)%N
			
			putchar(array[j]);
			j++; //j metakinhetai diaforetika
			
		}
		putchar('\n');
		
	}
	
	
	
	
	
	
	
	return(0);
}

