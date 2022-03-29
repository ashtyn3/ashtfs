#include "fs.h"
#include "config.h"
#include "fs_constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Time complexity: O(log(n))
// [index or 0, pointer bit, pointer bit, pointer bit, name length, name..., file bitmap]
block *find(drive *head, unsigned int size, int start)
{
	int buffer[BLK_SIZE];

	FILE *ptr = fopen(head->path, "rb");

	if (start != 0) {
		fseek(ptr, start * BLK_SIZE, SEEK_SET);
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

	int buffer[FS_SIZE * BLK_SIZE];
	buffer[1] = FS_SIZE * BLK_SIZE;
	buffer[2] = 1;

	FILE *write_ptr;
	write_ptr = fopen(disk.path, "wb");
	fwrite(buffer, sizeof(int), sizeof(buffer), write_ptr);

	return disk;
}

void write_block(drive *head, block *f)
{
	FILE *ptr = fopen(head->path, "r+b");
	if (NULL == ptr) {
		print_fatal("Failed to open image");
	}
	fseek(ptr, f->start * BLK_SIZE, SEEK_SET);
	fwrite(f->buffer, 1, BLK_SIZE, ptr);
	fclose(ptr);
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
	dir_block->buffer[HEAD_SIZE - 1] = strlen(name);

	int offset = 0;
	for (int i = 0; i < dir_block->buffer[HEAD_SIZE - 1]; i++) {
		dir_block->buffer[i + HEAD_SIZE] = name[offset];
		offset++;
	}

	update_header(root);
	write_block(root, dir_block);

	return start;
}

block *alloc_file(drive *root, unsigned int size, const char name[],
		  const char content[])
{
	int buffer[BLK_SIZE];

	int start = make_pointer(root);
	update_header(root);
	buffer[0] = start;
	buffer[HEAD_SIZE - 1] = strlen(name);

	int offset = 0;
	for (int i = 0; i < buffer[HEAD_SIZE - 1]; i++) {
		buffer[i + HEAD_SIZE] = name[offset];
		offset++;
	}
	int remaining = BLK_SIZE - HEAD_SIZE - offset;

	int cont_size = strlen(content);

	srand(time(NULL));
	if (cont_size > remaining) {
		int left = cont_size - remaining;
		int end_i = cont_size - (left + 1);
		for (int i = 0; i < end_i; i++) {
			buffer[i + HEAD_SIZE + offset + 1] = content[i];
		}

		char *chunk = (char *)malloc(left * sizeof(char));
		for (int i = 0; i < left; i++) {
			chunk[i] = content[end_i + 1 + i];
		}
		int *block_ptr = (int *)malloc(2 * sizeof(int));
		get_loc_pointers(root, start, buffer, block_ptr);

		int index = 2;
		// for (int i = 0; i < 2; i++) {
		// 	if (block_ptr[i] == 0) {
		// 		index = i + 1;
		// 		break;
		// 	}
		// } // find zero pointers in current block
		if (block_ptr[index - 1] != 0) {
			index = -1;
		}

		block *writing_block = (block *)malloc(sizeof(block));
		if (index == -1) { // if no zero pointers
			print_info("No non nil pointers");
			int addr = get_aval_block(root, block_ptr[index]);
			// write pointer to random index
			memcpy(writing_block, find(root, FS_SIZE, addr),
			       sizeof(block));
		} else {
			int ptr = make_pointer(root);
			block *temp = find(root, FS_SIZE, ptr);
			memcpy(writing_block, temp, sizeof(block));
			memcpy(writing_block->buffer, buffer,
			       (HEAD_SIZE + offset) * sizeof(int));
			writing_block->buffer[0] = ptr;
		}

		if (writing_block->buffer[3998] != 0) {
			print_info("extending double pointer tree");
			int *block_ptrs = (int *)malloc(3 * sizeof(int));
			get_loc_pointers(root, writing_block->start,
					 writing_block->buffer, block_ptr);
			int index = 0;
			for (int i = 0; i < 3; i++) {
				if (block_ptrs[i] == 0) {
					index = i;
					break;
				}
			}
			update_header(root);
			block *cont = alloc_file(root, FS_SIZE, name, chunk);
			buffer[index] = cont->start;
		} else {
			block *cont = alloc_file(root, FS_SIZE, name, chunk);
			buffer[index] = cont->start;
		}
	} else {
		int offset = 0;
		for (int i = 0; i < cont_size; i++) {
			buffer[i + HEAD_SIZE + buffer[HEAD_SIZE - 1]] =
				content[offset];
			offset++;
		}
	}
	buffer[1] = (int)time(NULL);
	block *t_block = (block *)malloc(sizeof(block));
	memcpy(t_block->buffer, buffer, BLK_SIZE);
	t_block->start = start;
	write_block(root, t_block);
	update_header(root);
	return t_block;
}

int append_dir(drive *root, unsigned int size, int dir_start, int item_start,
	       const char name[])
{
	block *base = find(root, FS_SIZE, dir_start);

	if (base->buffer[1] != 0) {
		int *ptr = (int *)malloc(BLK_SIZE * sizeof(int));

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
	int name_len = base->buffer[HEAD_SIZE - 1];

	if (base->buffer[3999] != 0) {
		int pointer = make_pointer(root);
		block *t_block = (block *)malloc(sizeof(block));
		memcpy(t_block, base->buffer, HEAD_SIZE + name_len);
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
// TODO: implement write disk function.
//   - Writes dirty blocks to img filesystem.
