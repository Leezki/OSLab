#include "x86.h"
#include "device.h"
#include "pcb.h"

#define USER_STACK 0x20000
#define USER_START 0x200000	
#define SECTSIZE 512
uint32_t stk[1024];
SegDesc gdt[NR_SEGMENTS];   
TSS tss;
void readseg(unsigned char *paddr, int count, int offset);
void readsect(void *dst, int offset);

void
init_seg() {
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
	gdt[SEG_VIDEO] = SEG(STA_W,   0xB8000, 		 0xffffffff, DPL_KERN);
    gdt[SEG_TSS].s = 0;
	set_gdt(gdt, sizeof(gdt));

    /*
	 * 初始化TSS
	 */ 
 tss.esp0 = (uint32_t)&stk + sizeof(stk);
 //	asm volatile("movl %%esp,%0" : "=m" (tss.esp0));
	tss.ss0 = KSEL(SEG_KDATA);
	asm volatile("ltr %%ax" :: "a"(KSEL(SEG_TSS)));

	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));
    asm volatile("movw %%ax,%%gs":: "a" (KSEL(SEG_VIDEO)));

	lldt(0);
}

void
load_umain(void) {
    /*
     * Load your app here
     * 加载用户程序
      */
	struct ELFHeader *elf;
   	struct ProgramHeader *ph, *eph; 
	unsigned char *pa,*i;
	cur_pcb_id++;
	elf = (struct ELFHeader *)0x400000;
	readseg((void *)elf, 4096, 0);
	ph = (struct ProgramHeader *)((char *)elf + elf->phoff);
	eph = ph + elf->phnum;
	for(;ph < eph; ph++){
		if(ph->type == 1){
			pa = (unsigned char *)ph->paddr;
			readseg(pa, ph->filesz, ph->off);
			for(i = pa + ph->filesz; i < pa + ph->memsz; *i ++ = 0);
		}
	}

	(pcb_table+cur_pcb_id)->regs.gs = USEL(SEG_VIDEO);
	(pcb_table+cur_pcb_id)->regs.fs = USEL(SEG_UDATA);
	(pcb_table+cur_pcb_id)->regs.es = USEL(SEG_UDATA);
	(pcb_table+cur_pcb_id)->regs.ss = USEL(SEG_UDATA);
	(pcb_table+cur_pcb_id)->regs.ds = USEL(SEG_UDATA);
	(pcb_table+cur_pcb_id)->regs.cs = USEL(SEG_UCODE);
	(pcb_table+cur_pcb_id)->regs.eflags = 0x202;

	(pcb_table+cur_pcb_id)->regs.eip = USER_START;
	(pcb_table+cur_pcb_id)->regs.esp = USER_START + USER_STACK;

	(pcb_table+cur_pcb_id)->state = RUNNABLE;
	(pcb_table+cur_pcb_id)->time_count = DEFAULT_TIME_COUNT;
	current = pcb_table;
	current->regs.ss = KSEL(SEG_KDATA);
	current->state = RUNNABLE;
	current->time_count = DEFAULT_TIME_COUNT;
	tss.esp0 = (uint32_t)(current) + sizeof(stackframe);
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); 
}

void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte2(0x1F2, 1);
	out_byte2(0x1F3, offset);
	out_byte2(0x1F4, offset >> 8);
	out_byte2(0x1F5, offset >> 16);
	out_byte2(0x1F6, (offset >> 24) | 0xE0);
	out_byte2(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}
void readseg(unsigned char *paddr, int count, int offset) {                 
	unsigned char *epaddr;
	epaddr = paddr + count;
	paddr -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 201;
	for(; paddr < epaddr; paddr += SECTSIZE, offset ++)
		readsect(paddr, offset);
}


