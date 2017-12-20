#include "lib.h"


void test_file() {
	char buf[512];
	int fp = open("test.dat");
	fs_read(fp, buf, 512);
	printf("%s\n", buf);
	char temp[30]=" Write successfully.";
	fs_write(fp, temp, 19);
	fs_lseek(fp, -30, SEEK_CUR);
	printf("\n");
	fs_read(fp, buf, 30);
	printf("%s\n", buf);
	fs_close(fp);
}
void
uentry(void){		
		int semaphore,i;
		semaphore=createSem(0);
		if (fork() == 0) {
			i=1;	
			while(i < 11){
				lockSem(semaphore);			
				printf("Pong");
		//		sleep(1);
				++i;
			}
			destroySem(semaphore);
		}
		  else{
			i=1;
		   	while(i < 11){
				    printf("Ping");
					unlockSem(semaphore);
					sleep(1);
					++i;	
			}
		}
		exit(0);
		while(1);
}

