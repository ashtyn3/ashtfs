#pragma once

#include "../lib/buffer.h"
#include "../lib/logger/logger.h"
#include <stdlib.h>

typedef struct block {
  int start;
  unsigned char buffer[4000];
  // 1000000000 / 10 =  100000000
  // 250000

  struct block *next;
} block;

block init(unsigned int);
block *find(block *, unsigned int, int);

int alloc_dir(block *, unsigned int, int, const char[]);
int make_pointer(block *);
int append_dir(block *root, unsigned int size, int dir_start, int item_start,
               const char name[]);
