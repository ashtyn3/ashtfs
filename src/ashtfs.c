#include "../fs/config.h"
#include "../fs/fs.h"
#include "../lib/buffer.h"

int main()
{
	drive root = init(FS_SIZE, IMG_PATH);

	int loc = make_pointer(&root);
	alloc_dir(&root, FS_SIZE, loc, "/");

	block *b = alloc_file(&root, FS_SIZE, "hi", "Hello, world!");
	write_block(&root, b);

	append_dir(&root, FS_SIZE, loc, b->start, "hi");

	block *dir = find(&root, FS_SIZE, loc);

	//block *header = find(&root, FS_SIZE, 0);
	//print_buf(header->buffer);

	return 0;
}
