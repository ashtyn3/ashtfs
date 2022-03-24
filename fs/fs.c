#include "fs.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Time complexity: O(log(n))
// [index or 0, pointer bit, pointer bit, pointer bit, name length, name..., file bitmap]
block *find(drive *head, unsigned int size, int start)
{
	int buffer[4000];

	FILE *ptr = fopen(head->path, "rb");

	if (start != 0) {
		fseek(ptr, start * 4000, SEEK_SET);
	}

	fread(buffer, sizeof(buffer), 1, ptr);

	block *blk = malloc(sizeof(block));
	memcpy(blk->buffer, buffer, sizeof(buffer));
	(*blk).start = start;

	fclose(ptr);
	return blk;
}

drive init(unsigned int size, char *img)
{
	drive disk = { .path = img };

	int buffer[FS_SIZE * 4000];
	buffer[1] = FS_SIZE * 4000;
	buffer[2] = 1;

	FILE *write_ptr;
	write_ptr = fopen(disk.path, "wb");
	fwrite(buffer, sizeof(int), sizeof(buffer), write_ptr);

	return disk;
}

void write_block(drive *head, block *f)
{
	FILE *ptr = fopen(head->path, "r+b");
	fseek(ptr, f->start * 4000, SEEK_SET);
	fwrite(f->buffer, sizeof(int), sizeof(f->buffer), ptr);
}

void update_header(drive *root)
{
	block *header = find(root, FS_SIZE, 0);
	header->buffer[1] = header->buffer[1] - 1;
	header->buffer[2] = header->buffer[2] + 1;

	write_block(root, header);
}

int alloc_dir(drive *root, unsigned int size, int start, const char name[])
{
	block *header = find(root, FS_SIZE, 0);
	if (start == 0) {
		print_fatal(
			"Cannot set start offset to 0. Filesystem required block.");
		print_info("Try start offset 1.");
	} else if (start < header->buffer[2]) {
		char *msg = "";
		sprintf(msg,
			"Writing to %d possibly overwrites a written block.",
			header->buffer[2]);
		print_warn(msg);
	}

	block *dir_block = find(root, FS_SIZE, start);
	if (dir_block->buffer[0] != 0) {
		char *msg = "";
		sprintf(msg, "Cannot write to offset %d with name %s",
			header->buffer[2], get_name(dir_block->buffer));
		print_fatal(msg);
	}
	dir_block->buffer[0] = start;
	dir_block->buffer[4] = strlen(name);

	int offset = 0;
	for (int i = 0; i < dir_block->buffer[4]; i++) {
		dir_block->buffer[i + 5] = name[offset];
		offset++;
	}

	update_header(root);
	write_block(root, dir_block);

	return start;
}

block *alloc_file(drive *root, unsigned int size, const char name[],
		  const char content[])
{
	print_warn("Temporary allocation");
	print_info(
		"Please use write_block(drive* head, block* f) to allocate block.");
	int start = make_pointer(root);
	int buffer[4000];

	buffer[0] = start;
	buffer[4] = strlen(name);

	int offset = 0;
	for (int i = 0; i < buffer[4]; i++) {
		buffer[i + 5] = name[offset];
		offset++;
	}
	int remaining = 4000 - 5 - offset;

	int cont_size = strlen(content);

	if (cont_size > remaining) {
		print_warn("NOT IMPLEMENTED");
		int chunk_amt = floor(cont_size / remaining);
		for (int i = 0; i < chunk_amt; i++) {
			int start = i * remaining;
			int end = start + remaining - 1;
			if (i == chunk_amt - 1) {
				end = strlen(content) - 1;
			}

			for (int i = start; i <= end; i++) {
				printf("%d ", content[i]);
			}
			printf("\n");
		}
	} else {
		int offset = 0;
		for (int i = 0; i < cont_size; i++) {
			buffer[i + 5 + buffer[4]] = content[offset];
			offset++;
		}
	}

	block *t_block = (block *)malloc(sizeof(block));
	memcpy(t_block, buffer, 4000);
	t_block->start = start;

	return t_block;
}

int append_dir(drive *root, unsigned int size, int dir_start, int item_start,
	       const char name[])
{
	block *base = find(root, FS_SIZE, dir_start);

	if (base->buffer[1] != 0) {
		int *ptr = (int *)malloc(4000 * sizeof(int));

		get_pointers(root, dir_start, base->buffer, ptr);
		int end = sizeof(&ptr) / sizeof(ptr[0]) - 1;

		append_dir(root, FS_SIZE, ptr[end], item_start, name);

		return 0;
	}

	const char *dirname = get_name(base->buffer);
	if (strcmp(name, dirname) == 0) {
		char *msg;
		sprintf(msg, "Cannot write %s as sub-path to %s", dirname,
			name);
		print_fatal(msg);
	}

	int id = base->buffer[0];
	int name_len = base->buffer[4];

	if (base->buffer[3999] != 0) {
		int pointer = make_pointer(root);
		block *t_block = (block *)malloc(sizeof(block));
		memcpy(t_block, base->buffer, 5 + name_len);
		t_block->buffer[0] = pointer;
		t_block->start = pointer;

		int offset = 1;
		while (1) {
			if (base->buffer[offset] == 0) {
				break;
			}
			offset++;
		}
		base->buffer[offset] = pointer;
		write_block(root, base);
		write_block(root, t_block);
		append_dir(root, FS_SIZE, t_block->start, item_start, name);
	} else {
		print_info("Writing to dir allocation: not implemented");
	}
	// FINISH IMPLEMENTING
	return 0;
}

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
	for (int i = 1; i < 4; i++) {
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
// TODO: implement write disk function.
//   - Writes dirty blocks to img filesystem.
