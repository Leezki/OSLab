#include "fs.h"                                                             
#include "common.h"
#include "assert.h"
extern Bitmap bitmap;
extern Dir dir;
void readseg(unsigned char *, int, int);
void readsect(void *, int);
 void printf(const char *format,...);
void load_file(void) {
	 readseg((unsigned char*)&bitmap, sizeof(bitmap), 401 * 512);
     readseg((unsigned char*)&dir, sizeof(dir), 2449 * 512);
     printf("%s\n", dir.entries[0].filename);
	 }
