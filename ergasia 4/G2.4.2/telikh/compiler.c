/*ola ta kena bytes exoun 0*/
/*den epitrepodai emfoleumena mutexes*/
/*ta regs pairnoun times apo (-128) ews 127*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include<pthread.h>

#define N 100
#define N_reg 11
#define N_exec 5
#define N_thread 4

struct local_task_state{
	int id;   //o kwdikos kathe task
	char state[9];  //h katastash tou
	int *idx;    //h metavlhth tou kathe task pou ''epikoinwnei'' me th mnhmh(deixnei sthn prwth thesh twn regs)
	int reg[N_reg]; //ta ges pou exei kathe thread
	int pc;  //metrhths edolwn
	int sem; //h metavlhth pou apothhkeuetai to address tou shmatoforou
	time_t waket; //h metavlhth pou ''metraei'' to xrono
	int *local_Mem;  //h eswterikh mnhmh tou kathe task
};

struct global_state{
	int *global_mem;  //h global_mem tou programmatos
	int *code;        //o kwdikas olwn twn body
	struct local_task_state *tasks; //ola ta task
	int *cur;          //poio task ekteleitai
};

void *core(void *r);
	
	/*mutexes*/
	pthread_mutex_t mtx,mtx_count,mtx_main;
	pthread_cond_t cond_mtx,cond_main; 
	/*Header*/
	int globals_size, num_of_tasks;
	/*Body*/
	int *locals_size, *code_size;
	/*Task*/
	int *task_body, *task_arg;
	/*Branches*/
	int *max_pc;
	/*counters*/
	int count_blc[N_thread],count_slp[N_thread],count_stop[N_thread];
	
	struct global_state glb;
	char *global_memory;
	
