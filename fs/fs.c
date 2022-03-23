#include "fs.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int append_dir(drive *root, unsigned int size, int dir_start, int item_start,
	       const char name[])
{
	block *base = find(root, FS_SIZE, dir_start);

	const char *dirname = get_name(base->buffer);

	if (strcmp(name, dirname) == 0) {
		char *msg;
		sprintf(msg, "Cannot write %s as sub-path to %s", dirname,
			name);
		print_fatal(msg);
	}

	// FINISH IMPLEMENTING
	return 0;
}

int make_pointer(drive *root)
{
	block *head = find(root, FS_SIZE, 0);
	return head->buffer[2];
}
// TODO: implement write disk function.
//   - Writes dirty blocks to img filesystem.
