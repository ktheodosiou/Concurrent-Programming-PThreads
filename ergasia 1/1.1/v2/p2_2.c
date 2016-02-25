#include <stdio.h>
#include <ctype.h>

#define N 1000


//in out end_flag einai global

int main(int argc, char *argv[]){
	
	char l1,array1[N],array2[N];
	int in1,out1,in2,out2,i,end_flag1,end_flag2;
	
// 	i = 0;
// 	while(1){
// 		l1=getchar();
// 		if(l1==EOF){
// // 			end_flag = 1;
// 			break;
// 		}
// 		array1[i] = l1;
// 		i++;
// 	}
	
// 	end_flag = 0;
	
// 	ina = 1000; //tha diagrafei
// 	outa = 0;
// 	ind = 0;
// 	outd = -1;

	while(1){
		if(end_flag1==1 && in1 == out1+1){
			end_flag2 = 1;
			break;
		}
		
		while(in1 == out1+1) {}//einai out+1 giati ayth einai h thesh poy theloyme na diabasoyme
		
		while(in2 == out2){}
		
		out1 = (out1+1)%N;
		array2[in2] = toupper(array1[out1]);
		in2 = (in2+1)%N;
	}
	
	
	printf("%s",array2);
	return(0);
}


