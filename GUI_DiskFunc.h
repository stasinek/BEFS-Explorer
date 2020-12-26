/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 */

//-----------------------------------------------------------------------------
#ifndef GUI_DISKFUNC_H
#define GUI_DISKFUNC_H
//-----------------------------------------------------------------------------
// BEFS include files
#include <src/add-ons/kernel/file_systems/befs/BEFS_Volume.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_Inode.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_PlusTree.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_Interface.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_SupportFunctions.h>
#include <BEOS_SystemWrapper.h>
//-----------------------------------------------------------------------------
// WINDOWS CRT include files
#define _CRT_SECURE_NO_DEPRECATE 1
#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <vector>
//-----------------------------------------------------------------------------
#include <resource/GUI_resource.h>
#define ICON_TREE	1
#define ICON_FILE	2
#define ICON_OTHER	3
#define ICON_DISK	4
#define PHYSICALDRIVE "\\\\.\\PhysicalDrive%i"
//-----------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------

__int32 __stdcall CountPhysicalDrives();
__int32 __stdcall ListPartitions(__int32 disk, HWND h, HTREEITEM* node, TVINSERTSTRUCTA* s, std::ofstream* a_debug);
__int32 __stdcall ListDirectories(HWND h, HTREEITEM* node, TVITEMA* s, ofstream* d, vector<Volume>* a_volumes);

//-----------------------------------------------------------------------------

class TreeData {
public:
        TreeData() {}
        TreeData(__int32 l, const char* n)
            :level(l)
        {
            strcpy(name,n);
        }

        TreeData(__int32 l, const char* n, int64_t i)
            :level(l), inode(i)
        {
            strcpy(name,n);
        }

        ~TreeData()
        {
            if (extra) free(extra);
        }
        __int32 volume;
        char    name[255];
        __int32 level;
        __int32 iData1;
        __int32 iData2;
        int64_t inode;
        void*   extra;
};
//-----------------------------------------------------------------------------
__int32 __stdcall AddChild(HWND hWnd, HTREEITEM* parent, char* t1, __int32 icon, TreeData* td, std::ofstream* debug);
//-----------------------------------------------------------------------------
#endif
