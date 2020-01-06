/*
 * linux/fs/befs/compatiblity.h
 *
 * Copyright (C) 2001 Will Dyson <will_dyson@pobox.com>
 *   AKA <will@cs.earlham.edu>
 *
 * This file trys to take care of differences between
 * kernel versions
 */
#define min_t(type,x,y) \
({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })

#define max_t(type,x,y) \
({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

#define vsnprintf(buf, n, fmt, args) vsprintf(buf, fmt, args)

#define uint64_t unsigned __int64 
#define uint32_t unsigned __int32 
#define uint16_t unsigned __int16 
#define __u64 uint64_t 
#define __u32 uint32_t 
#define __u16 uint16_t 
#define u64 uint64_t 
#define u32 uint32_t 
#define u16 uint16_t 

#define loff_t size_t
#define uint size_t
#define gid_t size_t
#define uid_t size_t
 


#define MODULE_LICENSE(x)
