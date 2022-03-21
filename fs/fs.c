#include "fs.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

block *find(block *head, unsigned int size, int start) {
  block *temp = head;
  for (int i = 0; i < size; i++) {
    if (temp->start == start) {
      break;
    } else {
      temp = temp->next;
    }
  }

  return temp;
}

block init(unsigned int size) {
  block *root = (struct block *)malloc(sizeof(struct block));
  block *head = root;

  for (int i = 0; i < size; i++) {
    rootnext = (struct block *)malloc(sizeof(struct block));
    root = *root.next;
  }

  head->buffer[0] = 0;
  head->buffer[1] = size;
  head->buffer[2] = 1;

  return *head;
}

void update_header(block *root) {
  block *header = find(root, FS_SIZE, 0);
  header->buffer[1] = header->buffer[1] - 1;

  block *updated_head = find(root, FS_SIZE, 0);
  updated_head->buffer[2] = updated_head->buffer[2] + 1;
}

int alloc_dir(block *root, unsigned int size, int start, const char name[]) {
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

int make_pointer(block *root) {
  root = find(root, FS_SIZE, 0);
  printf("pointer addr: %d\n", root->start);
  return root->buffer[2];
}
// TODO: implement write disk function.
//   - Writes dirty blocks to img filesystem.
