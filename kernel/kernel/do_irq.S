# 中断和异常处理函数的入口
# 中断处理函数会在IDT中为相应的中断/异常设置处理程序
# 中断/异常的行为参见i386手册

.globl   vec0;   vec0: mov $0x1ff000,%esp;    pushl $0;   jmp asm_do_irq
.globl   vec1;   vec1: mov $0x1ff000,%esp;    pushl $1;   jmp asm_do_irq
.globl   vec2;   vec2: mov $0x1ff000,%esp;    pushl $2;   jmp asm_do_irq
.globl   vec3;   vec3: mov $0x1ff000,%esp;    pushl $3;   jmp asm_do_irq
.globl   vec4;   vec4: mov $0x1ff000,%esp;    pushl $4;   jmp asm_do_irq
.globl   vec5;   vec5: mov $0x1ff000,%esp;    pushl $5;   jmp asm_do_irq
.globl   vec6;   vec6: mov $0x1ff000,%esp;    pushl $6;   jmp asm_do_irq
.globl   vec7;   vec7: mov $0x1ff000,%esp;    pushl $7;   jmp asm_do_irq
.globl   vec8;   vec8: mov $0x1ff000,%esp;    pushl $8;   jmp asm_do_irq
.globl   vec9;   vec9: mov $0x1ff000,%esp;    pushl $9;   jmp asm_do_irq
.globl  vec10;  vec10: mov $0x1ff000,%esp;   pushl $10;   jmp asm_do_irq
.globl  vec11;  vec11: mov $0x1ff000,%esp;   pushl $11;   jmp asm_do_irq
.globl  vec12;  vec12: mov $0x1ff000,%esp;   pushl $12;   jmp asm_do_irq
.globl  vec13;  vec13: mov $0x1ff000,%esp;   pushl $13;   jmp asm_do_irq
.globl  vec14;  vec14: mov $0x1ff000,%esp;   pushl $14;   jmp asm_do_irq
.globl  vec15;  vec15: mov $0x1ff000,%esp;   pushl $15;   jmp asm_do_irq
.globl  vec16;  vec16: mov $0x1ff000,%esp;   pushl $16;   jmp asm_do_irq
.globl  vec17;  vec17: mov $0x1ff000,%esp;   pushl $17;   jmp asm_do_irq
.globl  vec18;  vec18: mov $0x1ff000,%esp;   pushl $18;   jmp asm_do_irq
.globl  vec19;  vec19: mov $0x1ff000,%esp;   pushl $19;   jmp asm_do_irq
.globl  vecsys; vecsys: mov $0x1ff000,%esp; pushl $0x80;   jmp asm_do_irq
.globl  irq_empty; irq_empty: pushl $-1; jmp asm_do_irq

.globl  test1;  test1: pushl $1000;   jmp asm_do_irq
.globl  test2;  test2: pushl $1001;   jmp asm_do_irq

.globl irq0;  
irq0:cmpl $0x1e0000,%esp; jl ul_0; push $1000; jmp kernel_irq; 
ul_0:  mov $0x1ff000,%esp;  push $1000;  jmp asm_do_irq
.globl irq1;  
irq1:cmpl $0x1e0000,%esp; jl ul_1; push $1001; jmp kernel_irq; 
ul_1:  mov $0x1ff000,%esp;  push $1001;  jmp asm_do_irq
.globl irq14;  
irq14:cmpl $0x1e0000,%esp; jl ul_14; push $1014; jmp kernel_irq; 
ul_14:  mov $0x1ff000,%esp;  push $1014;  jmp asm_do_irq


# ！调用C编写的中断处理程序(见irq/irq_handle.c)！
.globl asm_do_irq
.extern irq_handle
# 在 ring3 用户空间下发生中断时需保存现场信息, 而陷入内核时要首先设置正确的段寄存器
asm_do_irq:

	pushal

	pushl %esp
	movw $16,%ax
	movw %ax,%ds
	movw $48,%ax
	movw %ax,%gs
	call irq_handle
	addl $4, %esp

	movw $35,%ax
	movw %ax,%ds
	popal

	addl $4, %esp

	mov $current, %esp
	mov (%esp), %esp
	add $0x30, %esp
	
	iret

.globl kernel_irq
kernel_irq:
    pushal
	movw $16,%ax
	movw %ax,%ds
	cmp $0x1f0000, %esp
	jg kernel_stack

	lea 0x24(%esp), %eax
	mov (%eax),%eax
	movl $pcb_table, %edx
	add $0x30, %edx
	mov %eax, (%edx)
	lea 0x28(%esp), %eax
	mov (%eax),%eax
	mov %eax, 0x4(%edx)
	lea 0x2c(%esp), %eax
	mov (%eax),%eax
	mov %eax, 0x8(%edx)
	lea 0x30(%esp), %eax
	mov %eax, 0xc(%edx)

	kernel_stack:
	push %esp

	call irq_handle
	add $4, %esp
	popal
	add $4, %esp
	iret

