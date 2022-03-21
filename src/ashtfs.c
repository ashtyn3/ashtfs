#include "../fs/config.h"
#include "../fs/fs.h"
#include "../lib/buffer.h"

int main() {
  block root = init(FS_SIZE);

  unsigned int pointer = make_pointer(&root);
  alloc_dir(&root, FS_SIZE, pointer, "bob");

  printf("root after first alloc:\n");
  print_buf(root.buffer);

  unsigned int pointer2 = make_pointer(&root);
  // append_dir(&root, FS_SIZE, pointer, pointer2, "steven");

  block *dir = find(&root, FS_SIZE, pointer);

  return 0;
}
