#include <stdio.h>
#include <string.h>
#include "oufs_goodies.h"
#include "oufs_lib.h"

#define debug 0

int main(int argc, char*argv[]){
	// Get the key environment variables
	char cwd[MAX_PATH_LENGTH];
	char disk_name[MAX_PATH_LENGTH];
	oufs_get_environment(cwd, disk_name);
	char path[MAX_PATH_LENGTH] = {0};
	if(vdisk_disk_open(disk_name) != 0) {
		return(-1);
	}
	if(argc==2)
		strncpy(path,argv[1],MAX_PATH_LENGTH);
	else if(argc == 1)

		strncpy(path,cwd,MAX_PATH_LENGTH);
	else{
		puts("usage: ./zfilez [<name>]");
		return -1;
	}
	//parse arguments
	INODE_REFERENCE parent;
	INODE_REFERENCE child;
	char local_name[MAX_PATH_LENGTH] = {0};
	int ret;
	if((ret = oufs_find_file(cwd,path,&parent,&child,local_name))){
		if(debug) fprintf(stderr, "File not found");
		return -1;
	}
	INODE inode;
	if(oufs_read_inode_by_reference(child, &inode)!=0) return -5;
	if(debug) printf("find result: %d, %d\n",parent,child);
	switch (inode.type)
	{
		case IT_DIRECTORY:

			return oufs_list(cwd,path);

		case IT_FILE:

			puts(local_name);
			puts("\n");

			return 0;

		default: 
			if(debug){
				puts("Unknown type: ");
				putchar(inode.type);
			}
			return -1;
	}


}

