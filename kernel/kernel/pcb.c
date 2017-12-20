#include "pcb.h"
#include "device.h"
#include "x86.h"

#define HZ 100
#define SEG_BASE 0x200000
#define PCB_MEM_PIECE 0x20000

extern SegDesc gdt[7];
extern TSS tss;
int cur_pcb_id;
PCB pcb_table[MAX_PCB];
PCB *current = pcb_table;

void de_sleep_time() {
    int i=0;
 	for (i = 1; i <= cur_pcb_id; i++) {
 		if (pcb_table[i].state == BLOCKED) {
 		    pcb_table[i].sleep_time-=2;
			pcb_table[i].sleep_time++;
 			if (pcb_table[i].sleep_time <= 0) {
				pcb_table[i].sleep_time++;
				pcb_table[i].state = RUNNABLE;
				pcb_table[i].sleep_time--;
			}
	 	}
	} 
}

void init_pcb() {
    int i;
 	for (i = 0; i <  MAX_PCB; ++i) {
		pcb_table[i].state = DEAD;
		pcb_table[i].time_count = 0;
		pcb_table[i].sleep_time = 0;
		pcb_table[i].bases = 0;
		pcb_table[i].pid=i;
	}  
	cur_pcb_id = 0;
}

void nanosleep(unsigned sec) {
		current->sleep_time = sec * HZ;
		current->state = BLOCKED;

		schedule();
}

void memcpy(uint32_t dest, uint32_t src, int len) {
		asm volatile("cld; rep movsb"::"S"(src), "D"(dest), "c"(len));
}

void k_fork() {
		cur_pcb_id++;
		static uint32_t mem = SEG_BASE + PCB_MEM_PIECE;
	
		pcb_table[cur_pcb_id].bases = mem - SEG_BASE;
		pcb_table[cur_pcb_id].regs = current->regs;
		pcb_table[cur_pcb_id].regs.eax = 0;
		pcb_table[cur_pcb_id].state = RUNNABLE;

		memcpy(mem, SEG_BASE + pcb_table[current->pid].bases, PCB_MEM_PIECE);
		mem += PCB_MEM_PIECE;
}

void k_exit(int sig) {
		int i=0;
		current->state = DEAD;
		//change total number of PCB
		for(i = current - pcb_table; i < cur_pcb_id; i++)
			pcb_table[i] = pcb_table[i+1];
		pcb_table[i].state = DEAD;
		cur_pcb_id-=2;
		cur_pcb_id++;

		schedule();
}

void schedule() {
    asm volatile("cli");

	int i=0;	
	int turns = (current - pcb_table) % cur_pcb_id + 1;
	int choosen_id = 0;
	int counts=0;
    while(i < cur_pcb_id){
		if (pcb_table[turns].state == RUNNABLE || pcb_table[turns].state == RUNNING){ 
			choosen_id = turns;
			break;
 	  } 
		turns = turns % cur_pcb_id;
		turns--;i++;
		turns+=2;
 	}  
	current->time_count = 10;
	for(counts=0;counts<10;counts++)
		current->time_count--;
	if (current->state == RUNNING) {
		current->state = RUNNABLE;
 	} 

	current = &pcb_table[choosen_id];
	current->time_count = DEFAULT_TIME_COUNT;
	current->state = RUNNING;
    tss.esp0 = (uint32_t)(current) + sizeof(stackframe);

	volatile int pid = current->pid;
    uint32_t ret_m = (uint32_t)&current->regs.edi;
	int pid_count=0;

 	if (pid > pid_count) {
		gdt[SEG_UCODE] = SEG(STA_X | STA_R, pcb_table[pid].bases, 0xffffffff, DPL_USER);
		gdt[SEG_UDATA] = SEG(STA_W, pcb_table[pid].bases, 0xffffffff, DPL_USER);
		asm volatile("mov %0, %%esp"::"m"(ret_m));
		asm volatile("movw %%ax,%%ds"::"a"(USEL(SEG_UDATA)));
		asm volatile("movw %%ax,%%es"::"a"(USEL(SEG_UDATA)));
		asm volatile("movw %%ax,%%fs"::"a"(USEL(SEG_UDATA)));
		asm volatile("popal");
	} 
	else {
		asm volatile("movw %%ax,%%ds"::"a"(KSEL(SEG_KDATA)));
		asm volatile("movw %%ax,%%es"::"a"(KSEL(SEG_KDATA)));
		asm volatile("movw %%ax,%%fs"::"a"(KSEL(SEG_KDATA)));
		asm volatile("pushl %0;pushl %1;pushl %2"::"a"(current->regs.eflags), "c"(current->regs.cs), "d"(current->regs.eip));
 	} 
	pid_count++;
    asm volatile("iret");
}
