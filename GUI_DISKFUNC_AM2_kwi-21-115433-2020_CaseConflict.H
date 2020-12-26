/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 */

#ifndef GUI_DISKFUNC_H
#define GUI_DISKFUNC_H

// BEFS include files
#include <src/add-ons/kernel/file_systems/befs/BEFS_Volume.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_Inode.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_PlusTree.h>
#include <src/add-ons/kernel/file_systems/befs/BEFS_Interface.h>
#include <resource/GUI_resource.h>
#include <BEOS_SystemWrapper.h>

// WINDOWS CRT include files
#define _CRT_SECURE_NO_DEPRECATE 1
#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define ICON_TREE	1
#define ICON_FILE	2
#define ICON_OTHER	3
#define ICON_DISK	4

int __stdcall CountPhysicalDrives();
int __stdcall ListPartitions(int disk, HWND h, HTREEITEM* node, TVINSERTSTRUCTA* s, std::ofstream* a_debug);
int __stdcall ListDirectories(HWND h, HTREEITEM* node, TVITEMA* s, ofstream* d, vector<Volume>* a_volumes);

class TreeData {
    public:
        TreeData() {}
        TreeData(int l, const char* n)
            :level(l)
        {
            sprintf(name, n);
        }

        TreeData(int l, const char* n, int64_t i)
            :level(l), inode(i)
        {
            sprintf(name,n);
        }

        ~TreeData()
        {
            if (name)
                free(name);

            if (extra)
                free(extra);
        }

        int level;
        int iData1;
        int iData2;
        int volume;
        int64_t inode;
        char name[255];
        void* extra;
};

int __stdcall AddChild(HWND hWnd, HTREEITEM* parent, char* t1, int icon, TreeData* td, std::ofstream* debug);

#endif
