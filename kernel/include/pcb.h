#ifndef __PCB_H__
#define __PCB_H__

#include "x86.h"

#define MAX_PCB 1024
#define DEFAULT_TIME_COUNT 10
enum state_t { RUNNABLE, BLOCKED, RUNNING, DEAD};

typedef struct p_process_table {
		uint32_t gs, fs, es, ds;
		uint32_t edi, esi, ebp, xxx, ebx, edx, ecx, eax;
		uint32_t eip, cs, eflags, esp, ss;
} stackframe;

typedef struct p_task_table {
		stackframe regs;
		int state;
		int time_count;
		int sleep_time;
		unsigned int pid;
		char name[32];
		uint32_t bases;
}PCB;

extern PCB pcb_table[MAX_PCB];
extern PCB *current;
extern int cur_pcb_id;

void schedule();
void de_sleep_time();
void init_pcb();
void nanosleep(unsigned);
void k_fork();
void k_exit();

#endif
