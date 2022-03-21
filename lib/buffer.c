#include "buffer.h"

void print_buf(const int buf[]) {
  printf("{");
  size_t n = sizeof(*buf);
  for (int i = 0; i < n; i++) {
    printf(" %d ", buf[i]);
  }
  printf("}\n");
}

char *get_name(const char buf[]) {
  int start_id = buf[0];
  int name_len = buf[1];

  char *name = {0};

  int offset = 0;
  for (int i = 2; i < name_len; i++) {
    name[offset] = buf[i];
    offset++;
  }
  return name;
}
