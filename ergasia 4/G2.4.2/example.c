#include <stdio.h>

#define N_thread 7
#define N 12

int main(int argc, char *argv[]) { 
	int i=0,x;
	int cpu[N_thread],min[N_thread],max[N_thread];
	
	for(i=0;i<N_thread;i++){
		cpu[i] = i;
		min[i] = 0;
		max[i] = 0;
	}
	
	for(i=0;i<N_thread;i++){
		x = N/N_thread;
		if(cpu[i]  < ( N%N_thread)){
			min[i] = (x+1)*cpu[i];
			max[i] = min[i] + x ;
			printf("%d\n",cpu[i]);
		}
		else{
			min[i] = (cpu[i]*x) + (N%N_thread);  
			max[i] = min[i] + x -1;
		}
		
	}
	
	
	for(i=0;i<N_thread;i++){
		printf("cpu %d min %d max %d\n",cpu[i],min[i],max[i]);
	}
	return(0);
}