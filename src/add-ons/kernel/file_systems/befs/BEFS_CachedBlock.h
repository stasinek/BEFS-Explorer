/* CachedBlock - interface for the block cache
 *
 * Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
//---------------------------------------------------------------------------
#ifndef BEFS_CACHED_BLOCK_H
#define BEFS_CACHED_BLOCK_H
//---------------------------------------------------------------------------
#include "BEFS.h"
#include "BEFS_Volume.h"
#include "BEFS_Journal.h"
#include "BEFS_Lock.h"
#include "BEFS_Chain.h"
#include "BEFS_Debug.h"
//---------------------------------------------------------------------------
// The CachedBlock class is completely implemented as inlines.
// It should be used when cache single blocks to make sure they
// will be properly released after use (and it's also very
// convenient to use them).
//---------------------------------------------------------------------------

class CachedBlock {
    public:
        CachedBlock(Volume *volume);
        CachedBlock(Volume *volume, int64_t block);
        CachedBlock(Volume *volume, block_run run);
        CachedBlock(CachedBlock *cached);
        ~CachedBlock();

        inline void Keep();
        inline void Unset();

        inline const uint8_t *SetTo(int64_t block, int64_t base, size_t length);
        inline const uint8_t *SetTo(int64_t block);
        inline const uint8_t *SetTo(block_run run);
        inline uint8_t *SetToWritable(Transaction &transaction, int64_t block, int64_t base, size_t length, bool empty = false);
        inline uint8_t *SetToWritable(Transaction &transaction, int64_t block, bool empty = false);
        inline uint8_t *SetToWritable(Transaction &transaction, block_run run, bool empty = false);
        inline status_t MakeWritable(Transaction &transaction);

        const uint8_t *Block() const { return fBlock; }
        int64_t BlockNumber() const { return fBlockNumber; }
        uint32_t BlockSize() const { return fVolume->BlockSize(); }
        uint32_t BlockShift() const { return fVolume->BlockShift(); }

    private:
        CachedBlock(const CachedBlock &);
        CachedBlock &operator=(const CachedBlock &);
            // no implementation

    protected:
        Volume	*fVolume;
        int64_t	fBlockNumber;
        uint8_t	*fBlock;
};

//---------------------------------------------------------------------------
// inlines
//---------------------------------------------------------------------------

inline CachedBlock::CachedBlock(Volume *volume)
    :
    fVolume(volume),
    fBlockNumber(0),
    fBlock(NULL)
{
}
//---------------------------------------------------------------------------

inline CachedBlock::CachedBlock(Volume *volume, int64_t block)
    :
    fVolume(volume),
    fBlockNumber(0),
    fBlock(NULL)
{
    SetTo(block);
}
//---------------------------------------------------------------------------

inline CachedBlock::CachedBlock(Volume *volume, block_run run)
    :
    fVolume(volume),
    fBlockNumber(0),
    fBlock(NULL)
{
    SetTo(volume->ToBlock(run));
}
//---------------------------------------------------------------------------

inline CachedBlock::CachedBlock(CachedBlock *cached)
    :
    fVolume(cached->fVolume),
    fBlockNumber(cached->BlockNumber()),
    fBlock(cached->fBlock)
{
    cached->Keep();
}
//---------------------------------------------------------------------------

inline CachedBlock::~CachedBlock()
{
    Unset();
}
//---------------------------------------------------------------------------

inline void CachedBlock::Keep()
{
    fBlock = NULL;
}
//---------------------------------------------------------------------------

inline void CachedBlock::Unset()
{
    if (fBlock != NULL)
        block_cache_put(fVolume->BlockCache(), fBlockNumber);
}
//---------------------------------------------------------------------------

inline const uint8_t *CachedBlock::SetTo(int64_t block, int64_t base, size_t length)
{
    Unset();
    fBlockNumber = block;
    return fBlock = (uint8_t *)block_cache_get_etc(fVolume->BlockCache(), block, base, length/*, offset*/);
}
//---------------------------------------------------------------------------

inline const uint8_t *CachedBlock::SetTo(int64_t block)
{
    return SetTo(block, block, 1);
}
//---------------------------------------------------------------------------

inline const uint8_t *CachedBlock::SetTo(block_run run)
{
    return SetTo(fVolume->ToBlock(run));
}
//---------------------------------------------------------------------------

inline uint8_t *CachedBlock::SetToWritable(Transaction &transaction, int64_t block, int64_t base, size_t length, bool empty)
{
    Unset();
    fBlockNumber = block;

    if (empty)
        return fBlock = (uint8_t *)block_cache_get_empty(fVolume->BlockCache(), block, transaction.ID());

    return fBlock = (uint8_t *)block_cache_get_writable_etc(fVolume->BlockCache(),
        block, base, length, transaction.ID());
}
//---------------------------------------------------------------------------

inline uint8_t *CachedBlock::SetToWritable(Transaction &transaction, int64_t block, bool empty)
{
    return SetToWritable(transaction, block, block, 1, empty);
}
//---------------------------------------------------------------------------

inline uint8_t *CachedBlock::SetToWritable(Transaction &transaction, block_run run, bool empty)
{
    return SetToWritable(transaction, fVolume->ToBlock(run), empty);
}
//---------------------------------------------------------------------------

inline status_t CachedBlock::MakeWritable(Transaction &transaction)
{
    if (fBlock == NULL)
        return B_NO_INIT;

    return block_cache_make_writable(fVolume->BlockCache(), fBlockNumber, transaction.ID());
}
//---------------------------------------------------------------------------

#endif	/* CACHED_BLOCK_H */
