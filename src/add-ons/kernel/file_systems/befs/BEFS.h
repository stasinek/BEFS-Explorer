/* bfs - BEFS definitions and helper functions
 *
 * Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de.
 * Parts of this code is based on work previously done by Marcus Overhagen.
 *
 * This file may be used under the terms of the MIT License.
 */
//---------------------------------------------------------------------------
#ifndef BEFS_H
#define BEFS_H
//---------------------------------------------------------------------------
#include "HAIKU_Defs.h"
#include "WINNT_Defs.h"
#include "BEFS_Endian.h"
//---------------------------------------------------------------------------
#ifdef _BOOT_MODE
//namespace BEFS {
#endif
//---------------------------------------------------------------------------
// ToDo: temporary fix! (missing but public ioctls)
#define IOCTL_FILE_UNCACHED_IO	10000
//---------------------------------------------------------------------------
struct block_run {
    int32_t		allocation_group;
    uint16_t		start;
    uint16_t		length;

    int32_t AllocationGroup() const { return /*BEFS_ENDIAN_TO_HOST_INT32*/(allocation_group); }
    uint16_t Start() const { return BEFS_ENDIAN_TO_HOST_INT16(start); }
    uint16_t Length() const { return BEFS_ENDIAN_TO_HOST_INT16(length); }
    inline bool operator==(const block_run &run) const;
    inline bool operator!=(const block_run &run) const;
    inline bool IsZero() const;
    inline bool MergeableWith(block_run run) const;
    inline void SetTo(int32_t group, uint16_t start, uint16_t length = 1);
    inline static block_run Run(int32_t group, uint16_t start, uint16_t length = 1);
    // can't have a constructor because it's used in a union
}/* _PACKED*/;

typedef block_run inode_addr;

// Since the block_run::length field spans 16 bits, the largest number of
// blocks covered by a block_run is 65535 (as long as we don't want to
// break compatibility and take a zero length for 65536).
#define MAX_BLOCK_RUN_LENGTH	65535

//**************************************


#define BEFS_DISK_NAME_LENGTH	32

struct disk_super_block {
        char		name[BEFS_DISK_NAME_LENGTH];
    int32_t		magic1;
    int32_t		fs_byte_order;
    uint32_t		block_size;
    uint32_t		block_shift;
        int64_t		num_blocks;
        int64_t		used_blocks;
    int32_t		inode_size;
    int32_t		magic2;
    int32_t		blocks_per_ag;
    int32_t		ag_shift;
    int32_t		num_ags;
    int32_t		flags;
    block_run	log_blocks;
        int64_t		log_start;
        int64_t		log_end;
    int32_t		magic3;
    inode_addr	root_dir;
    inode_addr	indices;
    int32_t		pad[8];

        int32_t Magic1() const { return BEFS_ENDIAN_TO_HOST_INT32(magic1); }
        int32_t Magic2() const { return BEFS_ENDIAN_TO_HOST_INT32(magic2); }
        int32_t Magic3() const { return BEFS_ENDIAN_TO_HOST_INT32(magic3); }
        int32_t ByteOrder() const { return BEFS_ENDIAN_TO_HOST_INT32(fs_byte_order); }
        uint32_t BlockSize() const { return BEFS_ENDIAN_TO_HOST_INT32(block_size); }
        uint32_t BlockShift() const { return BEFS_ENDIAN_TO_HOST_INT32(block_shift); }
        int64_t NumBlocks() const { return BEFS_ENDIAN_TO_HOST_INT64(num_blocks); }
        int64_t UsedBlocks() const { return BEFS_ENDIAN_TO_HOST_INT64(used_blocks); }
        int32_t InodeSize() const { return BEFS_ENDIAN_TO_HOST_INT32(inode_size); }
        int32_t BlocksPerAllocationGroup() const { return BEFS_ENDIAN_TO_HOST_INT32(blocks_per_ag); }
        int32_t AllocationGroups() const { return BEFS_ENDIAN_TO_HOST_INT32(num_ags); }
        int32_t AllocationGroupShift() const { return BEFS_ENDIAN_TO_HOST_INT32(ag_shift); }
        int32_t Flags() const { return BEFS_ENDIAN_TO_HOST_INT32(flags); }
        int64_t LogStart() const { return BEFS_ENDIAN_TO_HOST_INT64(log_start); }
        int64_t LogEnd() const { return BEFS_ENDIAN_TO_HOST_INT64(log_end); }

    // implemented in Volume.cpp:
    bool IsValid();
        void Initialize(const char *name, int64_t numBlocks, uint32_t blockSize);
}/* _PACKED*/;

#define SUPER_BLOCK_FS_LENDIAN		'BIGE'		/* BIGE */

