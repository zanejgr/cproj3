/**
 *  Given a current working directory and either an absolute or relative path, find both the inode of the
 * file or directory and the inode of the parent directory.  If one or both are not found, then they are
 * set to UNALLOCATED_INODE.
 *
 *  This implementation handles a variety of strange cases, such as consecutive /'s and /'s at the end of
 * of the path (we have to maintain some extra state to make this work properly).
 *
 * @param cwd Absolute path for the current working directory
 * @param path Absolute or relative path of the file/directory to be found
 * @param parent Inode reference for the parent directory
 * @param child  Inode reference for the file or directory specified by path
 * @param local_name String name of the file or directory without any path information (i.e., name relative
 *        to the parent)
 * @return 0 if no errors
 *         -1 if child not found
 *         -x if an error
 *
 */
#include "vdisk.h"
#include "oufs.h"
#include "oufs_lib.h"
#define debug 0
INODE_REFERENCE  oufs_find_directory_element(INODE* inode, char*directory_name){
	if(debug) puts("oufs_find_directory_element called for");
	if(debug) puts(directory_name);
	BLOCK* blockbuf = malloc(sizeof *blockbuf);

	if(debug) puts("making block buffer");
	for(int i = 0; i < BLOCKS_PER_INODE; i++){
		bzero(blockbuf,sizeof(*blockbuf));

		if(!vdisk_read_block(inode->data[i],blockbuf)){
			if(debug){ puts ("error reading block");
				putchar(inode->data[i]);
				puts("\n");
			}
			continue;
		}
		if(inode->type!=IT_DIRECTORY){
			if(debug)	puts("Inode not a directory");
			return( -1);
		}
		for(unsigned int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK;j++){
			if(debug){	puts("comparing");
				puts(directory_name);
				puts("to");
				puts(blockbuf->directory.entry[j].name);
			}
			if(!strncmp(directory_name,blockbuf->directory.entry[j].name,FILE_NAME_SIZE))
				return(blockbuf->directory.entry[j].inode_reference);
		}
	}
	if(debug){
		puts("unable to find");
		puts(directory_name);
	}
	return UNALLOCATED_INODE;
}
INODE_REFERENCE oufs_allocate_new_directory(INODE_REFERENCE parent){
	//make buffdrs for the parent and child inodes, as well as a BLOCK buffer
	BLOCK childblockbuf;
	BLOCK masterblockbuf;
	if(debug){
		printf("%ld",sizeof childblockbuf);
		puts(" bytes for blockbuf and");
		printf("%ld",sizeof masterblockbuf);
		puts(" bytes for master block\n");
	}

	INODE inode;
	//null-terminate
	
	bzero(inode.data,(sizeof inode)*(BLOCKS_PER_INODE));
	bzero(&childblockbuf,sizeof(childblockbuf));
	bzero(&masterblockbuf,sizeof(masterblockbuf));
	if(debug){
		printf("%ld",sizeof(inode.data));
		puts("for inode");
	}

	//read in the parent inode, parent block and master bloock
	vdisk_read_block(MASTER_BLOCK_REFERENCE,&masterblockbuf);
	
	// Scan for an available block
	//find space for the child inode
	unsigned int inode_byte;
	int flag;

	// Loop over each byte in the inode allocation table.
	for(inode_byte = 0, flag = 1; flag && inode_byte < N_INODE_BLOCKS/8; ++inode_byte) {
		if(masterblockbuf.master.inode_allocated_flag[inode_byte] != 0xff) { 

			// Found a byte that has an opening: stop scanning
			flag = 0;
			break;
		};
	};

	// Did we find a candidate byte in the table?
	if(flag == 1) {
		// No if(debug)
		fprintf(stderr, "No INODE available to allocate directory\n");
		return(UNALLOCATED_INODE);
	}

	// Found an available inode 
	// Set the inode allocated bit
	// Find the FIRST bit in the byte that is 0 (we scan in bit order: 0 ... 7)
	unsigned int inode_bit = oufs_find_open_bit(masterblockbuf.master.inode_allocated_flag[inode_byte]);

	// Now set the bit in the allocation table
	masterblockbuf.master.inode_allocated_flag[inode_byte] |= (1 << inode_bit);

	if(debug)
		fprintf(stderr, "Allocating inode=%d (%d)\n", inode_byte, inode_bit);

	//set up the child directory block
	BLOCK_REFERENCE blockReferenceBuffer = oufs_allocate_new_block();
	if(blockReferenceBuffer==UNALLOCATED_BLOCK){
		if(debug) puts("no block available to make direCTOry");
		return -1;
	}
	vdisk_read_block(blockReferenceBuffer,&childblockbuf);
	oufs_clean_directory_block(parent,inode_bit+inode_byte * 8,&childblockbuf);
	//set up values for the child inode and block
	inode.size = 2;
	inode.type = IT_DIRECTORY;
	inode.n_references = 1;
	inode.data[0] = blockReferenceBuffer;

	//write child block
	if(0!=vdisk_write_block(blockReferenceBuffer, &childblockbuf)&&debug){
		puts("couldn't write block");
		return UNALLOCATED_INODE;
	}

	//write child inode
	oufs_write_inode_by_reference(inode_bit+inode_byte*8,&inode);
	// Write out the updated master block, 
	vdisk_write_block(MASTER_BLOCK_REFERENCE,&masterblockbuf);

	if(debug) printf("Inode with inode refernce %d \n",inode_bit+inode_byte*8);
	return inode_bit + inode_byte*8;
}