int main(int argc, char *argv[]) { 
	int fd_hex, check, i, temp, k;
	/*Header*/
	int num_of_bodies, tot_code_size, global_init;
	/*thread*/
	pthread_t p_cpu;
	
	int c[N];
	int begin[4] = {0xde, 0xad, 0xbe, 0xaf};
	int body[4] = {0xde, 0xad, 0xc0, 0xde};
	int task[4] = {0xde, 0xad, 0xba, 0xbe};
	int end[4] = {0xfe, 0xe1, 0xde, 0xad};
	
	/*******************************DIAVASMA***********************************************/
	/*elegxoume an exei dwsei argument o xrhsths*/
	if(argc<2){
		printf("Put an argument\n");
		return(-1);
	} 
	/*anoigoume ta arxeia pou mas xreiazodai*/
	fd_hex = open(argv[1], O_RDONLY);
	if(fd_hex<0){
		printf("Error with fd_hex\n");
		return(-1);
	}
	
	/*HEADER*/
	/*arxikopoihsh c gia swsto diavasma 1 byte*/
	for(i=0;i<N;i++){
		c[i] = 0x0;
	}
	
	/*diavasma kai elegxos MagicBeg*/
	i = 0;
	do {
		check = read(fd_hex, &c[i], 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			
			return(-1);
		}
		if(begin[i] != c[i]){
			printf("This is not an executable hexfile!!\n");
			close(fd_hex);
			
			return(-1);
		}
		i++;
	}while(i<4);
	
	/*diavasma globals size kai elegxos*/
	for(i=0;i<4;i++){
		c[i] = 0x0;
	}
	check = read(fd_hex, c, 1);
	if(check<=0){
		if(check==0){
			printf("Control reaches end of non-void function\n");
		}
		else{
			printf("Problem with read\n");
		}
		close(fd_hex);
		
		return(-1);
	}
	globals_size = c[0];
	glb.global_mem =(int *) malloc(sizeof(int) * globals_size);
	if(glb.global_mem==NULL) {
		printf("Problem with malloc0\n");
		close(fd_hex);
		return(-1);
	}
	
	/*Num of bodies*/
	c[0] = 0x0;
	check = read(fd_hex, c, 1);
	if(check<=0){
		if(check==0){
			printf("Control reaches end of non-void function\n");
		}
		else{
			printf("Problem with read\n");
		}
		close(fd_hex);
		free(glb.global_mem);
		return(-1);
	}
	num_of_bodies = c[0];
	
	locals_size = (int *) malloc(sizeof(int)*num_of_bodies);
	if(locals_size==NULL) {
		printf("Problem with malloc1\n");
		close(fd_hex);
		free(glb.global_mem);
		return(-1);
	}
	code_size = (int *) malloc(sizeof(int)*num_of_bodies);
	if(locals_size==NULL) {
		printf("Problem with malloc1\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		return(-1);
	}
	
	/*total_code_size*/
	c[0] = 0x0;
	for(i=0;i<2;i++){
		check = read(fd_hex,&c[i], 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			return(-1);
		}
	}
	tot_code_size = (c[0]*16*16) +  c[1];
	
	glb.code = (int *) malloc(sizeof(int)*tot_code_size);
	if(glb.code==NULL) {
		printf("Problem with malloc1\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		return(-1);
	}
	
	/*num of tasks*/
	for(i=0;i<2;i++){
		c[i] = 0x0;
	}
	check = read(fd_hex, c, 1);
	if(check<=0){
		if(check==0){
			printf("Control reaches end of non-void function\n");
		}
		else{
			printf("Problem with read\n");
		}
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		free(glb.code);
		return(-1);
	}
	num_of_tasks = c[0];
	
	glb.tasks = (struct local_task_state*) malloc(sizeof(struct local_task_state)*num_of_tasks);
	if(glb.tasks==NULL) {
		printf("Problem with malloc2\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		free(glb.code);
		return(-1);
	}
	/*arxikopoihsh tou local_task_state gia kathe task*/
	for(i=0; i<num_of_tasks; i++) {
		glb.tasks[i].id = i;
		strcpy(glb.tasks[i].state,"READY");
		glb.tasks[i].idx = glb.tasks[i].reg;
		for(k=0; k<N_reg; k++) {
			glb.tasks[i].reg[k] = 0;
		}
		glb.tasks[i].pc = 0;
		glb.tasks[i].sem = -1;
		glb.tasks[i].waket = -1;
	}
	
	/*GLOBAL INIT*/
	c[0] = 0x0;
	for(i=0; i<globals_size; i++) {
		check = read(fd_hex, c, 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			return(-1);
		}
		global_init = c[0];
		/*gia na uposthrizei kai arnhtikous arithmous*/
		if(global_init>=0x80){
			global_init = global_init - 256;
		}
		glb.global_mem[i] = global_init;
		c[0] = 0x0;
	}
	
	/*BODY*/
	k = 0;
	for(temp=0; temp<num_of_bodies; temp++) {
		i = 0;
		/*magic body*/
		do {
			check = read(fd_hex, &c[i], 1);
			if(check<=0){
				if(check==0){
					printf("Control reaches end of non-void function\n");
				}
				else{
					printf("Problem with read\n");
				}
				close(fd_hex);
				free(glb.global_mem);
				free(locals_size);
				free(code_size);
				free(glb.code);
				free(glb.tasks);
				return(-1);
			}
			if(body[i] != c[i]){
				printf("ERROR: you haven't begin a body\n");
				close(fd_hex);
				free(glb.global_mem);
				free(locals_size);
				free(code_size);
				free(glb.code);
				free(glb.tasks);
				return(-1);
			}
			c[i] = 0x0;
			i++;
		}while(i<4);
		
		/*locals size*/
		check = read(fd_hex, c, 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			return(-1);
		}
		locals_size[temp] = c[0];
		
		/*code size*/
		c[0] = 0x0;
		check = read(fd_hex, c, 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			return(-1);
		}
		code_size[temp] = c[0];
		
		/*body */
		
		for(i=0; i<code_size[temp]; i++) {
			c[0] = 0x0;
			check = read(fd_hex, c, 1);
			if(check<=0){
				if(check==0){
					printf("Control reaches end of non-void function\n");
				}
				else{
					printf("Problem with read\n");
				}
				close(fd_hex);
				free(glb.global_mem);
				free(locals_size);
				free(code_size);
				free(glb.code);
				free(glb.tasks);
				return(-1);
			}
			glb.code[k] = c[0];
			k++;
		}
	}
	/*elegxos oti ola ta 2 bytes einai megalytera h isa toy 0*/
	for(i=1;i<tot_code_size;i=i+3){
		if(glb.code[i]>N_reg){
			printf("error: reg%d undeclared\n",glb.code[i]);
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			return(-1);
		}
		else if(glb.code[i]<0){
			printf("Segmentation fault (core dumped)6\n");
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			return(-1);
		}	
	}
	
	/*TASKS*/
	c[0] = 0x0;
	task_body = (int*) malloc(sizeof(int)*num_of_tasks);
	if(task_body==NULL) {
		printf("Problem with malloc2\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		free(glb.code);
		free(glb.tasks);
		return(-1);
	}
	task_arg = (int*) malloc(sizeof(int)*num_of_tasks);
	if(task_arg==NULL) {
		printf("Problem with malloc2\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		free(glb.code);
		free(glb.tasks);
		free(task_body);
		return(-1);
	}
	for(temp=0; temp<num_of_tasks; temp++) {
		i=0;
		do {
			check = read(fd_hex, &c[i], 1);
			if(check<=0){
				if(check==0){
					printf("Control reaches end of non-void function\n");
				}
				else{
					printf("Problem with read\n");
				}
				close(fd_hex);
				free(glb.global_mem);
				free(locals_size);
				free(code_size);
				free(glb.code);
				free(glb.tasks);
				free(task_body);
				free(task_arg);
				return(-1);
			}
			if(task[i] != c[i]){
				printf("This is not an executable hexfile!!\n");
				close(fd_hex);
				free(glb.global_mem);
				free(locals_size);
				free(code_size);
				free(glb.code);
				free(glb.tasks);
				free(task_body);
				free(task_arg);
				return(-1);
			}
			i++;
		}while(i<4);
		
		/*task body*/
		c[0] = 0x0;
		check = read(fd_hex, c, 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			free(task_body);
			free(task_arg);
			return(-1);
		}
		
		task_body[temp] = c[0];
		if(task_body[temp]>num_of_bodies || task_body[temp]<0){
			printf("This body %d doesn't exist\n", task_body[temp]);
		}
		
		glb.tasks[temp].local_Mem = (int *)malloc(sizeof(int)*(locals_size[task_body[temp]]));
		if(glb.tasks[temp].local_Mem==NULL) {
			printf("Problem with malloc2\n");
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			free(task_body);
			free(task_arg);
			for(i = 0; i<temp; i++) {
				free(&glb.tasks[i]);
			}
			return(-1);
		}
		/*arxikopoihsh tou pc tou kathe task sthn arxh tou body tou*/
		for(k=1; k<=num_of_bodies; k++){
			if(k<task_body[temp]){
				glb.tasks[temp].pc = glb.tasks[temp].pc + code_size[k-1]/3;
			}
			else {
				break;
			}
		}
		/*task arg*/
		c[0] = 0x0;
		check = read(fd_hex, c, 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			free(task_body);
			free(task_arg);
			for(i = 0; i<num_of_tasks; i++) {
				free(&glb.tasks[i]);
			}
			return(-1);
		}
		
		task_arg[temp] = c[0];
		/*vazoume sthn teleutaia thesh kathe local_mem to task_argument*/
		glb.tasks[temp].local_Mem[locals_size[task_body[temp]]-1] = task_arg[temp];
	}
	
	/*Magic end*/
	i=0;
	do {
		check = read(fd_hex, &c[i], 1);
		if(check<=0){
			if(check==0){
				printf("Control reaches end of non-void function\n");
			}
			else{
				printf("Problem with read\n");
			}
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			free(task_body);
			free(task_arg);
			for(i = 0; i<num_of_tasks; i++) {
				free(&glb.tasks[i]);
			}
			break;
		}
		if(end[i] != c[i]){
			printf("Control reaches end of non-void function!!!\n");
			close(fd_hex);
			free(glb.global_mem);
			free(locals_size);
			free(code_size);
			free(glb.code);
			free(glb.tasks);
			free(task_body);
			free(task_arg);
			for(i = 0; i<num_of_tasks; i++) {
				free(&glb.tasks[i]);
			}
			return(-1);
		}
		i++;
	}while(i<4);
	/***********************************TELOS DIAVASMATOS*************************************************************/
	
	/*YLOPOIHSH EDOLWN*/
	global_memory =(char *) malloc(sizeof(char) * globals_size);
	if(global_memory==NULL) {
		printf("Problem with malloc0\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		free(glb.code);
		free(glb.tasks);
		free(task_body);
		free(task_arg);
		for(i = 0; i<num_of_tasks; i++) {
			free(&glb.tasks[i]);
		}
		return(-1);
	}
	
	
	max_pc = (int *) malloc(sizeof(int)*num_of_bodies);
	if(locals_size==NULL) {
		printf("Problem with malloc1\n");
		close(fd_hex);
		free(glb.global_mem);
		free(locals_size);
		free(code_size);
		free(glb.code);
		free(glb.tasks);
		free(task_body);
		free(task_arg);
		for(i = 0; i<num_of_tasks; i++) {
			free(&glb.tasks[i]);
		}
		free(global_memory);
		return(-1);
	}
	
	/*boithitikos pinakas gia ta Branches oste na elegxoume to mexri poso mporei na paei to pc*/
	max_pc[0] = code_size[0];
	for(i=1;i<num_of_bodies;i++){
		max_pc[i] = max_pc[i-1] + code_size[i];
	}
	
	glb.cur = (int *) malloc(sizeof(int)*N_thread);
	if(glb.cur==NULL) {
		printf("Problem with malloc1\n");
		close(fd_hex);
		
		return(-1);
	}
	
	check = pthread_mutex_init(&mtx_count,NULL);
	
	check = pthread_mutex_init(&mtx,NULL);
	pthread_cond_init(&cond_mtx,NULL);
	
	check = pthread_mutex_init(&mtx_main,NULL);
	pthread_cond_init(&cond_main,NULL);
	check = pthread_mutex_lock(&mtx_main);
	
	for(i=0;i<N_thread && i<num_of_tasks;i++){
		glb.cur[i] = -1;
		count_stop[i] = 0;
		count_blc[i] = 0;
		count_slp[i] = 0;
	}
	for(i=0;i<N_thread && i<num_of_tasks;i++){

		check = pthread_mutex_lock(&mtx);
		
		check = pthread_create(&p_cpu,NULL,&core,&i);
		
		pthread_cond_wait(&cond_mtx,&mtx);
		check = pthread_mutex_unlock(&mtx);
	}
	
	pthread_cond_wait(&cond_main,&mtx_main);
	check = pthread_mutex_unlock(&mtx_main);
	
	close(fd_hex);
	return (0);
}

void *core(void *r){
	int cpu, min_cur, max_cur,p,k,i,j, temp, flag_yield;
	/*local sums counter */
	int sum_blc, sum_stop, sum_slp;
	/*sleep*/
	time_t seconds;
	
	pthread_mutex_lock(&mtx);
	cpu = *(int *)(r);
	pthread_cond_signal(&cond_mtx);
	pthread_mutex_unlock(&mtx);
	
	temp = num_of_tasks/N_thread;
	
	if(cpu  < ( num_of_tasks%N_thread)){
		min_cur = (temp + 1)*cpu;
		max_cur = min_cur + temp ;
	}
	else{
		min_cur = (cpu*temp) + (num_of_tasks%N_thread);  
		max_cur = min_cur + temp -1 ;
	}
	
	glb.cur[cpu] = min_cur-1;
	while(1) {
		pthread_mutex_lock(&mtx_count);
		
		count_stop[cpu] = 0;
		count_blc[cpu] = 0;
		count_slp[cpu] = 0;
		
		for(p=min_cur;p<=max_cur;p++){
			
			if(p>glb.cur[cpu] && (strcmp(glb.tasks[p].state,"READY")==0)){
				glb.cur[cpu] = p;
				break;
			}
			else if(strcmp(glb.tasks[p].state,"STOPPED")==0){
				count_stop[cpu]++;
			}
			else if((strcmp(glb.tasks[p].state,"SLEEPING")==0)){
				//elegxos ean exei perasei o xronos toy task to kanoume ready pairnei ton epeksergasth kai ayksanoume kai ton counter tou ready
				seconds = time(NULL);
				
				if(seconds>=glb.tasks[p].waket){
					glb.tasks[p].waket = -1;
					glb.cur[cpu] = p;
					strcpy(glb.tasks[p].state,"READY");
					break;
				}
				count_slp[cpu]++;
			}
			else if(strcmp(glb.tasks[p].state,"BLOCKED")==0){
				count_blc[cpu]++;
			}
		}
		
		i = glb.cur[cpu];
		sum_blc = 0;
		sum_slp = 0;
		sum_stop = 0;
		
		for(j=0;j<N_thread;j++){
			sum_blc = sum_blc + count_blc[j];
			sum_slp = sum_slp + count_slp[j];
			sum_stop = sum_stop + count_stop[j];
			
		}
	
		if(glb.cur[cpu]==max_cur){
			glb.cur[cpu] = min_cur-1;
		}
		else if((sum_blc+sum_stop)==num_of_tasks){
			if(sum_stop==num_of_tasks){
				//stelnei shma sth main
				pthread_mutex_lock(&mtx_main);
				pthread_cond_signal(&cond_main);
				pthread_mutex_unlock(&mtx_main);
				return(NULL);
			}
			else {
				printf("Segmentation fault!!\n");
				
				//stelnei shma sth main
				pthread_mutex_lock(&mtx_main);
				pthread_cond_signal(&cond_main);
				pthread_mutex_unlock(&mtx_main);
				return(NULL);
			}
		}
		else if(count_slp[cpu]==(max_cur-min_cur+1)){
			glb.cur[cpu] = min_cur-1;
			pthread_mutex_unlock(&mtx_count);
			continue;
		}
		else if(count_stop[cpu]==(max_cur-min_cur+1)){//ean teleiosan ta dika tou tasks epistrefei sth main
			pthread_mutex_unlock(&mtx_count);
			return(NULL);
		}
		else if(p==(max_cur+1)){
			glb.cur[cpu] = min_cur-1;
			pthread_mutex_unlock(&mtx_count);
			continue;
		}
		
		pthread_mutex_unlock(&mtx_count);
		
		flag_yield = 0;
		//k ta bytes tou kwdika kai i ta task
// 		printf("The task %d has the processor \n\n",i);
		for(j=0;j<N_exec;j++){
			k = glb.tasks[i].pc*3;
			switch(glb.code[k]){
				/*Load Store*/
				case 1:  //LLOAD
					if(glb.code[k+2]>locals_size[i]){
						printf("Warning: you have not allocate the memory to which you go to access\n");
						break;
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].local_Mem[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 2: //LLOADi
					if((glb.code[k+2]+*glb.tasks[i].idx)>locals_size[i]){
						printf("Warning: you have not allocate the memory to which you go to access\n");
						break;
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].local_Mem[glb.code[k+2]+*glb.tasks[i].idx];
					glb.tasks[i].pc++;
					break;
				case 3:   //GLOAD
					if((glb.code[k+2])>globals_size){
						printf("Warning: you have not allocate the memory to which you go to access\n");
						break;
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.global_mem[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 4:  //GLOADi
					if((glb.code[k+2]+*glb.tasks[i].idx)>globals_size){
						printf("Warning: you have not allocate the memory to which you go to access\n");
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.global_mem[glb.code[k+2]+*glb.tasks[i].idx];
					glb.tasks[i].pc++;
					break;
				case 5:  //LSTORE
					if(glb.code[k+2]>locals_size[i]){
						printf("Warning: you have not allocate the memory to which you go to access\n");
						break;
					}
					glb.tasks[i].local_Mem[glb.code[k+2]] = glb.tasks[i].reg[glb.code[k+1]];
					glb.tasks[i].pc++;
					break;
				case 6:  //LSTOREi
					if((glb.code[k+2]+*glb.tasks[i].idx)>locals_size[i]){
						printf("Warning: you have not allocate the memory to which you go to access\n");
						break;
					}
					glb.tasks[i].local_Mem[glb.code[k+2]+*glb.tasks[i].idx] = glb.tasks[i].reg[glb.code[k+1]];
					glb.tasks[i].pc++;
					break;
				case 7:  //GSTORE
					if((glb.code[k+2])>globals_size){
						printf("Warning: you have not allocate the memory to which you go to access\n");
						break;
					}
					glb.global_mem[glb.code[k+2]] = glb.tasks[i].reg[glb.code[k+1]];
					glb.tasks[i].pc++;
					break;
				case 8:  //GSTOREi
					if((glb.code[k+2]+*glb.tasks[i].idx)>globals_size){
						printf("Warning: you have not allocate the memory to which you go to access\n");
					}
					glb.global_mem[glb.code[k+2]+*glb.tasks[i].idx] = glb.tasks[i].reg[glb.code[k+1]];
					glb.tasks[i].pc++;
					break;
					
				/*Registers*/
				case 9:  //SET
					if(glb.code[k+2]>=0x80){
						glb.code[k+2] = glb.code[k+2] - 256;
					}
					
					glb.tasks[i].reg[glb.code[k+1]] = glb.code[k+2];
					glb.tasks[i].pc++;
					break;
				case 10: //ADD
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						exit(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)1\n");
						exit(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]+glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 11:  //SUB
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						exit(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)2\n");
						exit(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]-glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 12: //MUL
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						exit(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)3\n");
						exit(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]*glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 13:  //DIV
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						exit(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)4\n");
						exit(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]/glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 14:  //MOD
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						exit(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)5\n");
						exit(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]%glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
					
				/*Branches*/
				case 15:  //BRGZ
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					if (glb.tasks[i].reg[glb.code[k+1]] > 0) {
						if( (glb.tasks[i].pc+temp*3)>max_pc[task_body[i]-1]) {
							printf("Segmentation fault!1\n");
							exit(-1);
						}
						glb.tasks[i].pc= glb.tasks[i].pc+temp;
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 16:  //BRGEZ
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]] >=0) {
						
						if( (glb.tasks[i].pc+temp)*3> max_pc[task_body[i]-1]) {
							printf("Segmentation fault!2\n");
							exit(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp;
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 17:  //BRLZ
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]]  < 0) {
						if( (glb.tasks[i].pc+temp)*3> max_pc[task_body[i]-1]) {
							printf("Segmentation fault!3    %d\n", i);
							exit(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp; 
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 18:  //BRLEZ
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]] <= 0) {
						if( (glb.tasks[i].pc+temp)*3>max_pc[task_body[i]-1]) {
							printf("Segmentation fault!4\n");
							exit(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp;
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 19:  //BREZ
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]] == 0) {
						if( (glb.tasks[i].pc+temp)*3>max_pc[task_body[i]-1]) {
							printf("Segmentation fault!5\n");
							exit(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp; 
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 20:  //BRA
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if( (glb.tasks[i].pc+temp)*3> max_pc[task_body[i]-1]) {
						printf("Segmentation fault!6\n");
						exit(-1);
					}
					
					glb.tasks[i].pc= glb.tasks[i].pc+temp; 
					break;
					
				/*synch*/
				case 21:  //DOWN
					pthread_mutex_lock(&mtx_count);
					glb.global_mem[glb.code[k+2]]--;
					if (glb.global_mem[glb.code[k+2]] < 0) {
						strcpy(glb.tasks[i].state,"BLOCKED"); 
						glb.tasks[i].sem = glb.code[k+2];
						flag_yield = 1;
					}
					glb.tasks[i].pc++;
					pthread_mutex_unlock(&mtx_count);
					break;
				case 22:  //UP
					pthread_mutex_lock(&mtx_count);
					glb.global_mem[glb.code[k+2]]++;
					if (glb.global_mem[glb.code[k+2]] <=0) {
						for(temp = 0; temp<num_of_tasks; temp++) {
							if(glb.tasks[temp].sem ==  glb.code[k+2]) {
								strcpy(glb.tasks[temp].state,"READY");
								glb.tasks[temp].sem = -1;
								pthread_mutex_unlock(&mtx_count);
								break;
							}
						}
					}
					glb.tasks[i].pc++;
					pthread_mutex_unlock(&mtx_count);
					break;
					
				/*Varia*/
				case 23:  //YIELD
					flag_yield = 1;
					glb.tasks[i].pc++;
					break;
				case 24:  //SLEEP
					pthread_mutex_lock(&mtx_count);
					strcpy(glb.tasks[i].state,"SLEEPING");
					pthread_mutex_unlock(&mtx_count);
					seconds = time(NULL);
					glb.tasks[i].waket=seconds + (time_t)glb.tasks[i].reg[glb.code[k+1]];
					glb.tasks[i].pc++;
					flag_yield = 1;
					break;
				case 25:  //PRINT
					temp = 0;
					while(temp<globals_size){
						global_memory[temp] = (unsigned char)glb.global_mem[temp];
						temp++;
					}
					printf("%d: %s\n",glb.tasks[i].id,&global_memory[glb.code[k+2]]);
					glb.tasks[i].pc++;
					break;
				case 26:  //EXIT
					pthread_mutex_lock(&mtx_count);
					strcpy(glb.tasks[i].state, "STOPPED");
					pthread_mutex_unlock(&mtx_count);
					glb.tasks[i].pc++;
					flag_yield = 1;
					break;
					
				default:
					printf("error:%d pc:%d task %d undeclared\n", glb.code[k-3], glb.tasks[i].pc, i);
					glb.tasks[i].pc++;
					exit(-1);
			}
			if(flag_yield == 1){
				break;
			}
		}
	}
	
	
	return(NULL);
}