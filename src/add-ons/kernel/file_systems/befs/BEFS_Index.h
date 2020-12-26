/* Index - index access functions
 *
 * Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
#ifndef BEFS_INDEX_H
#define BEFS_INDEX_H

#include "BEFS.h"

class Transaction;
class Volume;
class Inode;


class Index {
    public:
        Index(Volume *volume);
        ~Index();

        status_t SetTo(const char *name);
        void Unset();

        Inode *Node() const { return fNode; };
        uint32_t Type();
        size_t KeySize();

        status_t Create(Transaction &transaction, const char *name, uint32_t type);

        status_t Update(Transaction &transaction, const char *name, int32_t type, const uint8_t *oldKey,
                    uint16_t oldLength, const uint8_t *newKey, uint16_t newLength, Inode *inode);

        status_t InsertName(Transaction &transaction, const char *name, Inode *inode);
        status_t RemoveName(Transaction &transaction, const char *name, Inode *inode);
        status_t UpdateName(Transaction &transaction, const char *oldName, const char *newName,
                    Inode *inode);

        status_t InsertSize(Transaction &transaction, Inode *inode);
        status_t RemoveSize(Transaction &transaction, Inode *inode);
        status_t UpdateSize(Transaction &transaction, Inode *inode);

        status_t InsertLastModified(Transaction &transaction, Inode *inode);
        status_t RemoveLastModified(Transaction &transaction, Inode *inode);
        status_t UpdateLastModified(Transaction &transaction, Inode *inode, int64_t modified = -1);

    private:
        Index(const Index &);
        Index &operator=(const Index &);
            // no implementation

        Volume		*fVolume;
        Inode		*fNode;
        const char	*fName;
};

#endif	/* INDEX_H */
