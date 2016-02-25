/*ola ta kena bytes exoun 0*/
/*den epitrepodai emfoleumena idia mutexes*/
/*ta regs pairnoun times apo (-128) ews 127*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define N 100
#define N_reg 11
#define N_exec 1

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
	int cur;          //poio task ekteleitai
};

int main(int argc, char *argv[]) { 
	int fd_hex, check, i, temp, k, j, p;
	/*Header*/
	int globals_size, num_of_bodies, num_of_tasks, tot_code_size, global_init;
	/*Body*/
	int *locals_size, *code_size;
	/*Task*/
	int *task_body, *task_arg;
	/*counters*/
	int count_blc,count_slp,count_stop, flag_yield;
	/*sleep*/
	time_t seconds;
	/*Branches*/
	int *max_pc;
	
	/*gia thn print*/
	char *global_memory;
	
	int c[N];
	int begin[4] = {0xde, 0xad, 0xbe, 0xaf};
	int body[4] = {0xde, 0xad, 0xc0, 0xde};
	int task[4] = {0xde, 0xad, 0xba, 0xbe};
	int end[4] = {0xfe, 0xe1, 0xde, 0xad};
	struct global_state glb;
	
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
				free(glb.tasks[i].local_Mem);
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
				free(glb.tasks[i].local_Mem);
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
				free(glb.tasks[i].local_Mem);
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
				free(glb.tasks[i].local_Mem);
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
			free(glb.tasks[i].local_Mem);
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
			free(glb.tasks[i].local_Mem);
		}
		free(global_memory);
		return(-1);
	}
	
	/*boithitikos pinakas gia ta Branches oste na elegxoume to mexri poso mporei na paei to pc*/
	max_pc[0] = code_size[0];
	for(i=1;i<num_of_bodies;i++){
		max_pc[i] = max_pc[i-1] + code_size[i];
	}
	
	glb.cur = -1;
	p=-1;
	/*epanalhyh mexri na termatistei to programma tou xrhsth*/
	while(1) {
		count_stop = 0;
		count_blc = 0;
		count_slp = 0;
		
		for(p=0;p<num_of_tasks;p++){
			/*ELEGXOS KATASTASHS TWN TASKS*/
			if(p>glb.cur && (strcmp(glb.tasks[p].state,"READY")==0)){
				glb.cur = p;
				break;
			}
			else if(strcmp(glb.tasks[p].state,"STOPPED")==0){
				count_stop++;
			}
			else if((strcmp(glb.tasks[p].state,"SLEEPING")==0)){
				//elegxos ean exei perasei o xronos toy task to kanoume ready pairnei ton epeksergasth kai ayksanoume kai ton counter tou ready
				seconds = time(NULL);
				
				if(seconds>=glb.tasks[p].waket){
					glb.tasks[p].waket = -1;
					glb.cur = p;
					strcpy(glb.tasks[p].state,"READY");
					break;
				}
				count_slp++;
			}
			else if(strcmp(glb.tasks[p].state,"BLOCKED")==0){
				count_blc++;
			}
		}
		
		i = glb.cur;
		/*arxikopoihsh tou glb.cur gia thn epomenh epanalhyh*/
		if(glb.cur==(num_of_tasks-1)){
			glb.cur = -1;
		}
		/*elegxos stis periptwseis pou den uparxei ready*/
		else if((count_blc+count_stop+count_slp)==num_of_tasks){
			/*an uparxei kapoio sleep sunexizei h ektelesh*/
			if(count_slp>0){
				glb.cur = -1;
				continue;
			}
			/*se periptwsh pou uparxoun task pou den exoun termatisei*/
			else if(count_stop!=num_of_tasks){
				printf("Segmentation fault!!\n");
				close(fd_hex);
				free(glb.global_mem);
				free(locals_size);
				free(code_size);
				free(glb.code);
				free(glb.tasks);
				free(task_body);
				free(task_arg);
				for(i = 0; i<num_of_tasks; i++) {
					free(glb.tasks[i].local_Mem);
				}
				free(global_memory);
				free(max_pc);
				return(-1);
			}
			else{
				break;
			}
		}
		else if(p==num_of_tasks){
			glb.cur = -1;
			continue;
		}
		
		flag_yield = 0;
		//k ta bytes tou kwdika kai i ta task
// 		printf("The task %d has the processor \n\n",i);
		for(j=0;j<N_exec;j++){
			/*gia na proxwrhsei to k edolh*/
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
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)1\n");
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]+glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 11:  //SUB
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)2\n");
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]-glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 12: //MUL
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)3\n");
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]*glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 13:  //DIV
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)4\n");
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]/glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				case 14:  //MOD
					if(glb.code[k+2]>N_reg) {
						printf("error: reg%d undeclared\n",glb.code[k+2]);
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					else if(glb.code[k+2]<0){
						printf("Segmentation fault (core dumped)5\n");
						close(fd_hex);
						free(glb.global_mem);
						free(locals_size);
						free(code_size);
						free(glb.code);
						free(glb.tasks);
						free(task_body);
						free(task_arg);
						for(i = 0; i<num_of_tasks; i++) {
							free(glb.tasks[i].local_Mem);
						}
						free(global_memory);
						free(max_pc);
						return(-1);
					}
					glb.tasks[i].reg[glb.code[k+1]]=glb.tasks[i].reg[glb.code[k+1]]%glb.tasks[i].reg[glb.code[k+2]];
					glb.tasks[i].pc++;
					break;
				/*Branches*/
				case 15:  //BRGZ
					/*elegxos gia arnhtikous arithmous*/
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					if (glb.tasks[i].reg[glb.code[k+1]] > 0) {
						/*an mas dinei kapoio offset pou ksepernaei thn uparxousa mnhmh*/
						if( (glb.tasks[i].pc+temp*3)>max_pc[task_body[i]-1]) {
							printf("Segmentation fault!1\n");
							close(fd_hex);
							free(glb.global_mem);
							free(locals_size);
							free(code_size);
							free(glb.code);
							free(glb.tasks);
							free(task_body);
							free(task_arg);
							for(i = 0; i<num_of_tasks; i++) {
								free(glb.tasks[i].local_Mem);
							}
							free(global_memory);
							free(max_pc);
							return(-1);
						}
						glb.tasks[i].pc= glb.tasks[i].pc+temp;
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 16:  //BRGEZ
					/*elegxos gia arnhtikous arithmous*/
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]] >=0) {
						/*an mas dinei kapoio offset pou ksepernaei thn uparxousa mnhmh*/
						if( (glb.tasks[i].pc+temp)*3> max_pc[task_body[i]-1]) {
							printf("Segmentation fault!2\n");
							close(fd_hex);
							free(glb.global_mem);
							free(locals_size);
							free(code_size);
							free(glb.code);
							free(glb.tasks);
							free(task_body);
							free(task_arg);
							for(i = 0; i<num_of_tasks; i++) {
								free(glb.tasks[i].local_Mem);
							}
							free(global_memory);
							free(max_pc);
							return(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp;
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 17:  //BRLZ
					/*elegxos gia arnhtikous arithmous*/
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]]  < 0) {
						/*an mas dinei kapoio offset pou ksepernaei thn uparxousa mnhmh*/
						if( (glb.tasks[i].pc+temp)*3> max_pc[task_body[i]-1]) {
							printf("Segmentation fault!3\n");
							close(fd_hex);
							free(glb.global_mem);
							free(locals_size);
							free(code_size);
							free(glb.code);
							free(glb.tasks);
							free(task_body);
							free(task_arg);
							for(i = 0; i<num_of_tasks; i++) {
								free(glb.tasks[i].local_Mem);
							}
							free(global_memory);
							free(max_pc);
							return(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp; 
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 18:  //BRLEZ
					/*elegxos gia arnhtikous arithmous*/
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]] <= 0) {
						/*an mas dinei kapoio offset pou ksepernaei thn uparxousa mnhmh*/
						if( (glb.tasks[i].pc+temp)*3>max_pc[task_body[i]-1]) {
							printf("Segmentation fault!4\n");
							close(fd_hex);
							free(glb.global_mem);
							free(locals_size);
							free(code_size);
							free(glb.code);
							free(glb.tasks);
							free(task_body);
							free(task_arg);
							for(i = 0; i<num_of_tasks; i++) {
								free(glb.tasks[i].local_Mem);
							}
							free(global_memory);
							free(max_pc);
							return(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp;
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 19:  //BREZ
					/*elegxos gia arnhtikous arithmous*/
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if (glb.tasks[i].reg[glb.code[k+1]] == 0) {
						/*an mas dinei kapoio offset pou ksepernaei thn uparxousa mnhmh*/
						if( (glb.tasks[i].pc+temp)*3>max_pc[task_body[i]-1]) {
							printf("Segmentation fault!5\n");
							close(fd_hex);
							free(glb.global_mem);
							free(locals_size);
							free(code_size);
							free(glb.code);
							free(glb.tasks);
							free(task_body);
							free(task_arg);
							for(i = 0; i<num_of_tasks; i++) {
								free(glb.tasks[i].local_Mem);
							}
							free(global_memory);
							free(max_pc);
							return(-1);
						}
						
						glb.tasks[i].pc= glb.tasks[i].pc+temp; 
						break;
					}
					glb.tasks[i].pc++;
					break;
				case 20:  //BRA
					/*elegxos gia arnhtikous arithmous*/
					if(glb.code[k+2]>=0x80){
						temp = glb.code[k+2] - 256;
					}
					else {
						temp = glb.code[k+2];
					}
					
					if( (glb.tasks[i].pc+temp)*3> max_pc[task_body[i]-1]) {
						printf("Segmentation fault!6\n");
						return(-1);
					}
					
					glb.tasks[i].pc= glb.tasks[i].pc+temp; 
					break;
					
				/*synch*/
				case 21:  //DOWN
					glb.global_mem[glb.code[k+2]]--;
					if (glb.global_mem[glb.code[k+2]] < 0) {
						strcpy(glb.tasks[i].state,"BLOCKED"); 
						glb.tasks[i].sem = glb.code[k+2];
						flag_yield = 1;
					}
					glb.tasks[i].pc++;
					break;
				case 22:  //UP
					glb.global_mem[glb.code[k+2]]++;
					if (glb.global_mem[glb.code[k+2]] <=0) {
						for(temp = 0; temp<num_of_tasks; temp++) {
							/*find blocked*/
							if(glb.tasks[temp].sem ==  glb.code[k+2]) {
								strcpy(glb.tasks[temp].state,"READY");
								glb.tasks[temp].sem = -1;
								break;
							}
						}
					}
					glb.tasks[i].pc++;
					break;
					
				/*Varia*/
				case 23:  //YIELD
					flag_yield = 1;
					glb.tasks[i].pc++;
					break;
				case 24:  //SLEEP
					strcpy(glb.tasks[i].state,"SLEEPING");
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
					strcpy(glb.tasks[i].state, "STOPPED");
					glb.tasks[i].pc++;
					flag_yield = 1;
					break;
					
				default:
					printf("error:%d pc:%d task %d undeclared\n", glb.code[k-3], glb.tasks[i].pc, i);
					glb.tasks[i].pc++;
					close(fd_hex);
					free(glb.global_mem);
					free(locals_size);
					free(code_size);
					free(glb.code);
					free(glb.tasks);
					free(task_body);
					free(task_arg);
					for(i = 0; i<num_of_tasks; i++) {
						free(glb.tasks[i].local_Mem);
					}
					free(global_memory);
					free(max_pc);
					return(-1);
			}
			/*flag_yield =1 otan prepei n dwthei o epeksergasths se allo task*/
			if(flag_yield == 1){
				break;
			}
		}
	}
	
	
	close(fd_hex);
	free(glb.global_mem);
	free(locals_size);
	free(code_size);
	free(glb.code);
	free(task_body);
	free(task_arg);
	for(i = 0; i<num_of_tasks; i++) {
		free(glb.tasks[i].local_Mem);
	}
	free(glb.tasks);
	free(global_memory);
	free(max_pc);
	return (0);
}

