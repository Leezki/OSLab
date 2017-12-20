# 系统启动时，BIOS会完成初始化，然后将磁盘第一个512字节保存到物理内存0x00007C00
# 位置，然后设置%CS=0000h, %IP=7C00h

#include "asm.h"
# 从此开始是16位代码
.code16
.globl start
start:	
	#Disable interrupts
	cli						
	#Open A20
	mov		$0x2401, %ax
	int		$0x15
	#Load gdt
	lgdt	gdtdesc
	#PE=1
	mov		%cr0, %eax
	or		$0x1, %eax
	mov		%eax, %cr0
	#jmp to pretection mode
	ljmp	$GDT_ENTRY(1), $start32
.loop:
    jmp .loop


.code32
start32:
	movw	$GDT_ENTRY(2), %ax
	mov		%ax, %ds
	mov		%ax, %ss
	movw	$GDT_ENTRY(3), %ax
	mov		%ax, %gs
    mov		$load, %esi
	movl	$0x0400, %ecx
	call	my_print
	call	bootmain
.loop32:
    jmp .loop32

#hello:      .string "Boot: Hello, World!"     #字符串内容
load:		.string "Loading..."     #字符串内容
.globl execute 
execute:	.string "Execute..."     #字符串内容
.globl kernel 
kernel:		.string "Kernel..."     #字符串内容
.globl back
back:		.string "Back..."

.globl my_print 
my_print:
	push	%edi
	push	%ebx
	movzx	%ch, %ebx
	movl	%ebx, %edx
	shl		$6, %ebx
	shl		$4, %edx
	add		%edx, %ebx
	movzx	%cl, %edi
	add		%ebx, %edi
	shl		$1, %edi
	movb	$0x0c, %ah
	l1:
	movb	0x0(%esi), %al
	testb	%al, %al
	jz l2
	movw	%ax, %gs:(%edi)
	add		$2, %edi
	add		$1, %esi
	jmp		l1
	l2:
	pop		%ebx
	pop		%edi
	ret



# GDT
.p2align 2                         # 对齐
    #GDT definition here
gdt:    #NULL
    .word    0,0
    .byte    0,0,0,0
	SEG_ASM(0x0a, 0x0, 0xffffffff)    # CS
	SEG_ASM(0x02, 0x0, 0xffffffff)    # DS 
	SEG_ASM(0x02, 0xb8000, 0xffffffff)# GS

gdtdesc:                           # GDT描述符
	.word   (gdtdesc - gdt - 1)    # GDT长度，留意地址运算
	.long   gdt                    # GDT地址

