#include "fs.h"
#include "fs_constants.h"
#include "config.h"

int make_pointer(drive *root)
{
	int aval;
	FILE *ptr = fopen(root->path, "r+b");
	while (1) {
		block *head = find(root, FS_SIZE, 0);
		block *temp = find(root, FS_SIZE, head->buffer[2]);
		if (temp->buffer[0] == 0) {
			aval = temp->start;
			break;
		} else {
			head->buffer[2] = head->buffer[2] + 1;
			fwrite(head->buffer, sizeof(int), sizeof(head->buffer),
			       ptr);
		}
	}
	fclose(ptr);
	return aval;
}
int *get_pointers(drive *root, const int start, const int buf[], int *pointers)
{
	int pointer_i = 0;
	for (int i = 1; i < HEAD_SIZE - 1; i++) {
		int addr = buf[i];
		if (addr == 0) {
			break;
		}
		pointers[pointer_i] = addr;
		block *t_block = find(root, FS_SIZE, addr);
		get_pointers(root, addr, t_block->buffer, pointers);
	}
	return pointers;
}
int *get_loc_pointers(drive *root, const int start, const int buf[],
		      int *pointers)
{
	for (int i = 0; i < 2; i++) {
		int addr = buf[i + 1];
		pointers[i] = addr;
	}
	return pointers;
}

int get_aval_block(drive *root, const int start)
{
	block *b = find(root, FS_SIZE, start);
	int *ptr = (int *)malloc(2 * sizeof(int));
	get_loc_pointers(root, start, b->buffer, ptr);
	print_buf(ptr);
	if (ptr[2] == 0) {
		return start;
	} else {
		get_aval_block(root, start);
	}
	return start;
}