#define BEFS_SUPER_BLOCK_MAGIC1			'BFS1'		/* BEFS1 */
#define BEFS_SUPER_BLOCK_MAGIC2			0xdd121031
#define BEFS_SUPER_BLOCK_MAGIC3			0x15b6830e

#define SUPER_BLOCK_MAGIC1			0x534b5931		/* 'SKY1' */
#define SUPER_BLOCK_MAGIC2			0x33551214
#define SUPER_BLOCK_MAGIC3			0x12149977

#define SUPER_BLOCK_DISK_CLEAN		'CLEN'		/* CLEN */
#define SUPER_BLOCK_DISK_DIRTY		'DIRT'		/* DIRT */

//**************************************

#define NUM_DIRECT_BLOCKS			12

struct data_stream {
    block_run	direct[NUM_DIRECT_BLOCKS];
        int64_t		max_direct_range;
    block_run	indirect;
        int64_t		max_indirect_range;
    block_run	double_indirect;
        int64_t		max_double_indirect_range;
        int64_t		size;

        int64_t MaxDirectRange() const { return BEFS_ENDIAN_TO_HOST_INT64(max_direct_range); }
        int64_t MaxIndirectRange() const { return BEFS_ENDIAN_TO_HOST_INT64(max_indirect_range); }
        int64_t MaxDoubleIndirectRange() const { return BEFS_ENDIAN_TO_HOST_INT64(max_double_indirect_range); }
        int64_t Size() const { return BEFS_ENDIAN_TO_HOST_INT64(size); }
}/* _PACKED*/;

// This defines the size of the indirect and double indirect
// blocks. Note: the code may not work correctly at some places
// if this value is changed (it's not tested).
#define NUM_ARRAY_BLOCKS		4
#define ARRAY_BLOCKS_SHIFT		2
#define INDIRECT_BLOCKS_SHIFT	(ARRAY_BLOCKS_SHIFT + ARRAY_BLOCKS_SHIFT)

//**************************************

struct bfs_inode;

struct small_data {
    uint32_t		type;
    uint16_t		name_size;
    uint16_t		data_size;
    char		name[0];	// name_size long, followed by data

        uint32_t Type() const { return BEFS_ENDIAN_TO_HOST_INT32(type); }
        uint16_t NameSize() const { return BEFS_ENDIAN_TO_HOST_INT16(name_size); }
        uint16_t DataSize() const { return BEFS_ENDIAN_TO_HOST_INT16(data_size); }

    inline char		*Name() const;
    inline uint8_t	*Data() const;
    inline uint32_t	Size() const;
    inline small_data *Next() const;
    inline bool		IsLast(const bfs_inode *inode) const;
} /*_PACKED*/;

// the file name is part of the small_data structure
#define FILE_NAME_TYPE			'CSTR'
#define FILE_NAME_NAME			0x13
#define FILE_NAME_NAME_LENGTH	1


//**************************************

class Volume;

#define SHORT_SYMLINK_NAME_LENGTH	144 // length incl. terminating '\0'

struct bfs_inode {
#pragma pack(1)
    int32_t		magic1;
    inode_addr	inode_num;
    int32_t		uid;
    int32_t		gid;
    int32_t		mode;				// see sys/stat.h
    int32_t		flags;
    bigtime_t	create_time;
    bigtime_t	last_modified_time;
    inode_addr	parent;
    inode_addr	attributes;
    uint32_t		type;				// attribute type

    int32_t		inode_size;
    uint32_t		etc;				// a pointer to the Inode object during construction

    union {
        data_stream		data;
        char 			short_symlink[SHORT_SYMLINK_NAME_LENGTH];
    };
    int32_t		pad[4];
#ifndef WIN32
    small_data	small_data_start[0];
#endif // WIN32
        int32_t Magic1() const { return BEFS_ENDIAN_TO_HOST_INT32(magic1); }
        int32_t UserID() const { return BEFS_ENDIAN_TO_HOST_INT32(uid); }
        int32_t GroupID() const { return BEFS_ENDIAN_TO_HOST_INT32(gid); }
        int32_t Mode() const { return BEFS_ENDIAN_TO_HOST_INT32(mode); }
        int32_t Flags() const { return BEFS_ENDIAN_TO_HOST_INT32(flags); }
        int32_t Type() const { return BEFS_ENDIAN_TO_HOST_INT32(type); }
        int32_t InodeSize() const { return BEFS_ENDIAN_TO_HOST_INT32(inode_size); }
        bigtime_t LastModifiedTime() const { return BEFS_ENDIAN_TO_HOST_INT64(last_modified_time); }
        bigtime_t CreateTime() const { return BEFS_ENDIAN_TO_HOST_INT64(create_time); }
#ifndef WIN32
    small_data *SmallDataStart() { return small_data_start; }
#else
    small_data *SmallDataStart(){ return (small_data *)(&pad[4]);};
#endif // WIN32

