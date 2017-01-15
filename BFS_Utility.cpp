/* Utility - some helper classes
 *
 * Copyright 2001-2005, Axel Dörfler, axeld@pinc-software.de.
 * This file may be used under the terms of the MIT License.
 */


#include "BFS_Utility.h"
#include "BFS_Debug.h"


bool
sorted_array::FindInternal(uint64 value, int32 &index) const
{
	int32 min = 0, max = count-1;
	uint64 cmp;
	while (min <= max) {
		index = (min + max) / 2;

		cmp = values[index] - value;
		if (cmp < 0)
			min = index + 1;
		else if (cmp > 0)
			max = index - 1;
		else
			return true;
	}
	return false;
}


void 
sorted_array::Insert(uint64 value)
{
	// if there are more than 8 values in this array, use a
	// binary search, if not, just iterate linearly to find
	// the insertion point
	int32 i;
	if (count > 8 ) {
		if (!FindInternal(value,i)
			&& values[i] <= value)
			i++;
	} else {
		for (i = 0;i < count; i++)
			if (values[i] > value)
				break;
	}

	memmove(&values[i+1],&values[i],(count - i) * sizeof(uint64));
	values[i] = value;
	count++;
}


bool 
sorted_array::Remove(uint64 value)
{
	int32 index = Find(value);
	if (index == -1)
		return false;

	memmove(&values[index],&values[index + 1],(count - index) * sizeof(uint64));
	count--;

	return true;
}

