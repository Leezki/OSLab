#include "fs.h"
#include "common.h"
//#include "string.h"

#define diskoff 512 * 401
#define baseoff 512 * 256 * 8

struct Block {
	uint8_t data[512];
};
typedef struct Block Block;

void initfcb(void) {
	int i;
	for (i = 0; i < NR_FILES; i++)
		fcb_pool[i].opened = 0;
}

int fs_open(const char *pathname) {
	printf("pathname %s\n", pathname);
	int i;
	for (i = 0; i < NR_FCB; i++) {
		if (fcb_pool[i].opened)
			continue;
		fcb_pool[i].opened = 1;
		fcb_pool[i].offset = 0;

		int j;
		for (j = 0; j < NR_FILES; j++) {
				printf("Open %s..\n", pathname);
				fcb_pool[i].size = dir.entries[j].file_size;
				printf("dir.entries.inode_offset %d\n", dir.entries[j].inode_offset);
 				ide_read((diskoff + baseoff + 512 + dir.entries[j].inode_offset * sizeof(iNode)) / 512, fcb_pool[i].block, 1);			}
 		
		return i;
 	}
	return -1;
}

int fs_read(int fd, void *buf, int len) {
 	if (fd < 0 || fd >= NR_FCB) {
		printf("Out of the range of files!\n");
		assert(0);
	} 
	assert(fcb_pool[fd].opened);

	int offset_prev = fcb_pool[fd].offset;
	int sect = fcb_pool[fd].offset / 512;
	int pos = sect * 512;
	int begin;
	int size;
	fcb_pool[fd].offset += len;
	if (fcb_pool[fd].offset > fcb_pool[fd].size)
		fcb_pool[fd].offset = fcb_pool[fd].size;

	Block temp;
	while (len) {
		ide_read((diskoff + baseoff + 512 + 3 * sizeof(iNode)) / 512 + fcb_pool[fd].block[sect], temp.data, 1);
		begin = offset_prev - pos;
		if (len >= 512 - begin) {
			len = len - 512 + begin;
			size = 512 - begin;
 		}
		else {
			size = len - begin;
			len = 0;
 		}
		memcpy(buf, &temp.data[begin], size);
		buf += size;
		pos += SECTSIZE;
		offset_prev = pos;
		sect++;
 	}
	return 0;
}

int fs_write(int fd, void *buf, int len) {
 	if (fd < 0 || fd >= NR_FCB) {
		printf("Out of the range of files!\n");
		assert(0);
	}
	assert(fcb_pool[fd].opened);

	int offset_prev = fcb_pool[fd].offset;
	int sect = offset_prev / 512;
	int pos = sect * 512;
	int begin;
	int size;
	fcb_pool[fd].offset += len;
	if (fcb_pool[fd].offset > fcb_pool[fd].size)
		fcb_pool[fd].offset = fcb_pool[fd].size;

 	Block temp;
 	while (len) {
		ide_read((diskoff + baseoff + 512 + 3 * sizeof(iNode)) / 512 + fcb_pool[fd].block[sect], temp.data, 1);
		begin = offset_prev - pos;
 		if (len > 512 - begin) {
			len = len - 512 + begin;
			size = 512 - begin;
		}
 		else {
			size = len - begin;
			len = 0;
		}
		memcpy(&temp.data[begin], buf, size);
		ide_write((diskoff + baseoff + 512 + 3 * sizeof(iNode)) / 512 + fcb_pool[fd].block[sect], temp.data, 1);
		buf += size;
		pos += SECTSIZE;
		offset_prev = pos;
		sect++;
	}
	return 0;
}

int fs_lseek(int fd, int offset, int whence) {
	if (fd < 0 || fd >= NR_FCB) {
		printf("Out of the range of files!\n");
		assert(0);
 	}
	assert(fcb_pool[fd].opened);

	switch(whence) {
		case SEEK_SET:
			break;
		case SEEK_CUR:
			offset += fcb_pool[fd].offset;
			break;
		case SEEK_END:
			offset += dir.entries[fd].file_size;
			break;
		default:
			assert(0);
 	}
 	if (offset >= 0 && offset <= dir.entries[fd].file_size) {
		fcb_pool[fd].offset = offset;
		return offset;
	}
	else {
		printf("Out of boundary!\n");
		assert(0);
 	}
	return -1;
}

int fs_close(int fd) {
	if (fd < 0 || fd >= NR_FCB){
		printf("Out of the range of files!\n");
		assert(0);
	}
	assert(fcb_pool[fd].opened);
	fcb_pool[fd].opened = 0;
	return 0;
}


