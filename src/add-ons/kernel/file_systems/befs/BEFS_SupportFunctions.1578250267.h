/* Author: Peter Speybrouck - peter.speybrouck@gmail.com */

//---------------------------------------------------------------------------
// WinAPI commctrl32
struct _TREEITEM;
typedef struct _TREEITEM *HTREEITEM;
#ifdef __GNUC__
typedef struct HWND__ *HWND;
#endif
//---------------------------------------------------------------------------
//tree traversal functions
HTREEITEM GetNext(HWND hTree, HTREEITEM Item);
HTREEITEM GetNextUpItem(HWND hTree, HTREEITEM Item);
//---------------------------------------------------------------------------
