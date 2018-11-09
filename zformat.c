#include <stdio.h>
#include <string.h>
#include "oufs_lib.h"
#include "oufs.h"
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
   	BLOCK* blockbuf = malloc(sizeof(blockbuf));	
	bzero(blockbuf,sizeof(blockbuf));
  	bzero(blockbuf->master.inode_allocated_flag,sizeof(blockbuf->master.inode_allocated_flag));
	bzero(blockbuf->master.block_allocated_flag,sizeof(blockbuf->master.block_allocated_flag));
	vdisk_write_block(MASTER_BLOCK_REFERENCE,blockbuf);


	//save changes
	vdisk_disk_close();

}

