/*
 * linux/include/linux/bfs_fs.h
 *
 * Copyright (C) 1999 Makoto Kato (m_kato@ga2.so-net.ne.jp)
 */

#ifndef _LINUX_BFS_FS
#define _LINUX_BFS_FS

#include <linux/types.h>
#include <linux/fs.h>

/*
 * for debug
 */
/* #define BFS_DEBUG */

#ifdef BFS_DEBUG
#define BFS_OUTPUT(x)        printk x
#define BFS_DUMP_SUPER_BLOCK bfs_dump_super_block
#define BFS_DUMP_INODE       bfs_dump_inode
#define BFS_DUMP_INDEX_ENTRY bfs_dump_index_entry
#define BFS_DUMP_INDEX_NODE  bfs_dump_index_node
#define BFS_DUMP_INODE_ADDR  bfs_dump_inode_addr
#else
#define BFS_OUTPUT(x)
#define BFS_DUMP_SUPER_BLOCK(x)
#define BFS_DUMP_INODE(x)
#define BFS_DUMP_INDEX_ENTRY(x)
#define BFS_DUMP_INDEX_NODE(x)
#define BFS_DUMP_INODE_ADDR(x)
#endif


/*
 * Flags of superblock
 */

#define BFS_CLEAN 0x434c454e
#define BFS_DIRTY 0x44495254

/*
 * Max name length of BFS
 */

#define BFS_NAME_LEN 255
#define BFS_SYMLINK_LEN 160

/*
 * Magic headers of BFS's superblock, inode and index
 */

#define BFS_SUPER_BLOCK_MAGIC1 0x42465331 /* BFS1 */
#define BFS_SUPER_BLOCK_MAGIC2 0xdd121031
#define BFS_SUPER_BLOCK_MAGIC3 0x15b6830e

#define BFS_INODE_MAGIC1 0x3bbe0ad9

#define BFS_INDEX_MAGIC 0x69f6c2e8

#define BFS_SUPER_MAGIC BFS_SUPER_BLOCK_MAGIC1


#define BFS_NUM_DIRECT_BLOCKS 12
#define B_OS_NAME_LENGTH 32

/*
 * BFS filesystem type
 */

#define BFS_PPC 1
#define BFS_X86 2

/*
 * default gid and uid
 */

#define BFS_DEFAULT_GID 0
#define BFS_DEFAULT_UID 0

/*
 * Flags of inode
 */

#define BFS_INODE_IN_USE      0x00000001
#define BFS_ATTR_INODE        0x00000004
#define BFS_INODE_LOGGED      0x00000008
#define BFS_INODE_DELETED     0x00000010

#define BFS_PERMANENT_FLAG    0x0000ffff

#define BFS_INODE_NO_CREATE   0x00010000
#define BFS_INODE_WAS_WRITTEN 0x00020000
#define BFS_NO_TRANSACTION    0x00040000


#define BFS_IADDR2INO(iaddr,sb) \
	(((iaddr)->allocation_group << (sb)->ag_shift) + (iaddr)->start)
#define BFS_INODE2INO(inode) \
	(((inode)->u.bfs_i.i_inode_num.allocation_group << \
		(inode)->i_sb->u.bfs_sb.ag_shift) \
	+ (inode)->u.bfs_i.i_inode_num.start)

#define BFS_BLOCK_PER_INODE(sb) \
	((sb)->u.bfs_sb.block_size / sizeof(bfs_inode_addr))

#define BFS_IS_EMPTY_IADDR(iaddr) \
	((!(iaddr)->allocation_group) && (!(iaddr)->start) && (!(iaddr)->len))

#define BFS_TYPE(sb) \
	((sb)->u.bfs_sb.mount_opts.bfs_type)

/* 
 * special type of BFS
 */

typedef long long	bfs_off_t;
typedef long long	bfs_bigtime_t;
typedef void	bfs_binode_etc;


/* Block runs */
typedef struct _bfs_block_run {
	__u32	allocation_group;
	__u16	start;
	__u16	len;
} bfs_block_run;


typedef bfs_block_run	bfs_inode_addr;


/*
 * The Superblock Structure
 */

typedef struct _bfs_super_block {
	char	name[B_OS_NAME_LENGTH];
	__u32	magic1;
	__u32	fs_byte_order;

	__u32	block_size;
	__u32	block_shift;

	bfs_off_t  num_blocks;
	bfs_off_t  used_blocks;

	__u32          inode_size;

	__u32          magic2;
	__u32          blocks_per_ag;
	__u32          ag_shift;
	__u32          num_ags;

	__u32          flags;

	bfs_block_run  log_blocks;
	bfs_off_t      log_start;
	bfs_off_t      log_end;

	__u32          magic3;
	bfs_inode_addr root_dir;
	bfs_inode_addr indices;

	__u32          pad[8];
} __attribute__ ((packed)) bfs_super_block;


