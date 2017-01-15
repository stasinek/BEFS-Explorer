/* Author: Peter Speybrouck - peter.speybrouck@gmail.com */


#include <windows.h>
#include <commctrl.h>

//tree traversal functions
HTREEITEM GetNext(HWND hTree, HTREEITEM Item);
HTREEITEM GetNextUpItem(HWND hTree, HTREEITEM Item);
