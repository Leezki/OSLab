#include "x86.h"
#include "device.h"

#define INTERRUPT_GATE_32   0xE
#define TRAP_GATE_32        0xF

/* IDT表的内容 */
struct GateDescriptor idt[NR_IRQ];

/* 初始化一个中断门(interrupt gate) */
static void
set_intr(struct GateDescriptor *ptr, uint32_t selector, uint32_t offset, uint32_t dpl) {
	ptr->offset_15_0 = offset & 0xFFFF;
	ptr->segment = selector << 3;
	ptr->pad0 = 0;
	ptr->type = INTERRUPT_GATE_32;
	ptr->system = FALSE;
	ptr->privilege_level = dpl;
	ptr->present = TRUE;
	ptr->offset_31_16 = (offset >> 16) & 0xFFFF;
}

/* 初始化一个陷阱门(trap gate) */
static void
set_trap(struct GateDescriptor *ptr, uint32_t selector, uint32_t offset, uint32_t dpl) {
	ptr->offset_15_0 = offset & 0xFFFF;
	ptr->segment = selector << 3;
	ptr->pad0 = 0;
	ptr->type = TRAP_GATE_32;
	ptr->system = FALSE;
	ptr->privilege_level = dpl;
	ptr->present = TRUE;
	ptr->offset_31_16 = (offset >> 16) & 0xFFFF;
}

/* 声明函数，这些函数在汇编代码里定义 */
void test1();
void test2();
void irq_empty();

void vec0();
void vec1();
void vec2();
void vec3();
void vec4();
void vec5();
void vec6();
void vec7();
void vec8();
void vec9();
void vec10();
void vec11();
void vec12();
void vec13();
void vec14();
void vec15();
void vec16();
void vec17();
void vec18();
void vec19();
void vecsys();
void irq0();
void irq1();
void irq14();


void init_idt() {
	int i;
	/* 为了防止系统异常终止，所有irq都有处理函数(irq_empty)。 */
	for (i = 0; i < NR_IRQ; i ++) {
		set_trap(idt + i, SEG_KCODE, (uint32_t)irq_empty, DPL_KERN);
	}
    /*
     * init your idt here
     * 初始化 IDT 表, 为中断设置中断处理函数
     */
	set_trap(idt + 0, SEG_KCODE, (uint32_t)vec0, DPL_USER);
	set_trap(idt + 1, SEG_KCODE, (uint32_t)vec1, DPL_USER);
	set_trap(idt + 2, SEG_KCODE, (uint32_t)vec2, DPL_USER);
	set_trap(idt + 3, SEG_KCODE, (uint32_t)vec3, DPL_USER);
	set_trap(idt + 4, SEG_KCODE, (uint32_t)vec4, DPL_USER);
	set_trap(idt + 5, SEG_KCODE, (uint32_t)vec5, DPL_USER);
	set_trap(idt + 6, SEG_KCODE, (uint32_t)vec6, DPL_USER);
	set_trap(idt + 7, SEG_KCODE, (uint32_t)vec7, DPL_USER);
	set_trap(idt + 8, SEG_KCODE, (uint32_t)vec8, DPL_USER);
	set_trap(idt + 9, SEG_KCODE, (uint32_t)vec9, DPL_USER);
	set_trap(idt + 10, SEG_KCODE, (uint32_t)vec10, DPL_USER);
	set_trap(idt + 11, SEG_KCODE, (uint32_t)vec11, DPL_USER);
	set_trap(idt + 12, SEG_KCODE, (uint32_t)vec12, DPL_USER);
	set_trap(idt + 13, SEG_KCODE, (uint32_t)vec13, DPL_USER);
	set_trap(idt + 14, SEG_KCODE, (uint32_t)vec14, DPL_USER);
	set_trap(idt + 15, SEG_KCODE, (uint32_t)vec15, DPL_USER);
	set_trap(idt + 16, SEG_KCODE, (uint32_t)vec16, DPL_USER);
	set_trap(idt + 17, SEG_KCODE, (uint32_t)vec17, DPL_USER);
	set_trap(idt + 18, SEG_KCODE, (uint32_t)vec18, DPL_USER);
	set_trap(idt + 19, SEG_KCODE, (uint32_t)vec19, DPL_USER);
	set_trap(idt + 0x80, SEG_KCODE, (uint32_t)vecsys, DPL_USER);

    set_intr(idt + 0x41, SEG_KCODE, (uint32_t)test1, DPL_KERN);
    set_intr(idt + 0x42, SEG_KCODE, (uint32_t)test2, DPL_USER);
    set_intr(idt + 0x20 + 0, SEG_KCODE, (uint32_t)irq0, DPL_KERN);
    set_intr(idt + 0x20 + 1, SEG_KCODE, (uint32_t)irq1, DPL_USER);
    set_intr(idt + 0x20 + 14, SEG_KCODE, (uint32_t)irq14, DPL_USER);

	/* 写入IDT */
	save_idt(idt, sizeof(idt));
}

