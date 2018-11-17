#ifndef OUFS_GOODIES
#define OUFS_GOODIES
#endif
#include "oufs.h"

INODE_REFERENCE oufs_find_directory_element(INODE* inode, char* directory_name);

INODE_REFERENCE oufs_allocate_new_directory(INODE_REFERENCE parent);

int oufs_find_file(char*cwd, char*path, INODE_REFERENCE *parent, INODE_REFERENCE* child, char* local_name);

int oufs_mkdir(char*cwd, char*path);
