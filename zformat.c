#include <stdio.h>
#include <string.h>
#include "oufs_lib.h"
#include "oufs.h"
#include "oufs_goodies.h"
#include <stdlib.h>
#include <strings.h>
#define debug 0
int main(int argc,char**argv){
	// Get the key environment variables
	char cwd[MAX_PATH_LENGTH];
	char disk_name[MAX_PATH_LENGTH];
	oufs_get_environment(cwd, disk_name);

	if(vdisk_disk_open(disk_name) != 0) {
		if(debug)
			puts("error opening disk");
		return(-1);
	}

	//set 0 to master block
	//zero out all contents, and initialize values for the root directory
	BLOCK* blockbuf = malloc(sizeof *blockbuf);	
	if(debug){
	       	puts("mallocing ");
		printf("%ld",sizeof(* blockbuf));
		puts(" bytes in blockbuf\n");
	}
	bzero(blockbuf,sizeof(*blockbuf));
	bzero(blockbuf->master.inode_allocated_flag,sizeof(blockbuf->master.inode_allocated_flag));
	bzero(blockbuf->master.block_allocated_flag,sizeof(blockbuf->master.block_allocated_flag));
	blockbuf->master.block_allocated_flag[0]=0xff;
	blockbuf->master.block_allocated_flag[1]=0x03;
	blockbuf->master.inode_allocated_flag[0]=0x01;
	vdisk_write_block(MASTER_BLOCK_REFERENCE,blockbuf);

	if(debug){
		puts("wrote master");
	}
	free(blockbuf);
	if(debug){
		puts("mallocing ");
		printf("%ld",sizeof(*blockbuf));
		puts(" bytes for blockbuf\n");
	}
	blockbuf = malloc(sizeof(*blockbuf));
	bzero(blockbuf,sizeof(*blockbuf));
	//inode 0 value
	INODE* inode = malloc(sizeof(inode));
	bzero(inode->data,sizeof(inode->data));
	if(debug){
		puts("mallocing ");
		printf("%ld",sizeof(inode->data));
		puts("for inode");
		puts("made inode buffer!");
	}
	inode->type = IT_DIRECTORY;
	inode->n_references = 1;
	for(int i = 0; i < BLOCKS_PER_INODE; i++){
		inode->data[i]=UNALLOCATED_BLOCK;
	}
	inode->data[0]=ROOT_DIRECTORY_BLOCK;

	inode->size = 0x0002;
	blockbuf->inodes.inode[0]=*inode;
	for(int i = 1; i < N_BLOCKS_IN_DISK; i++)
		vdisk_write_block(i,blockbuf);
	//make root directory
	free(blockbuf);
	blockbuf=malloc(sizeof(* blockbuf));
	if(debug){
		puts("mallocing ");
		printf("%ld",sizeof(blockbuf->directory));
		puts(" bytes for blockbuf\n");
	}
	bzero(blockbuf,sizeof(*blockbuf));
	vdisk_read_block(ROOT_DIRECTORY_BLOCK,blockbuf);

	oufs_clean_directory_block(0,0,blockbuf);
	vdisk_write_block(ROOT_DIRECTORY_BLOCK,blockbuf);
	//save changes
	vdisk_disk_close();

}

