/*
 * Copyright 2007, Ingo Weinhold, bonefish@cs.tu-berlin.de.
 * Distributed under the terms of the MIT License.
 */

#ifndef BEOS_SYSTEM_DEPENDENCIES_H
#define BEOS_SYSTEM_DEPENDENCIES_H

#define _CRT_SECURE_NO_DEPRECATE 1
//#define DEBUG 1

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _BOOT_MODE
#include <BEOS_Defs.h>
//#include <fs_attr.h>
//#include <fs_cache.h>
//#include <fs_index.h>
//#include <fs_info.h>
//#include <fs_interface.h>
//#include <fs_query.h>
//#include <fs_volume.h>
//#include <Drivers.h>
//#include <KernelExport.h>
//#include <NodeMonitor.h>
//#include <TypeConstants.h>
#endif	// _BOOT_MODE

#include <src/add-ons/kernel/file_systems/befs/BEFS_DoublyLinkedList.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_Stack.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_Cache.h>

#endif	// _SYSTEM_DEPENDENCIES_H
