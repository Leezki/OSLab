#include "boot.h"

#define SECTSIZE 512
#define uint32_t unsigned long

void readsect(void *dst, int offset);	
void readseg(uint32_t offset, uint32_t *paddr, int num);	


void bootmain(void){
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;

	/* 这里是加载磁盘程序的代码 */
	elf = (struct ELFHeader *)0x8000;
	readseg(1, (uint32_t*)elf, 200);
	ph = (struct ProgramHeader *)((uint32_t)elf + elf->phoff);

	short num = elf->phnum;
	for (; num > 0; --num, ++ph) {
		uint32_t offset = (uint32_t)elf + ph->off;
		asm volatile("mov %%ds, %%ax; mov %%ax, %%es":::"%eax");
		asm volatile("cld; rep movsb"::"c"(ph->filesz), "D"(ph->paddr), "S"(offset));
		asm volatile("mov %0, %%ecx"::"m"(ph->filesz));
		asm volatile("cld; rep stosb"::"c"(ph->memsz - ph->filesz), "a"(0));
	}
	asm volatile("pushl %esp; movl $0x1f0000, %esp");
	((void(*)(void))elf->entry)();
	asm volatile("popl %esp");
}

void readseg(uint32_t offset, uint32_t *paddr, int num) {
	for (; num > 0; num --, offset++) {
		readsect(paddr,offset);
		paddr += SECTSIZE/sizeof(uint32_t);
	}
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* 等待磁盘完毕 */
}

/* 读磁盘的一个扇区 */
void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	} 
}
