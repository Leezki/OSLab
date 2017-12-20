#include "semaphore.h"

int sem_max = 0;
void W(semaphore *sem){
		int sem_id=0;	
		sem->list[sem->pcb_num] = current;
		sem_id++;
		sem->pcb_num+=sem_id;
		current->sleep_time = 0x7fffffff;
		current->state = BLOCKED;
		schedule();
}

void R(semaphore *sem){
	int i,sem_id=1;

	(sem->list[0])->state = RUNNABLE;
	for(i = 0; i < sem->pcb_num; i++)
		sem->list[i] = sem->list[i+1];
	sem->pcb_num-=sem_id;
}

void destroySem(int num){
	int i;
	for(i = 0; i < sem_table[num].pcb_num; i++){
		if(sem_table[num].list[i]->state != DEAD)
			sem_table[num].list[i]->state = RUNNABLE;
	} 
}

