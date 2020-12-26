/*
 *  linux/include/linux/bfs_fs_sb.h
 *
 * Copyright (C) 1999  Makoto Kato (m_kato@ga2.so-net.ne.jp)
 */

#ifndef _LINUX_BFS_FS_SB
#define _LINUX_BFS_FS_SB

#include <linux/types.h>
#include <linux/bfs_fs.h>


struct bfs_sb_info {
	__u32		block_size;
	__u32		block_shift;
	bfs_off_t	num_blocks;
	bfs_off_t	used_blocks;
	__u32		inode_size;

	/*
	 * Allocation group information
	 */

	__u32	blocks_per_ag;
	__u32	ag_shift;
	__u32	num_ags;

	/*
	 * jornal log entry
	 */

	bfs_block_run log_blocks;
	bfs_off_t log_start;
	bfs_off_t log_end;

	bfs_inode_addr root_dir;
	bfs_inode_addr indices;

	bfs_mount_options mount_opts;

	struct nls_table * nls;
};
#endif
