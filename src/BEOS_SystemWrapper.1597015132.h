/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 */
#ifndef BEOS_SYSTEM_H
#define BEOS_SYSTEM_H
//
#include <BEOS_Defs.h>

#ifndef _IMPEXP_KERNEL
#define _IMPEXP_KERNEL
#endif

//
// UID, GID
//
gid_t getegid(void);
uid_t geteuid(void);

// ?
int read_pos(HANDLE fDevice, int64_t sStart, void* data, unsigned int size);
int read_pos2(HANDLE fDevice, int64_t sStart, void* data, unsigned int size);
int write_pos(HANDLE fDevice, int64_t sStart, void* data, unsigned int size);
ssize_t	readv_pos(HANDLE fd, int64_t pos, const struct iovec *vec, size_t count);
ssize_t	writev_pos(HANDLE fd, int64_t pos, const struct iovec *vec,size_t count);
//
// Threads
//
status_t resume_thread(thread_id id);
//
// Threads::Semaphore
//
status_t acquire_sem(sem_id sem);
status_t acquire_sem_etc(sem_id sem, uint32_t count, uint32_t flags, bigtime_t timeout);
sem_id create_sem(uint32_t thread_count, const char *name);
status_t delete_sem(sem_id sem);
status_t release_sem(sem_id sem);
status_t release_sem_etc(sem_id sem, int32_t count, uint32_t flags);
//
// Threads::Atomic
//
int32_t	atomic_add (int32_t *value, int32_t addValue);
int32_t	atomic_set (int32_t *value, int32_t newValue);
//
// V-node
//
status_t get_vnode(int fs, vnode_id id, void **_vnode);
status_t publish_vnode (unsigned int mountID, int64_t vnodeID, void* privateNode);
status_t new_vnode (unsigned int mountID, int64_t vnodeID, void* privateNode);
status_t remove_vnode (unsigned int mountID, int64_t vnodeID);
status_t unremove_vnode (unsigned int mountID, int64_t vnodeID);
status_t put_vnode (unsigned int mountID, int64_t vnodeID);
//
// Query
//
void notify_query_entry_created(int a, int b, unsigned int c,int64_t d, const char* e, int64_t f);
void notify_query_entry_removed(int a, int b, unsigned int c,int64_t d, const char* e, int64_t f);
//
// Cache Callback
//
typedef void (*tFoo)(int f, void*ff);
//
// Cache
//
status_t cache_next_block_in_transaction(void* a, int,unsigned int* b, int64_t * c, int d, int e);
void cache_abort_sub_transaction(void* d, int id);
void cache_abort_transaction(void* d, int id);
int32_t cache_blocks_in_transaction(void* d, int id);
void cache_end_transaction(void* d, int id, tFoo f, void*);
int cache_start_sub_transaction(void* d, int id);
int cache_start_transaction(void* d);

#endif // BEOS_SYSTEM_H