typedef struct _bfs_data_stream {
	bfs_block_run	direct[BFS_NUM_DIRECT_BLOCKS];
	bfs_off_t	max_direct_range;
	bfs_block_run	indirect;
	bfs_off_t	max_indirect_range;
	bfs_block_run	double_indirect;
	bfs_off_t	max_double_indirect_range;
	bfs_off_t	size;
} __attribute__ ((packed)) bfs_data_stream;


/* Attribute */
typedef struct _bfs_small_data {
	__u32	type;
	__u16	name_size;
	__u16	data_size;
} __attribute__ ((packed)) bfs_small_data;


/* Inode structure */
typedef struct _bfs_inode {
	__u32           magic1;
	bfs_inode_addr  inode_num;
	__u32           uid;
	__u32           gid;
	__u32           mode;
	__u32           flags;
	bfs_bigtime_t   create_time;
	bfs_bigtime_t   last_modified_time;
	bfs_inode_addr  parent;
	bfs_inode_addr  attributes;
	__u32           type;

	__u32           inode_size;
	__u32           etc; /* not use */

	union {
		bfs_data_stream	 datastream;
		char             symlink[BFS_SYMLINK_LEN];
	} data;
	__u32            pad[4]; /* not use */
} __attribute__ ((packed)) bfs_inode;


typedef struct _bfs_index_entry {
	__u32      magic;
	__u32      node_size;
	__u32      max_number_of_levels;
	__u32      data_type;
	bfs_off_t  root_node_pointer;
	bfs_off_t  free_node_pointer;
	bfs_off_t  maximum_size;
} __attribute__ ((packed)) bfs_index_entry;
	

typedef struct _bfs_index_node {
	bfs_off_t left;
	bfs_off_t right;
	bfs_off_t overflow;
	__u16     all_key_count;
	__u16     all_key_length;
} __attribute__ ((packed)) bfs_index_node;


typedef struct _bfs_mount_options {
	gid_t	gid;
	uid_t	uid;
	int	bfs_type;
	char *  iocharset;
} bfs_mount_options;


#ifdef __KERNEL__
/*
 * Function prototypes
 */

/* file.c */
extern int bfs_read (struct inode *, struct file *, char *, int);
extern bfs_inode_addr bfs_read_data_stream (struct super_block *,
	bfs_data_stream *, int *);

/* inode.c */
extern int bfs_bmap (struct inode *, int);
extern void bfs_read_inode (struct inode *);
extern struct buffer_head * bfs_bread (struct inode *);
extern struct buffer_head * bfs_bread2 (struct super_block *, bfs_inode_addr);
extern void bfs_write_inode (struct inode *);
extern int bfs_sync_inode (struct inode *);

/* namei.c */
extern void bfs_release (struct inode *, struct file *);
extern int bfs_lookup (struct inode *, struct dentry *);

/* super.c */
extern void bfs_put_super (struct super_block *);
extern int bfs_remount (struct super_block *, int *, char *);
extern struct super_block * bfs_read_super (struct super_block *,void *,int);
extern int init_bfs_fs(void);
extern int bfs_statfs (struct super_block *, struct statfs *, int);

/* index.c */
extern char * bfs_get_key_from_index_node (bfs_index_node *, int *, int,
	char *, int *, bfs_off_t *);
extern struct buffer_head * bfs_read_index_node (bfs_inode_addr,
	struct super_block *, int, bfs_off_t *);

/* debug.c */
extern void bfs_dump_super_block (bfs_super_block *);
extern void bfs_dump_inode (bfs_inode *);
extern void bfs_dump_index_entry (bfs_index_entry *);
extern void bfs_dump_index_node (bfs_index_node *);

/* util.c */
extern char * bfs_utf2nls (char *, int, char **, int *, struct super_block *);
extern char * bfs_nls2utf (char *, int, char **, int *, struct super_block *);

/*
 * Inodes and files operations
 */

/* dir.c */
extern struct inode_operations bfs_dir_inode_operations;

/* file.c */
extern struct inode_operations bfs_file_inode_operations;

/* symlink.c */
extern struct inode_operations bfs_symlink_inode_operations;

#endif  /* __KERNEL__ */



#endif
