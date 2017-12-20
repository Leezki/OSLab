#ifndef _SEM_H_
#define _SEM_H_

#include "pcb.h"

typedef struct s_semaphore{
	int value;
	int pcb_num;
	char state;
	PCB *list[MAX_PCB];
} semaphore;

semaphore sem_table[10];
extern int sem_max;

void W(semaphore* sem);
void R(semaphore* sem);
void destroySem(int num);


#endif