/******************
 * END zane CODE***
 * BEGIN prof CODE*
 * ***************/

int oufs_find_file(char *cwd, char * path, INODE_REFERENCE *parent, INODE_REFERENCE *child, char *local_name)
{
	INODE_REFERENCE grandparent;
	char full_path[MAX_PATH_LENGTH];

	// Construct an absolute path the file/directory in question
	if(path[0] == '/') {
		strncpy(full_path, path, MAX_PATH_LENGTH-1);
	}else{
		if(strlen(cwd) > 1) {
			strncpy(full_path, cwd, MAX_PATH_LENGTH-1);
			strncat(full_path, "/", 2);
			strncat(full_path, path, MAX_PATH_LENGTH-1-strnlen(full_path, MAX_PATH_LENGTH));
		}else{
			strncpy(full_path, "/", 2);
			strncat(full_path, path, MAX_PATH_LENGTH-2);
		}
	}

	if(debug) {
		fprintf(stderr, "Full path: %s\n", full_path);
	};

	// Start scanning from the root directory
	// Root directory inode
	grandparent = *parent = *child = 0;
	if(debug)
		fprintf(stderr, "Start search: %d\n", *parent);

	// Parse the full path
	char *directory_name;
	directory_name = strtok(full_path, "/");
	while(directory_name != NULL) {
		if(strlen(directory_name) >= FILE_NAME_SIZE-1) 
			// Truncate the name
			directory_name[FILE_NAME_SIZE - 1] = 0;
		if(debug){
			fprintf(stderr, "Directory: %s\n", directory_name);
		}
		if(strlen(directory_name) != 0) {
			// We have a non-empty name
			// Remember this name
			if(local_name != NULL) {
				// Copy local name of file 
				strncpy(local_name, directory_name, MAX_PATH_LENGTH-1);
				// Make sure we have a termination
				local_name[MAX_PATH_LENGTH-1] = 0;
			}

			// Real next element
			INODE inode;
			// Fetch the inode that corresponds to the child
			if(oufs_read_inode_by_reference(*child, &inode) != 0) {
				return(-3);
			}

			// Check the type of the inode
			if(inode.type != 'D') {
				// Parent is not a directory
				*parent = *child = UNALLOCATED_INODE;
				return(-2);  // Not a valid directory
			}
			// Get the new inode that corresponds to the name by searching the current directory
			INODE_REFERENCE new_inode = oufs_find_directory_element(&inode, directory_name);
			grandparent = *parent;
			*parent = *child;
			*child = new_inode;
			if(new_inode == UNALLOCATED_INODE) {
				// name not found
				//  Is there another (nontrivial) step in the path?
				//  Loop until end or we have found a nontrivial name
				do {
					directory_name = strtok(NULL, "/");
					if(directory_name != NULL && strlen(directory_name) >= FILE_NAME_SIZE-1) 
						// Truncate the name
						directory_name[FILE_NAME_SIZE - 1] = 0;
				}while(directory_name != NULL && (strcmp(directory_name, "") == 0));

				if(directory_name != NULL) {
					// There are more sub-items - so the parent does not exist
					*parent = UNALLOCATED_INODE;
				};
				// Directory/file does not exist
				return(-1);
			};
		}
		// Go on to the next directory
		directory_name = strtok(NULL, "/");
		if(directory_name != NULL && strlen(directory_name) >= FILE_NAME_SIZE-1) 
			// Truncate the name
			directory_name[FILE_NAME_SIZE - 1] = 0;
	};

	// Item found.
	if(*child == UNALLOCATED_INODE) {
		// We went too far - roll back one step ***
		*child = *parent;
		*parent = grandparent;
	}
	if(debug) {
		fprintf(stderr, "Found: %d, %d\n", *parent, *child);
	}
	// Success!
	return(0);
} 



