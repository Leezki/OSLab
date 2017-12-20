#ifndef __lib_h__
#define __lib_h__

void printf(const char *format,...);
int sleep(unsigned);
int exit(int);
int fork();
int createSem(int);
int destroySem(int);
int lockSem(int);
int unlockSem(int);

#endif
