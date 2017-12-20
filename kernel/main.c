#include "common.h"
#include "x86.h"
#include "device.h"
#include "pcb.h"
#include "clk_time.h"
#define OFFSET 8
#define LOC 80

extern void enter_user_space(void);
extern void init_timer();
void load_file();
 void printf(const char *format,...);
extern int row;
extern int line;

void
kentry(void) {
	init_serial();			//初始化串口输出
    init_pcb();
	init_idt();
	init_intr();
	init_timer();
    init_seg();
    load_umain();
	load_file();
    //enter_user_space();
   printf("Kernel has finished initialized!\n");
	enable_interrupt();
	char c='^';
	int pos=0;
    while(1) {
			wait_for_interrupt();
			
			putchar('^');
			pos = (row * LOC + line) * 2;
			asm volatile("movw %%ax, %%gs:(%%edi)"::"D"(pos),"a"(c+(0xc << OFFSET)));
			row += (++line) / LOC;
			line %= LOC;			
	}
	assert(0);
}

