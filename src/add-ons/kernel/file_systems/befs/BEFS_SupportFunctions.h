/* Author: Peter Speybrouck - peter.speybrouck@gmail.com */

//---------------------------------------------------------------------------
// WinAPI commctrl32
struct _TREEITEM;
typedef struct _TREEITEM *HTREEITEM;
typedef struct HWND__ *HWND;
//---------------------------------------------------------------------------
//tree traversal functions
HTREEITEM GetNext(HWND hTree, HTREEITEM Item);
HTREEITEM GetNextUpItem(HWND hTree, HTREEITEM Item);
//---------------------------------------------------------------------------
