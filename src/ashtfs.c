#include "../fs/config.h"
#include "../fs/fs.h"
#include "../lib/buffer.h"

static char *rand_string(char *str, size_t size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK";
	if (size) {
		--size;
		for (size_t n = 0; n < size; n++) {
			int key = rand() % (int)(sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size] = '\0';
	}
	return str;
}

int main()
{
	drive root = init(FS_SIZE, IMG_PATH);

	//int loc = make_pointer(&root);
	//alloc_dir(&root, FS_SIZE, loc, "/");

	char *content = (char *)malloc(4011 * sizeof(char));
	rand_string(content, 4010);
	block *b = alloc_file(&root, FS_SIZE, "hi", content);

	block *idk = find(&root, FS_SIZE, 1);
	print_buf(idk->buffer);

	return 0;
}
