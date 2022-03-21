#include "fs.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

block *find(drive *head, unsigned int size, int start) {
  int buffer[4000];
  FILE *ptr = fopen(head->path, "rb");
  fseek(ptr, start * 1000, SEEK_SET);
  fread(buffer, sizeof(buffer), 1, ptr);

  block *blk = malloc(sizeof(block));
  memcpy(blk->buffer, buffer, sizeof(buffer));
  (*blk).start = start;

  fclose(ptr);
  return blk;
}

drive init(unsigned int size, char *img) {
  drive disk = {.path = img};

  int buffer[FS_SIZE * 1000];
  buffer[1] = FS_SIZE * 1000;
  buffer[2] = 1;

  FILE *write_ptr;
  write_ptr = fopen(disk.path, "wb");
  fwrite(buffer, sizeof(int), sizeof(buffer), write_ptr);

  return disk;
}

void write_block(drive *head, block *f) {
  FILE *ptr = fopen(head->path, "r+b");
  fseek(ptr, f->start * 1000, SEEK_SET);
  fwrite(f->buffer, sizeof(int), sizeof(f->buffer), ptr);
}

void update_header(drive *root) {
  block *header = find(root, FS_SIZE, 0);
  header->buffer[1] = header->buffer[1] - 1;
  header->buffer[2] = header->buffer[2] + 1;

  write_block(root, header);
}

int alloc_dir(drive *root, unsigned int size, int start, const char name[]) {
  block *header = find(root, FS_SIZE, 0);
  if (start == 0) {
    print_fatal("Cannot set start offset to 0. Filesystem required block.");
    print_info("Try start offset 1.");
  } else if (start < header->buffer[2]) {
    char *msg = "";
    sprintf(msg, "Writing to %d possibly overwrites a written block.",
            header->buffer[2]);
    print_warn(msg);
  }

  block *dir_block = find(root, FS_SIZE, start);
  dir_block->buffer[0] = start;
  dir_block->buffer[1] = strlen(name);

  int offset = 0;
  for (int i = 0; i < dir_block->buffer[1]; i++) {
    dir_block->buffer[i + 2] = name[offset];
    offset++;
  }

  update_header(root);
  write_block(root, dir_block);

  return start;
}

int append_dir(block *root, unsigned int size, int dir_start, int item_start,
               const char name[]) {
  //  block *base = find(root, FS_SIZE, dir_start);

  //  char *dirname = get_name((char *)base->buffer);

  //  if (dirname == name) {
  //    char *msg = "";
  //    sprintf(msg, "Cannot write %s to folder %s", dirname, name);
  //    print_fatal(msg);
  //  }

  //  int index = 0;
  //  for (int i = 0; i < 4000; i++) {
  //    unsigned char unit = base->buffer[i];
  //    if (unit == 0 && i > 1) {
  //      index = i + 1;
  //      break;
  //    }
  //  }

  //  base->buffer[index + 2] = item_start;
  //  base->buffer[index + 3] = strlen(name);

  //  index = index + 4;

  //  int offset = 0;
  //  for (int i = 0; i < strlen(name); i++) {
  //    base->buffer[index + i] = name[offset];
  //    offset++;
  //  }

  // FINISH IMPLEMENTING
  return 0;
}

int make_pointer(drive *root) {
  block *head = find(root, FS_SIZE, 0);
  return head->buffer[2];
}
// TODO: implement write disk function.
//   - Writes dirty blocks to img filesystem.
