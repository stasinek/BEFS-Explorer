/* Utility - some helper classes
 *
 * Copyright 2001-2005, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
//---------------------------------------------------------------------------
#ifndef BEFS_UTILITY_H
#define BEFS_UTILITY_H
//---------------------------------------------------------------------------
#include "BEFS.h"
//---------------------------------------------------------------------------
// Simple array, used for the duplicate handling in the B+Tree,
// and for the log entries.
//---------------------------------------------------------------------------

struct sorted_array {
    public:
        int64_t	count;
        int64_t	values[0];

        inline int32_t Find(int64_t value) const;
        void Insert(int64_t value);
        bool Remove(int64_t value);

    private:
        bool FindInternal(int64_t value,int32_t &index) const;
};


inline int32_t
sorted_array::Find(int64_t value) const
{
    int32_t i;
    return FindInternal(value,i) ? i : -1;
}

#endif	/* UTILITY_H */
