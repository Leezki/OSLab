#include "common.h"
#include "x86.h"
#include "device.h"
#include "pcb.h"
#include "semaphore.h"
#include "fs.h"

void do_syscall(struct TrapFrame *tf);
extern void putchar(char);
 void printf(const char *format,...);
int row = 0;
int line = 0;


inline void SaveReg(struct TrapFrame *tf){
	current->regs.eax = tf->eax;
	current->regs.ebx = tf->ebx;
	current->regs.ecx = tf->ecx;
	current->regs.edx = tf->edx;
	current->regs.esi = tf->esi;
	current->regs.edi = tf->edi;
	current->regs.ebp = tf->ebp;
}

void
irq_handle(struct TrapFrame *tf) {
	if(tf->irq <= 19) {
	}
	else if (tf->irq  == 0x80) {
			do_syscall(tf);
	}
    else if (tf->irq  >= 1000) {
		switch(tf->irq) {
		    case 1000: {
			current->time_count--;
			de_sleep_time();
			if (current->state == BLOCKED || current->state == DEAD || current->time_count <= 0) {
				SaveReg(tf);
				schedule();
	 	 	}}
			break;
		    case 1001:break;
			case 1014:break;
            default:assert(0);
	 	} 
	} 
	else {
		assert(0);
	} 
}

void do_syscall(struct TrapFrame *tf) {
	switch(tf->eax){	
		case 4:{
		char *buf = (void *)(tf->ecx + pcb_table[current->pid].bases);
		if (tf->ebx == 1 || tf->ebx == 2) {
			int i = 1;i--;
			for (; i < tf->edx; ++i) {
				putchar(*(buf + i));
				if(buf[i]=='\n'){
					line=2;	
					row ++;
					line-=2;
				}
				else{
								int pos = (row * 80 + line) * 2;
								asm volatile("movw %%ax, %%gs:(%%edi)"::"D"(pos),"a"(buf[i]+(0xc << 8)));
								line +=2;line--;
								row += line / 80;
								line %= 80;
							}
				}
			
			tf->eax = tf->edx;
		}
		else 
			assert(0);
		}break;

		case 7:{
			uint32_t sleep_time = tf->ebx;
			nanosleep(sleep_time);
 		}break;
		case 2:{				
			SaveReg(tf);		
			k_fork();
			tf->eax = cur_pcb_id;
		}break;
		case 1:{
			int id = tf->ebx;
			k_exit(id);
  		}break;
		case 10:{
			int sem_id=1;
			sem_table[sem_max].value = tf->ebx;
			sem_table[sem_max].pcb_num = 0;
			tf->eax = sem_max;
			sem_max+=sem_id;
		}break;
		case 11:{
			destroySem(tf->ebx);
		}break;
		case 8:{
			SaveReg(tf);
			int sem_no = tf->edx;
			int sem_id = tf->ebx;
			if (sem_table[sem_no].value==0){
			}	
			sem_table[sem_no].value--;
			sem_id = 0;
			if(sem_table[sem_no].value < sem_id){
			//	putchar('W');
				W(&sem_table[sem_no]);
			}
		}break;
		case 9:{
			int sem_no = tf->ebx;
			int sem_id = tf->edx;
			sem_table[sem_no].value++;
			sem_id = 0;
			if (sem_table[sem_no].value <= sem_id){
				R(&sem_table[sem_no]);
			//	putchar('R');
			} 
		}break;
		case 3:fs_read(tf->ebx,(void*)(tf->ecx+0x3000000),tf->edx);break;
		case 19:fs_lseek(tf->ebx,tf->ecx,tf->edx);break;
		case 5:printf("%s\n",(char*)tf->ecx);tf->eax=fs_open((char*)(tf->ebx+0x3000000));break;
		case 6:fs_close(tf->ebx);break;
		case 15:fs_write(tf->ebx,(void*)(tf->ecx+0x3000000),tf->edx);
		default:assert(0);
	}
}
