/*
 *  linux/include/linux/bfs_fs_i.h
 *
 * Copyright (C) 1999  Makoto Kato (m_kato@ga2.so-net.ne.jp)
 *
 *  from
 *
 *  linux/include/linux/ex2_fs_i.h
 */

#ifndef _LINUX_BFS_FS_I_
#define _LINUX_BFS_FS_I_

#include <linux/bfs_fs.h>

struct bfs_inode_info {
	bfs_inode_addr	i_inode_num;
	__u32	i_uid;
	__u32	i_gid;
	__u32	i_mode;
	__u32	i_flags;

	bfs_bigtime_t	i_create_time;
	bfs_bigtime_t	i_last_modified_time;

	bfs_inode_addr	i_parent;
	bfs_inode_addr	i_attribute;

	__u32	i_type;

	union {
		bfs_data_stream ds;
		char            symlink[BFS_SYMLINK_LEN];
	} i_data;
};

#endif /* _LINUX_BFS_FS_I_ */