/**
 *  Make a new directory
 *
 * @param cwd Absolute path representing the current working directory
 * @param path Absolute or relative path to the file/directory
 * @return 0 if success
 *         -x if error
 *
 */
int oufs_mkdir(char *cwd, char *path)
{
	INODE_REFERENCE parent;
	INODE_REFERENCE child;
	char local_name[MAX_PATH_LENGTH];
	int ret;

	// Attempt to find the specified directory
	if((ret = oufs_find_file(cwd, path, &parent, &child, local_name)) < -1) {
		if(debug)
			fprintf(stderr, "oufs_mkdir(): ret = %d\n", ret);
		return(-1);
	};

	if(parent != UNALLOCATED_INODE && child == UNALLOCATED_INODE) {
		// Parent exists and child does not
		if(debug)
			fprintf(stderr, "oufs_mkdir(): parent=%d, child=%d\n", parent, child);

		// Get the parent inode
		INODE inode;
		if(oufs_read_inode_by_reference(parent, &inode) != 0) {
			return(-5);
		}
		if(debug){
			printf("Find result: %d, %d\n", parent, child);
		}

		if(inode.type == 'D') {
			// Parent is a directory
			BLOCK block;
			// Read the directory
			if(vdisk_read_block(inode.data[0], &block) != 0) {
				return(-6);
			}
			// Find a hole in the directory entry list
			for(unsigned int i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i) {
				if(block.directory.entry[i].inode_reference == UNALLOCATED_INODE) {
					// Found the hole: use this one
					if(debug)
						fprintf(stderr, "Making in parent inode: %d\n", parent);

					INODE_REFERENCE inode_reference = oufs_allocate_new_directory(parent);
					if(inode_reference == UNALLOCATED_INODE) {
						fprintf(stderr, "Disk is full\n");
						return(-4);
					}
					// Add the item to the current directory
					block.directory.entry[i].inode_reference = inode_reference;
					if(debug)
						fprintf(stderr, "new file: %s\n",  local_name);
					strncpy(block.directory.entry[i].name, local_name, FILE_NAME_SIZE-1);
					block.directory.entry[i].name[FILE_NAME_SIZE-1] = 0;

					// Write the block back out
					if(vdisk_write_block(inode.data[0], &block) != 0) {
						return(-7);
					}

					// Update the parent inode size
					inode.size++;

					// Write out the  parent inode
					if(oufs_write_inode_by_reference(parent, &inode) != 0) {
						return(-8);
					}

					// All done
					return(0);
				}
			}
			// No holes
			fprintf(stderr, "Parent is full\n");
			return(-4);
		}else{
			// Parent is not a directory
			fprintf(stderr, "Parent is a file\n");
			return(-3);
		}
	}else if(child != UNALLOCATED_INODE){
		// Child exists
		fprintf(stderr, "%s already exists\n", path);
		return(-1);
	}else{
		// Parent does not exist
		fprintf(stderr, "Parent does not exist\n");
		return(-2);
	}
	// Should not get to this point
	return(-100);
}