    status_t InitCheck(Volume *volume);
        // defined in Inode.cpp
}/*_PACKED*/;

#define INODE_MAGIC1			0x3bbe0ad9
#define INODE_TIME_SHIFT		16
#define INODE_TIME_MASK			0xffff
#define INODE_FILE_NAME_LENGTH	256

enum inode_flags {
    INODE_IN_USE			= 0x00000001,	// always set
    INODE_ATTR_INODE		= 0x00000004,
    INODE_LOGGED			= 0x00000008,	// log changes to the data stream
    INODE_DELETED			= 0x00000010,
    INODE_NOT_READY			= 0x00000020,	// used during Inode construction
    INODE_LONG_SYMLINK		= 0x00000040,	// symlink in data stream

    INODE_PERMANENT_FLAGS	= 0x0000ffff,

    INODE_WAS_WRITTEN		= 0x00020000,
    INODE_NO_TRANSACTION	= 0x00040000,
    INODE_DONT_FREE_SPACE	= 0x00080000,	// only used by the "chkbfs" functionality
        INODE_CHKBEFS_RUNNING	= 0x00200000,
};

//**************************************

struct file_cookie {
    bigtime_t last_notification;
        int64_t	last_size;
    //size_t	last_size;
    int		open_mode;
};

// notify every second if the file size has changed
#define INODE_NOTIFICATION_INTERVAL	1000000LL

//**************************************


inline int32_t divide_roundup(int32_t num,int32_t divisor)
{
    return (num + divisor - 1) / divisor;
}

inline int64_t divide_roundup(int64_t num,int32_t divisor)
{
    return (num + divisor - 1) / divisor;
}

inline int
get_shift(uint64_t i)
{
    int c;
    c = 0;
    while (i > 1) {
        i >>= 1;
        c++;
    }
    return c;
}

inline int32_t
round_up(uint32_t data)
{
    // rounds up to the next uint64_t boundary
    return (data + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1);
}


/************************ block_run inline functions ************************/
//	#pragma mark -


inline bool
block_run::operator==(const block_run &run) const
{
    return allocation_group == run.allocation_group
        && start == run.start
        && length == run.length;
}


inline bool
block_run::operator!=(const block_run &run) const
{
    return allocation_group != run.allocation_group
        || start != run.start
        || length != run.length;
}


inline bool
block_run::IsZero() const
{
    return allocation_group == 0 && start == 0 && length == 0;
}


inline bool
block_run::MergeableWith(block_run run) const
{
    // 65535 is the maximum allowed run size for BEFS
    return allocation_group == run.allocation_group
        && Start() + Length() == run.Start()
        && (uint32_t)Length() + run.Length() <= MAX_BLOCK_RUN_LENGTH;
}


inline void
block_run::SetTo(int32_t _group,uint16_t _start,uint16_t _length)
{
        allocation_group = HOST_ENDIAN_TO_BEFS_INT32(_group);
        start = HOST_ENDIAN_TO_BEFS_INT16(_start);
        length = HOST_ENDIAN_TO_BEFS_INT16(_length);
}


inline block_run
block_run::Run(int32_t group, uint16_t start, uint16_t length)
{
    block_run run;
        run.allocation_group = HOST_ENDIAN_TO_BEFS_INT32(group);
        run.start = HOST_ENDIAN_TO_BEFS_INT16(start);
        run.length = HOST_ENDIAN_TO_BEFS_INT16(length);
    return run;
}


/************************ small_data inline functions ************************/
//	#pragma mark -


inline char *
small_data::Name() const
{
    return const_cast<char *>(name);
}


inline uint8_t *
small_data::Data() const
{
    return (uint8_t *)Name() + NameSize() + 3;
}


inline uint32_t
small_data::Size() const
{
    return sizeof(small_data) + NameSize() + 3 + DataSize() + 1;
}


inline small_data *
small_data::Next() const
{
    return (small_data *)((uint8_t *)this + Size());
}


inline bool
small_data::IsLast(const bfs_inode *inode) const
{
    // we need to check the location first, because if name_size is already beyond
    // the block, we would touch invalid memory (although that can't cause wrong
    // results)
    return (uint32_t)this > (uint32_t)inode + inode->InodeSize() - sizeof(small_data) || name_size == 0;
}

#ifdef _BOOT_MODE
//}	// namespace BEFS
#endif

#endif	/* BEFS_H */



