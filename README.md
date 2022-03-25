# ashtfs

A file system for virtual machines.

## Disk Layout

The disk can be compared to a flat array of blocks. Each block has the following parts:

```
4KB size block:

[ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]
  ^ ^^^^^ ^ ^^^^^ ^^^^^^^^^^^^^^^^^
  a   b   c   d           e
```
- a     (int): the 0th bit or the index bit stores hat index of the current block.
- b  (int[3]): 3 integers that act as pointers to the extending blocks.
- c     (int): this stores the length of the name of the file or directory allocated to that block.
- d (uint8[]): This is a series of 8 bit unsigned integers, which form the actual name of the file or directory.
- e   (int[]): This is the file bitmap or the actual content of the file.


**Extending blocks** = Blocks that store chunks of files or directories that were too large to fit in one block.    
**Pointers**         = indexes to the beginning of blocks in the file system.
