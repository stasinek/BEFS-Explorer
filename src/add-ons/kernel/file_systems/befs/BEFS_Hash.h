/*
** Copyright 2001-2002, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/
//---------------------------------------------------------------------------
#ifndef BEFS_HASH_H
#define BEFS_HASH_H
//---------------------------------------------------------------------------
#include "BEFS.h"
//---------------------------------------------------------------------------
// can be allocated on the stack
typedef struct hash_iterator {
    void *current;
    int bucket;
} hash_iterator;
//---------------------------------------------------------------------------
typedef struct hash_table hash_table;
extern struct hash_table *hash_init(uint32_t table_size, int next_ptr_offset,
    int compare_func(void *element, const void *key),
    uint32_t hash_func(void *element, const void *key, uint32_t range));
extern int hash_uninit(struct hash_table *table);
extern status_t hash_insert(struct hash_table *table, void *_element);
extern status_t hash_remove(struct hash_table *table, void *_element);
extern void hash_remove_current(struct hash_table *table, struct hash_iterator *iterator);
extern void *hash_remove_first(struct hash_table *table, uint32_t *_cookie);
extern void *hash_find(struct hash_table *table, void *e);
extern void *hash_lookup(struct hash_table *table, const void *key);
extern struct hash_iterator *hash_open(struct hash_table *table, struct hash_iterator *i);
extern void hash_close(struct hash_table *table, struct hash_iterator *i, bool free_iterator);
extern void *hash_next(struct hash_table *table, struct hash_iterator *i);
extern void hash_rewind(struct hash_table *table, struct hash_iterator *i);

/* function pointers must look like this:
 *
 * uint32_t hash_func(void *e, const void *key, uint32_t range);
 *		hash function should calculate hash on either e or key,
 *		depending on which one is not NULL - they also need
 *		to make sure the returned value is within range.
 * int compare_func(void *e, const void *key);
 *		compare function should compare the element with
 *		the key, returning 0 if equal, other if not
 */

extern uint32_t hash_hash_string(const char *str);

//}	// namespace HaikuKernelEmu
//}	// namespace UserlandFS

#endif	/* HASH_H */
