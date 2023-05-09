/* bfs_control - additional functionality exported via ioctl()
**
** Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de
** This file may be used under the terms of the OpenBeOS License.
*/
#ifndef BEFS_CONTROL_H
#define BEFS_CONTROL_H
//---------------------------------------------------------------------------
#include "BEFS.h"
//---------------------------------------------------------------------------
/* ioctl to check the version of BEFS used - parameter is a uint32_t *
 * where the number is stored
 */
//---------------------------------------------------------------------------
#define BEFS_IOCTL_VERSION			14200
//---------------------------------------------------------------------------
/* ioctls to use the "chkbfs" feature from the outside
 * all calls use a struct check_result as single parameter
 */
//---------------------------------------------------------------------------
#define	BEFS_IOCTL_START_CHECKING	14201
#define BEFS_IOCTL_STOP_CHECKING		14202
#define BEFS_IOCTL_CHECK_NEXT_NODE	14203

//---------------------------------------------------------------------------
/* all fields except "flags", and "name" must be set to zero before
 * BEFS_IOCTL_START_CHECKING is called
 */
//---------------------------------------------------------------------------
struct check_control {
    uint32_t		magic;
    uint32_t		flags;
    char		name[B_FILE_NAME_LENGTH];
    vnode_id	inode;
    uint32_t		mode;
    uint32_t		errors;
    struct {
        int64_t	missing;
        int64_t	already_set;
        int64_t	freed;
    } stats;
    status_t	status;
    void		*cookie;
};

//---------------------------------------------------------------------------
/* values for the flags field */
/* files that shouldn't be part of its parent will be removed
 * (i.e. a directory contains an attribute, ...)
 * Works only if B_FIX_BITMAP_ERRORS is set, too
 */
//---------------------------------------------------------------------------
#define BEFS_FIX_BITMAP_ERRORS	1
#define BEFS_REMOVE_WRONG_TYPES	2
//---------------------------------------------------------------------------
/* removes nodes that couldn't be opened at all from its parent
 * directory.
 * Also requires the B_FIX_BITMAP_ERRORS to be set.
 */
//---------------------------------------------------------------------------
#define BEFS_REMOVE_INVALID		4
//---------------------------------------------------------------------------
/* values for the errors field */
//---------------------------------------------------------------------------
#define BEFS_MISSING_BLOCKS		1
#define BEFS_BLOCKS_ALREADY_SET	2
#define BEFS_INVALID_BLOCK_RUN	4
#define	BEFS_COULD_NOT_OPEN		8
#define BEFS_WRONG_TYPE			16
#define BEFS_NAMES_DONT_MATCH	32

//---------------------------------------------------------------------------
/* check control magic value */
//---------------------------------------------------------------------------
#define BEFS_IOCTL_CHECK_MAGIC	'BChk'

#endif	/* BEFS_CONTROL_H */
