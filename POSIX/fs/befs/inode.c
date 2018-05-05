/*
 * befs_inode.c
 * 
 * Copyright (C) 2001 Will Dyson <will_dyson@pobox.com>
 */

#include <linux/fs.h>


#include <befs_fs.h>
#include <endian.h>




/*
	Validates the correctness of the befs befs_inode
	Returns BEFS_OK if the befs_inode should be used, otherwise
	returns BEFS_BAD_INODE
*/
int befs_check_inode(befs_super_block* sb, befs_inode* raw_inode, 
		befs_blocknr_t befs_inode)
{
	__u32 magic1 = fs32_to_cpu(sb, raw_inode->magic1);
	befs_inode_addr ino_num = fsrun_to_cpu(sb, raw_inode->inode_num);
	__u32 flags = fs32_to_cpu(sb, raw_inode->flags);

	/* check magic header. */
	if (magic1 != BEFS_INODE_MAGIC1) {
		befs_error(sb, 
			"befs_inode has a bad magic header - befs_inode = %lu",
			befs_inode);
		return BEFS_BAD_INODE;
	}
	
	/*
	 * Sanity check2: inodes store their own block address. Check it.
	 */
	if (befs_inode != iaddr2blockno(sb, &ino_num) ) {
		befs_error(sb, "befs_inode blocknr field disagrees with vfs "
				"VFS: %lu, befs_inode %lu", 
				befs_inode, 
				iaddr2blockno(sb, &ino_num) );
		return BEFS_BAD_INODE;
	}
	
	/*
	 * check flag
	 */

	if (!(flags & BEFS_INODE_IN_USE)) {
		befs_error(sb, "befs_inode is not used - befs_inode = %lu",
			befs_inode);
		return BEFS_BAD_INODE;
	}
	
	return BEFS_OK;
}
