/* BlockAllocator - block bitmap handling and allocation policies
 *
 * Copyright 2001-2006, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
#ifndef BEFS_BLOCK_ALLOCATOR_H
#define BEFS_BLOCK_ALLOCATOR_H

#include "BEFS_Lock.h"

struct disk_super_block;
struct check_control;
struct check_cookie;

class AllocationGroup;
class Transaction;
class Volume;
class Inode;
class BlockAllocator {
	public:
		BlockAllocator(Volume *volume);
		~BlockAllocator();

		status_t Initialize(bool full = true);
		status_t InitializeAndClearBitmap(Transaction &transaction);

		status_t AllocateForInode(Transaction &transaction, const block_run *parent,
					mode_t type, block_run &run);
        status_t Allocate(Transaction &transaction, Inode *inode, int64_t numBlocks,
					block_run &run, uint16_t minimum = 1);
		status_t Free(Transaction &transaction, block_run run);

		status_t AllocateBlocks(Transaction &transaction, int32_t group, uint16_t start,
					uint16_t numBlocks, uint16_t minimum, block_run &run);

		status_t StartChecking(check_control *control);
		status_t StopChecking(check_control *control);
		status_t CheckNextNode(check_control *control);

		status_t CheckBlockRun(block_run run, const char *type = NULL, check_control *control = NULL, bool allocated = true);
		status_t CheckInode(Inode *inode, check_control *control = NULL);

		size_t BitmapSize() const;

	private:
		bool _IsValidCheckControl(check_control *control);
        bool _CheckBitmapIsUsedAt(int64_t block) const;
        void _SetCheckBitmapAt(int64_t block);

		static status_t _Initialize(BlockAllocator *);

		Volume			*fVolume;
		Semaphore		fLock;
		AllocationGroup	*fGroups;
		int32_t			fNumGroups;
		uint32_t			fBlocksPerGroup;

		uint32_t			*fCheckBitmap;
		check_cookie	*fCheckCookie;
};

#endif	/* BLOCK_ALLOCATOR_H */
