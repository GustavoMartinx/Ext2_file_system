/*
 * ext2super.c
 *
 * Reads the super-block from a Ext2 floppy disk.
 *
 * Questions?
 * Emanuele Altieri
 * ealtieri@hampshire.edu
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./ext2_fs.h"

#define BASE_OFFSET 1024                   /* locates beginning of the super block (first group) */
#define FD_DEVICE "./myext2image.img"      /* the floppy disk device */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)

static unsigned int block_size = 0;        /* block size (to be calculated) */


// Function to read super block
void read_super_block(struct ext2_super_block super) {

	printf("Reading super-block from device " FD_DEVICE ":\n"
	       "Inodes count............: %u\n"
	       "Blocks count............: %u\n"
	       "Reserved blocks count...: %u\n"
	       "Free blocks count.......: %u\n"
	       "Free inodes count.......: %u\n"
	       "First data block........: %u\n"
	       "Block size..............: %u\n"
	       "Blocks per group........: %u\n"
	       "Inodes per group........: %u\n"
	       "Creator OS..............: %u\n"
	       "First non-reserved inode: %u\n"
	       "Size of inode structure.: %hu\n"
	       ,
	       super.s_inodes_count,  
	       super.s_blocks_count,
	       super.s_r_blocks_count,     /* reserved blocks count */
	       super.s_free_blocks_count,
	       super.s_free_inodes_count,
	       super.s_first_data_block,
	       block_size,
	       super.s_blocks_per_group,
	       super.s_inodes_per_group,
	       super.s_creator_os,
	       super.s_first_ino,          /* first non-reserved inode */
	       super.s_inode_size);
		
	printf("\n\n");
}



void read_group_descriptor(struct ext2_group_desc group) {
	printf("Reading first group-descriptor from device " FD_DEVICE ":\n"
	       "Blocks bitmap block: %u\n"
	       "Inodes bitmap block: %u\n"
	       "Inodes table block.: %u\n"
	       "Free blocks count..: %u\n"
	       "Free inodes count..: %u\n"
	       "Directories count..: %u\n"
	       ,
	       group.bg_block_bitmap,
	       group.bg_inode_bitmap,
	       group.bg_inode_table,
	       group.bg_free_blocks_count,
	       group.bg_free_inodes_count,
	       group.bg_used_dirs_count);    /* directories count */

	printf("\n\n");
}



// Function to read inode
static void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode)
{
	lseek(fd, BLOCK_OFFSET(group->bg_inode_table) + (inode_no - 1) * sizeof(struct ext2_inode), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));
} /* read_inode() */




// Function to read root inode
void print_read_root_inode(struct ext2_inode inode)
{
	printf("Reading root inode\n"
		   "File mode: %hu\n"
		   "Owner UID: %hu\n"
		   "Size.....: %u bytes\n"
		   "Blocks...: %u\n",
		   inode.i_mode,     /* inode.i_mode */
		   inode.i_uid,      /* inode.i_mode */
		   inode.i_size,     /* inode.i_mode */
		   inode.i_blocks);  /* inode.i_mode */

	for (int i = 0; i < EXT2_N_BLOCKS; i++)
		if (i < EXT2_NDIR_BLOCKS) /* direct blocks */
			printf("Block %2u : %u\n", i, inode.i_block[i]);
		else if (i == EXT2_IND_BLOCK) /* single indirect block */
			printf("Single...: %u\n", inode.i_block[i]);
		else if (i == EXT2_DIND_BLOCK) /* double indirect block */
			printf("Double...: %u\n", inode.i_block[i]);
		else if (i == EXT2_TIND_BLOCK) /* triple indirect block */
			printf("Triple...: %u\n", inode.i_block[i]);
}




int main(void)
{
	struct ext2_super_block super;
	struct ext2_group_desc group;
	struct ext2_inode inode;
	int fd;
	// int i; (i do for de imprimir read root inode)

	/* open floppy device */

	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0) {
		perror(FD_DEVICE);
		exit(1);  /* error while opening the floppy device */
	}

	
	





	/****** read super-block *******/

	lseek(fd, BASE_OFFSET, SEEK_SET); 
	read(fd, &super, sizeof(super));

	if (super.s_magic != EXT2_SUPER_MAGIC) {
		fprintf(stderr, "Not a Ext2 filesystem\n");
		exit(1);
	}
		
	block_size = 1024 << super.s_log_block_size;

	read_super_block(super);




	/********* read group descriptor ***********/

	lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
	read(fd, &group, sizeof(group));
	//close(fd);
	
	read_group_descriptor(group);




	
	/******** read root inode ********/
 	read_inode(fd, 2, &group, &inode);
 
 	print_read_root_inode(inode);


	close(fd);
	exit(0);
} /* main() */
