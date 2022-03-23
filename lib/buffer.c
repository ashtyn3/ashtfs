#include "buffer.h"
#include "stdlib.h"

void print_buf(const int buf[])
{
	printf("{");
	for (int i = 0; i < 100; i++) {
		printf(" %d ", buf[i]);
	}
	printf("}\n");
}

char *get_name(const int buf[])
{
	int start_id = buf[0];
	int name_len = buf[4];

	int *name = (int *)malloc(name_len * sizeof(int));

	int offset = 0;
	for (int i = 0; i < name_len; i++) {
		name[offset] = buf[i + 5];
		offset++;
	}
	return (char *)name;
}
