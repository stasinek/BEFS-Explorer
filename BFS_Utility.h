/* Utility - some helper classes
 *
 * Copyright 2001-2005, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */
#ifndef BFS_UTILITY_H
#define BFS_UTILITY_H


#include "BOS_System_Dependencies.h"


// Simple array, used for the duplicate handling in the B+Tree,
// and for the log entries.

struct sorted_array {
	public:
		uint64	count;
		uint64	values[0];

		inline int32 Find(uint64 value) const;
		void Insert(uint64 value);
		bool Remove(uint64 value);

	private:
		bool FindInternal(uint64 value,int32 &index) const;
};


inline int32
sorted_array::Find(uint64 value) const
{
	int32 i;
	return FindInternal(value,i) ? i : -1;
}

#endif	/* UTILITY_H */
