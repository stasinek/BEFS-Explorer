/*
 * Copyright 2003-2004, Axel Dörfler, axeld@pinc-software.de.
 * Distributed under the terms of the MIT License.
 */
//---------------------------------------------------------------------------
#ifndef BEFS_ENDIAN_H
#define BEFS_ENDIAN_H
//---------------------------------------------------------------------------
#	define BEFS_ENDIAN_TO_HOST_INT16(value) value
#	define BEFS_ENDIAN_TO_HOST_INT32(value) value
#	define BEFS_ENDIAN_TO_HOST_INT64(value) value
#	define HOST_ENDIAN_TO_BEFS_INT16(value) value
#	define HOST_ENDIAN_TO_BEFS_INT32(value) value
#	define HOST_ENDIAN_TO_BEFS_INT64(value) value
//---------------------------------------------------------------------------
#if !defined(BEFS_LITTLE_ENDIAN_ONLY) && !defined(BEFS_BIG_ENDIAN_ONLY)
//	default setting; BEFS is now primarily a little endian file system
#	define BEFS_LITTLE_ENDIAN_ONLY
#endif
//---------------------------------------------------------------------------
#if defined(BEFS_LITTLE_ENDIAN_ONLY) && B_HOST_IS_LENDIAN \
        || defined(BEFS_BIG_ENDIAN_ONLY) && B_HOST_IS_BENDIAN
		/* host is BEFS endian */
#	define BEFS_NATIVE_ENDIAN
#	define BEFS_ENDIAN_TO_HOST_INT16(value) value
#	define BEFS_ENDIAN_TO_HOST_INT32(value) value
#	define BEFS_ENDIAN_TO_HOST_INT64(value) value
#	define HOST_ENDIAN_TO_BEFS_INT16(value) value
#	define HOST_ENDIAN_TO_BEFS_INT32(value) value
#	define HOST_ENDIAN_TO_BEFS_INT64(value) value
#elif defined(BEFS_LITTLE_ENDIAN_ONLY) && B_HOST_IS_BENDIAN \
        || defined(BEFS_BIG_ENDIAN_ONLY) && B_HOST_IS_LENDIAN
		/* host is big endian, BEFS is little endian or vice versa */
#	define BEFS_ENDIAN_TO_HOST_INT16(value) __swap_int16(value)
#	define BEFS_ENDIAN_TO_HOST_INT32(value) __swap_int32(value)
#	define BEFS_ENDIAN_TO_HOST_INT64(value) __swap_int64(value)
#	define HOST_ENDIAN_TO_BEFS_INT16(value) __swap_int16(value)
#	define HOST_ENDIAN_TO_BEFS_INT32(value) __swap_int32(value)
#	define HOST_ENDIAN_TO_BEFS_INT64(value) __swap_int64(value)
#else
	// ToDo: maybe build a version that supports both, big & little endian?
	//		But since that will need some kind of global data (to
	//		know of what type this file system is), it's probably 
	//		something for the boot loader; anything else would be
	//		a major pain.
#endif
//---------------------------------------------------------------------------
#endif	/* BEFS_ENDIAN_H */
//---------------------------------------------------------------------------
