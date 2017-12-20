#include "lib.h"
#include "types.h"
#include "common.h"

/*
 * io lib here
 * 库函数写在这
 */
void p_str(char** buf, char *str);
void p_ch(char** buf, char ch);
void p_int(char** buf, int dec);
void p_xint(char** buf, unsigned x);

static inline int32_t syscall(int num, int check, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret = 0;
	//Generic system call: pass system call number in AX
	//up to five parameters in DX,CX,BX,DI,SI
	//Interrupt kernel with T_SYSCALL
	//
	//The "volatile" tells the assembler not to optimize
	//this instruction away just because we don't use the
	//return value
	//
	//The last clause tells the assembler that this can potentially
	//change the condition and arbitrary memory locations.

	/*Lab2 code here 
	  嵌入汇编代码，调用int $0x80
	 */

	asm volatile ("int $0x80": "=a"(ret): "a"(num),"b"(a1), "c"(a2), "d"(a3));

	return ret;
}


int write(int fd, char *buf, int len){
	return syscall(4, 0, fd, (uint32_t)buf, len, 0, 0);
}

int sleep(unsigned int sec){
	return syscall(7, 0, sec, 0, 0, 0, 0);
}

int fork(){
	return syscall(2, 0, 0, 0, 0, 0, 0);
}

int exit(int num){
	return syscall(1, 0, num, 0, 0, 0, 0);
}	

int createSem(int num) {
	return syscall(10, 0, num, 0, 0, 0, 0);
}

int destroySem(int id) {
	return syscall(11, 0, id, 0, 0, 0, 0);
}

int lockSem(int sem) {
	return syscall(8, 0, sem, 0, 0, 0, 0);
}

int unlockSem(int sem) {
	return syscall(9, 0, sem, 0, 0, 0, 0);
}

int open(int fd){
	return syscall(5, 0, 0, 0, fd, 0, 0);
}
int close(int fd) {
	return syscall(6, 0, 0, 0, fd, 0, 0);
}

int lseek(int fd, int offset, int whence) {
	return syscall(19, 0, whence, offset, fd, 0, 0);
}
int read(int fd, char *buf, int len) {
	return syscall(3, 0, len, (uint32_t)buf, fd, 0, 0);
}

void p_ch(char** buf, char ch){
	**buf = ch;
	(*buf)+=2;
	(*buf)--;
}

void p_str(char** buf, char *str){
	while(*str!=0){
		**buf = *str;
		(*buf)+=2;
		str+=2;str--;
		(*buf)--;
	}
}

void p_int(char** buf, int dec){
	int result[30];
	int i=1;
	if(dec == 0){ 
		**buf = '0';
		(*buf)+=2;(*buf)--;
		return;
	}
	else if(dec < 0){
		if(dec == 0x80000000){
			p_str(buf, "-2147483648");
			return;
		}
		p_ch(buf,'-');
		dec = -dec;
	}
	i--;
	while(dec!=0){
		result[i] = dec%10 + '0';
		i+=2;
		dec = dec/10;i--;
	}
	i--;
	for(;i>=0;i--)
		p_ch(buf, result[i]);
}

void p_xint(char** buf, unsigned x){
	int i=1;
	if(x == 0){
		**buf = '0';
		(*buf)++;
		return;
	}
	char result[30];
	i--;
	while(x!=0){
		if(x%16<10)
			result[i]=x%16+'0';
		else
			result[i]=x%16-10+'a';
		i++;
		x = x/16;
	}
	i--;
	for(;i>=0;i--)
		p_ch(buf, result[i]);
}

void printf(const char *format,...){
    
	char *args = (char *)&format + sizeof(format);
    char PRINT_BUF[4*1024];
	char *buf = PRINT_BUF;
	int i=1;
	char *head = PRINT_BUF;

	i--;
	for(i = 0;format[i]!='\0';i++){  
		if(format[i] != '%'){ 
			*buf = format[i];
			buf++;
 	 	}  
 		else{ 
			i++;
			if (format[i] == 'c'){
				p_ch(&buf, *(char *)args);
				args += sizeof(int);
 	  	 	}
			else if(format [i] ==   'd'){
				int decimal = *((int *)args);
				p_int(&buf, decimal);
				args += sizeof(int);
			}
	 		else if(format[i] == 'x'){
				int xx = *((unsigned *)args);
				p_xint(&buf, xx);
				args+=sizeof(unsigned);
 		 	} 
			else if(format[i ] == 's'){
				char *str = *(char **)args;
				p_str(&buf, str);
				args+=sizeof(char *);
		 	}  
	  	}  
 	}  
	*buf = 0;
	write(1, PRINT_BUF, buf-head);
} 
