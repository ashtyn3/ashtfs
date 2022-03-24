#pragma once

#include "../lib/buffer.h"
#include "../lib/logger/logger.h"
#include <stdlib.h>

typedef struct block {
	int start;
	int buffer[4000];
	// 1000000000 / 10 =  100000000
	// 250000

} block;

typedef struct drive {
	char *path;
	block dirty;
} drive;

drive init(unsigned int, char *img);
block *find(drive *, unsigned int, int);

int alloc_dir(drive *, unsigned int, int, const char[]);
block *alloc_file(drive *root, unsigned int size, const char name[],
		  const char content[]);
int make_pointer(drive *);
int *get_pointers(drive *root, const int start, const int buf[], int *pointers);
int append_dir(drive *root, unsigned int size, int dir_start, int item_start,
	       const char name[]);

void write_block(drive *, block *);
void update_header(drive *);
