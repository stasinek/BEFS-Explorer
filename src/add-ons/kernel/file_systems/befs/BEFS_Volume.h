/* Volume - BEFS super block, mounting, etc.
 *
 * Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
#ifndef VOLUME_H
#define VOLUME_H
#include "BEFS_BlockAllocator.h"
#include "BEFS_Chain.h"
#include "BEFS_Cache.h"
#include "BEFS.h"

class Journal;
class Inode;
class Query;

enum volume_flags {
    VOLUME_READ_ONLY	= 0x0001
};

enum volume_initialize_flags {
    VOLUME_NO_INDICES	= 0x0001,
};

#define PART_TABLE 0
#define BOOT_RECORD 1
#define EXTENDED_PART 2

#define PART_UNKNOWN 0x00		//Unknown.
#define PART_DOS2_FAT 0x01		//12-bit FAT.
#define PART_DOS3_FAT 0x04		//16-bit FAT. Partition smaller than 32MB.
#define PART_EXTENDED 0x05		//Extended MS-DOS Partition.
#define PART_DOS4_FAT 0x06		//16-bit FAT. Partition larger than or equal to 32MB.
#define PART_DOS32 0x0B			//32-bit FAT. Partition up to 2047GB.
#define PART_DOS32X 0x0C		//Same as PART_DOS32(0Bh), but uses Logical Block Address Int 13h extensions.
#define PART_DOSX13 0x0E		//Same as PART_DOS4_FAT(06h), but uses Logical Block Address Int 13h extensions.
#define PART_DOSX13X 0x0F		//Same as PART_EXTENDED(05h), but uses Logical Block Address Int 13h extensions.
#define PART_SKYFS 0xEC			//64 bit SkyFS partition
#define PART_EXT2 0x83			//EXT2/EXT3 Linux partition
#define PART_SWAP 0x82			//Linux swap partition
#define PART_NTFS 0x07			//NTFS partition
#define PART_BEFS 0xEB			//64 bit BEFS partition, it should be this one
#define PART_BEFS1 46			//64 bit BEFS partition, WHY IS IT THIS ONE????

typedef struct
{
    BYTE	chBootInd;
    BYTE	chHead;
    BYTE	chSector;
    BYTE	chCylinder;
    BYTE	chType;
    BYTE	chLastHead;
    BYTE	chLastSector;
    BYTE	chLastCylinder;
    DWORD	dwRelativeSector;
    DWORD	dwNumberSectors;

}PARTITION;

typedef struct
{
    WORD	wCylinder;
    WORD	wHead;
    WORD	wSector;
    DWORD	dwNumSectors;
    WORD	wType;
    DWORD	dwRelativeSector;
    DWORD	dwNTRelativeSector;
    DWORD	dwBytesPerSector;

}DRIVEPACKET;

class Volume {
    public:
        Volume(mount_id id);
        //Volume(const char* device);
        ~Volume();

        status_t			Mount(int diskNr, int partitionNr, uint32_t flags);

        // next 3 will be removed
        status_t			Mount(const char *device, int partitionNr, uint32_t flags);
        status_t			MountBEFS(const char *deviceName, int partitionNr, uint32_t flags);
        int					CountSkyfsPartitions(const char* device, bool skyfs);
        // till here
        status_t			Unmount();
        status_t			Initialize(const char *device, const char *name,
                                uint32_t blockSize, uint32_t flags);

        bool				IsValidSuperBlock();
        bool				IsReadOnly() const;
        void				Panic();
        RecursiveLock		&Lock();

        block_run			Root() const { return fSuperBlock.root_dir; }
        Inode				*RootNode() const { return fRootNode; }
        block_run			Indices() const { return fSuperBlock.indices; }
        Inode				*IndicesNode() const { return fIndicesNode; }
        block_run			Log() const { return fSuperBlock.log_blocks; }
        int32_t				&LogStart() { return fLogStart; }
        int32_t				&LogEnd() { return fLogEnd; }
        HANDLE				Device() const { return fDevice; }

        mount_id			ID() const { return fID; }
        const char			*Name() const { return fSuperBlock.name; }

        int64_t				NumBlocks() const { return fSuperBlock.NumBlocks(); }
        int64_t				UsedBlocks() const { return fSuperBlock.UsedBlocks(); }
        int64_t				FreeBlocks() const { return NumBlocks() - UsedBlocks(); }

        uint32_t				BlockSize() const { return fBlockSize; }
        uint32_t				BlockShift() const { return fBlockShift; }
        uint32_t				InodeSize() const { return fSuperBlock.InodeSize(); }
        uint32_t				AllocationGroups() const { return fSuperBlock.AllocationGroups(); }
        uint32_t				AllocationGroupShift() const { return fAllocationGroupShift; }
        disk_super_block	&SuperBlock() { return fSuperBlock; }

        int64_t				ToOffset(block_run run) const { return /*fStartPos.QuadPart+*/ (ToBlock(run) << BlockShift()); }
        int64_t				ToBlock(block_run run) const { return ((((int64_t)run.AllocationGroup()) << AllocationGroupShift()) | (int64_t)run.Start()); }
        block_run			ToBlockRun(int64_t block) const;
        status_t			ValidateBlockRun(block_run run);

        int64_t				ToVnode(block_run run) const { return ToBlock(run); }
        int64_t				ToVnode(int64_t block) const { return block; }
        int64_t				VnodeToBlock(vnode_id id) const { return (int64_t)id; }

        status_t			CreateIndicesRoot(Transaction &transaction);

        // block bitmap
        BlockAllocator		&Allocator();
        status_t			AllocateForInode(Transaction &transaction, const Inode *parent,
                                mode_t type, block_run &run);
        status_t			AllocateForInode(Transaction &transaction, const block_run *parent,
                                mode_t type, block_run &run);
        status_t			Allocate(Transaction &transaction, Inode *inode,
                                int64_t numBlocks, block_run &run, uint16_t minimum = 1);
        status_t			Free(Transaction &transaction, block_run run);

        // cache access
        status_t			WriteSuperBlock();
        status_t			FlushDevice();

        // queries
        void				UpdateLiveQueries(Inode *inode, const char *attribute, int32_t type,
                                const uint8_t *oldKey, size_t oldLength,
                                const uint8_t *newKey, size_t newLength);
        bool				CheckForLiveQuery(const char *attribute);
        void				AddQuery(Query *query);
        void				RemoveQuery(Query *query);

        status_t			Sync();
        Journal				*GetJournal(int64_t refBlock) const;

        void				*BlockCache() { return fBlockCache; }

        uint32_t				GetUniqueID();

        static status_t		Identify(HANDLE fd, disk_super_block *superBlock);
        int64_t				StartPos() const {return fStartPos.QuadPart; }

    protected:
        mount_id			fID;
        HANDLE				fDevice;
        disk_super_block	fSuperBlock;

        uint32_t				fBlockSize;
        uint32_t				fBlockShift;
        uint32_t				fAllocationGroupShift;

        BlockAllocator		fBlockAllocator;
        RecursiveLock		fLock;
        Journal				*fJournal;
        int32_t				fLogStart, fLogEnd;

        Inode				*fRootNode;
        Inode				*fIndicesNode;

        int32_t				fDirtyCachedBlocks;

        SimpleLock			fQueryLock;
        Chain<Query>		fQueries;

        int32_t				fUniqueID;
        uint32_t				fFlags;

        void				*fBlockCache;

        PARTITION			diskInfo;
        DRIVEPACKET			partitionInfo;
        // TODO: using static here is VERY VERY BAD DESIGN
        LARGE_INTEGER		fStartPos;  //keep the start sector of this partition of the drive
        int					skyfsNr;

};
void dump_block_run2(const char *prefix, const block_run &run);
void dump_data_stream2(const data_stream *stream);
char * get_tupel2(uint32_t id);
void dump_inode2(const bfs_inode *inode);

// inline functions

inline bool
Volume::IsReadOnly() const
{
     return fFlags & VOLUME_READ_ONLY;
}


inline RecursiveLock &
Volume::Lock()
{
     return fLock;
}


inline BlockAllocator &
Volume::Allocator()
{
     return fBlockAllocator;
}


inline status_t
Volume::AllocateForInode(Transaction &transaction, const block_run *parent, mode_t type, block_run &run)
{
    return fBlockAllocator.AllocateForInode(transaction, parent, type, run);
}


inline status_t
Volume::Allocate(Transaction &transaction, Inode *inode, int64_t numBlocks, block_run &run, uint16_t minimum)
{
    return fBlockAllocator.Allocate(transaction, inode, numBlocks, run, minimum);
}


inline status_t
Volume::Free(Transaction &transaction, block_run run)
{
    return fBlockAllocator.Free(transaction, run);
}


inline status_t
Volume::FlushDevice()
{
    return block_cache_sync(fBlockCache);
}


inline Journal *
Volume::GetJournal(int64_t /*refBlock*/) const
{
     return fJournal;
}


inline uint32_t
Volume::GetUniqueID()
{
     return atomic_add(&fUniqueID, 1);
}

#endif	/* VOLUME_H */

