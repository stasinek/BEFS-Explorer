/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
*  ----------------------------------------------------------------------------
*  "THE BEER-WARE LICENSE" (Revision 42):
*  <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
*  can do whatever you want with this stuff. If we meet some day, and you think
*  this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
*  ----------------------------------------------------------------------------
*/

//-----------------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE 1
#define DEBUG 1
//-----------------------------------------------------------------------------
#include <GUI_DiskFunc.h>
#include <resource/GUI_resource.h>
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
//-----------------------------------------------------------------------------
#include <WINNT_Defs.h>
//-----------------------------------------------------------------------------

// visual studio 2003
// to fix LNK2005, linker options -> ignore libraries: LIBCD.lib ntdll.lib
// then additional dependancies: comctl32.lib LIBCD.lib ntdll.lib

// visual studio 2005
// here this problem lib seems to be LIBCMTD.lib instead of LIBCD.lib

using namespace std;

//-----------------------------------------------------------------------------
// Handles
//-----------------------------------------------------------------------------
HINSTANCE hInst; // main function handler
//-----------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN // this will assume smaller exe
#define ID_copy 100
#define ID_props 200
#define ID_about 300
//-----------------------------------------------------------------------------
TVITEMA tvi;
HTREEITEM Selected;
TVINSERTSTRUCTA tvinsert;   // struct to config out tree control
HTREEITEM Parent;           // Tree item handle
HTREEITEM Before;           // .......
HTREEITEM Root;             // .......
HIMAGELIST hImageList;      // Image list array hadle
HBITMAP hBitMap;            // bitmap handler
bool flagSelected=false;
//-----------------------------------------------------------------------------
// for drag and drop
//-----------------------------------------------------------------------------
HWND hTree;
TVHITTESTINFO tvht;
HTREEITEM hitTarget;
POINTS Pos;
bool Dragging;
//-----------------------------------------------------------------------------
// for lable editing
//-----------------------------------------------------------------------------
HWND hEdit,hProgress;
HWND hList;
//-----------------------------------------------------------------------------
// for debug output
//-----------------------------------------------------------------------------
extern ofstream debug;
ofstream debug;
//-----------------------------------------------------------------------------
// volume mounting stuff
//-----------------------------------------------------------------------------
int mountID=1;
vector<Volume> volumes;
void* iter;
void* it;
dirent_t d,dd;
uint32_t num,num2;
HMENU popupMenu;
//-----------------------------------------------------------------------------
//function prototypes:
//-----------------------------------------------------------------------------
int addChild(HWND hWnd,HTREEITEM* parent, char* t1,int icon,TreeData* td);
void AddAttrInfo(char* pszAttrName, char* pszAttrData, char* type, int iAccess);
void SetupListView(void);
BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------------------

VOID InitPopupMenu(HWND hWnd)
{
    LPSTR str[3] = {"Copy to local folder", "Properties...", "About..."};
    int ID[3] = {ID_copy, ID_props, ID_about};

    MENUITEMINFOA mi;

    popupMenu = CreatePopupMenu();

    for (int i = 0; i < 3; i++) {
        mi.cbSize = sizeof(MENUITEMINFOA);
        mi.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
        //mi.hbmpItem=LoadBitmap((HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), str[i]);
        mi.fType = MFT_RIGHTORDER;
        mi.cch = strlen(str[i]);
        mi.wID = ID[i];
        mi.dwTypeData = str[i];

        InsertMenuItemA(popupMenu, 0, FALSE, &mi);
    }
}
//-----------------------------------------------------------------------------
//====================MAIN DIALOG==========================================//
//-----------------------------------------------------------------------------

BOOL CALLBACK PropertiesDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) // what are we doing ?
    {
        case WM_INITDIALOG:
        {
            hList = GetDlgItem(hWnd, IDC_LIST1);
            SetupListView();
            //tvi should contain data about the selected item
            TreeData* td = reinterpret_cast<TreeData*>(tvi.lParam);
            if (td) {
                debug << "init properties dialog = " << td->level << " name = " << td->name << " inode = " << td->inode << "volume = " << td->extra << endl;
            }

            //add filesize, attributes, ... to dialog
            //Volume* v=(Volume*)td->extra;
            Volume* v = (Volume*)(&volumes[td->volume]);
            Inode* I;
            //void* f; // file_cookie
            //file_cookie ff;
            bfs_read_vnode(v, td->inode, (void**)&I, true /*not used*/);
            //memcpy(&ff,f,sizeof(file_cookie));
            void* attIt;
            uint32_t num;
            dirent_t d;
            struct attr_stat AttrStat;
            attr_cookie* attCookie;
            bfs_open_attr_dir(v, I, &attIt);
            status_t st = bfs_read_attr_dir(v, I, attIt, &d,0, &num);
            debug << "attributes: \tName\t\tData\n" << st << num << endl;
            int a = 0;
            while (st == 0 && num > 0) {
                bfs_open_attr(v, I, d.d_name, O_RDONLY, &attCookie);
                bfs_read_attr_stat(v, I, attCookie, &AttrStat);
                unsigned char tupel[5];
                tupel[0] = 0xff & (AttrStat.st_type >> 24);
                tupel[1] = 0xff & (AttrStat.st_type >> 16);
                tupel[2] = 0xff & (AttrStat.st_type >> 8);
                tupel[3] = 0xff & (AttrStat.st_type);
                tupel[4] = 0;
                for (int16_t i = 0;i < 4;i++) {
                    if (tupel[i] < ' ' || tupel[i] > 128)
                        tupel[i] = '.';
                }

                int64_t leng=AttrStat.st_size;
                char*  buff = (char*)calloc(leng,sizeof(char));
                bfs_read_attr(v, I, attCookie, 0,buff, &leng);
                debug << "attribute: " << d.d_name << "  " << "length: " << leng << " type: " << tupel << "  data: ";
                debug.flush();
                ostringstream oss;
                switch(AttrStat.st_type) {
                    /*
                    B_ANY_TYPE = 'ANYT',
  B_ATOM_TYPE = 'ATOM',
  B_ATOMREF_TYPE = 'ATMR',
  B_BOOL_TYPE = 'BOOL',
  B_CHAR_TYPE = 'CHAR',
  B_COLOR_8_BIT_TYPE = 'CLRB',
  B_DOUBLE_TYPE = 'DBLE',
  B_FLOAT_TYPE = 'FLOT',
  B_GRAYSCALE_8_BIT_TYPE = 'GRYB',
  B_INT16_TYPE = 'SHRT',
  B_INT64_TYPE = 'LLNG',
  B_INT8_TYPE = 'BYTE',
  B_LARGE_ICON_TYPE = 'ICON',
  B_MEDIA_PARAMETER_GROUP_TYPE = 'BMCG',
  B_MEDIA_PARAMETER_TYPE = 'BMCT',
  B_MEDIA_PARAMETER_WEB_TYPE = 'BMCW',
  B_MESSAGE_TYPE = 'MSGG',
  B_MESSENGER_TYPE = 'MSNG',
  B_MIME_TYPE = 'MIMS',
  B_MINI_ICON_TYPE = 'MICN',
  B_MONOCHROME_1_BIT_TYPE = 'MNOB',
  B_OBJECT_TYPE = 'OPTR',
  B_OFF_T_TYPE = 'OFFT',
  B_PATTERN_TYPE = 'PATN',
  B_POINTER_TYPE = 'PNTR',
  B_POINT_TYPE = 'BPNT',
  B_PROPERTY_INFO_TYPE = 'SCTD',
  B_RAW_TYPE = 'RAWT',
  B_RECT_TYPE = 'RECT',
  B_REF_TYPE = 'RREF',
  B_RGB_32_BIT_TYPE = 'RGBB',
  B_RGB_COLOR_TYPE = 'RGBC',
  B_SIZE_T_TYPE = 'SIZT',
  B_SSIZE_T_TYPE = 'SSZT',
  B_STRING_TYPE = 'CSTR',
  B_TIME_TYPE = 'TIME',
  B_UINT16_TYPE = 'USHT',
  B_UINT32_TYPE = 'ULNG',
  B_UINT64_TYPE = 'ULLG',
  B_UINT8_TYPE = 'UBYT',
  B_VECTOR_ICON_TYPE = 'VICN',
  B_ASCII_TYPE = 'TEXT'
  S_MIME_TYPE = 'MIMS'
                    */
                    case B_UINT8_TYPE:
                        {
                            byte tmp;
                            memcpy(&tmp, buff, 1);
                            oss << tmp;
                            break;
                        }
                    case B_INT8_TYPE:
                        {
                            byte tmp;
                            memcpy(&tmp, buff, 1);
                            oss << tmp;
                            break;
                        }
                    case B_INT16_TYPE:
                        {
                            short tmp;
                            memcpy(&tmp, buff, 2);
                            oss << tmp;
                            break;
                        }
                    case B_SSIZE_T_TYPE:
                        {
                            int tmp;
                            memcpy(&tmp, buff, 4);
                            oss << tmp;
                            break;
                        }
                    case B_INT32_TYPE:
                        {
                            int tmp;
                            memcpy(&tmp, buff, 4);
                            oss << tmp;
                            break;
                        }
                    case B_OFF_T_TYPE:
                        {
                            __int64 tmp;
                            memcpy(&tmp, buff, sizeof(__int64));
                            oss << tmp;
                            break;
                        }
                    case B_INT64_TYPE:
                        {
                            __int64 tmp;
                            memcpy(&tmp, buff, sizeof(__int64));
                            oss << tmp;
                            break;
                        }
                    case B_UINT64_TYPE:
                        {
                            unsigned __int64 tmp;
                            memcpy(&tmp, buff, sizeof(unsigned __int64));
                            oss << tmp;
                            break;
                        }
                    case B_UINT16_TYPE:
                        {
                            unsigned short tmp;
                            memcpy(&tmp, buff, sizeof(unsigned short));
                            oss << tmp;
                            break;
                        }
                    case B_UINT32_TYPE:
                        {
                            unsigned int tmp;
                            memcpy(&tmp, buff, sizeof(unsigned int));
                            oss << tmp;
                            break;
                        }
                    case B_SIZE_T_TYPE:
                        {
                            unsigned int tmp;
                            memcpy(&tmp, buff, sizeof(unsigned int));
                            oss << tmp;
                            break;
                        }
                    case B_DOUBLE_TYPE:
                        {
                            double tmp;
                            memcpy(&tmp, buff, sizeof(double));
                            oss << tmp;
                            break;
                        }
                    case B_FLOAT_TYPE:
                        {
                            float tmp;
                            memcpy(&tmp, buff, sizeof(float));
                            oss << tmp;
                            break;
                        }

                    case B_ANY_TYPE:
                    case B_ATOM_TYPE:
                    case B_BOOL_TYPE:
                    case B_CHAR_TYPE:
                    case B_COLOR_8_BIT_TYPE:
                    case B_GRAYSCALE_8_BIT_TYPE:
                    case B_LARGE_ICON_TYPE:
                    case B_MEDIA_PARAMETER_GROUP_TYPE:
                    case B_MEDIA_PARAMETER_TYPE:
                    case B_MEDIA_PARAMETER_WEB_TYPE:
                    case B_MESSAGE_TYPE:
                    case B_MESSENGER_TYPE:
                    case B_MINI_ICON_TYPE:
                    case B_MONOCHROME_1_BIT_TYPE:
                    case B_OBJECT_TYPE:
                    case B_PATTERN_TYPE:
                    case B_POINTER_TYPE:
                    case B_POINT_TYPE:
                    case B_PROPERTY_INFO_TYPE:
                    case B_RAW_TYPE:
                    case B_RECT_TYPE:
                    case B_REF_TYPE:
                    case B_RGB_32_BIT_TYPE:
                    case B_RGB_COLOR_TYPE:
                    case B_TIME_TYPE:
                    case B_VECTOR_ICON_TYPE:
                    case B_ASCII_TYPE:
                    // these all work as string
                    case B_MIME_TYPE:
                    case B_STRING_TYPE:
                    default:
                        for (int64_t i = 0; i < leng; i++) {
                            //debug<<buff[i];
                            oss << buff[i];
                        }
                        break;
                }
                string str = oss.str();
                AddAttrInfo(d.d_name, (char*)str.c_str(), (char*)tupel,a++);
                st = bfs_read_attr_dir(v, I, attIt, &d, 0, &num);
                debug << str.c_str() << "\n";
                debug.flush();
            }
            //bfs_NtClose(v,I,&ff);

            ostringstream oss;
            oss << "Name: " << td->name;
            string str = oss.str();
            SetDlgItemTextA(hWnd, IDC_STATIC2, str.c_str());
            if (I->IsFile()) {
                SetDlgItemTextA(hWnd, IDC_STATIC3, "Type: File");
                ostringstream oss2;
                oss2 << "Size: " << I->Size() << " bytes";
                string str = oss2.str();
                SetDlgItemTextA(hWnd, IDC_STATIC4, str.c_str());
            } else if (I->IsDirectory())
                SetDlgItemTextA(hWnd, IDC_STATIC3, "Type: Directory");
            else if (I->IsSymLink())
                SetDlgItemTextA(hWnd, IDC_STATIC3, "Type: Symlink");
            else
                SetDlgItemTextA(hWnd, IDC_STATIC3, "Type: Unknown");

            if (!I->IsFile())
                SetDlgItemTextA(hWnd, IDC_STATIC4, "");
            //read attibutes
        }

        case WM_COMMAND: // Controling the Buttons
        {
            switch (LOWORD(wParam)) // what we pressed on?
            {
                case IDOK:
                {
                    //close the dialog
                    EndDialog(hWnd, 0);
                }
                break;
            }
            break;

            case WM_CLOSE: // We closing the Dialog
            {
                EndDialog(hWnd, 0);
            }
            break;
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) // what are we doing ?
    {
        // This Window Message is the heart of the dialog //
        //================================================//
        case WM_INITDIALOG:
        {
            debug.open ("debuglog.txt", ios::app|ios::out);
            debug << "=============================================================================================\n";
            debug << "Logging started\n";
            debug << "=============================================================================================\n";
            debug.flush();

            InitCommonControls();	    // make our tree control to work
            hTree=GetDlgItem(hWnd, IDC_TREE1);
            // creating image list and put it into the tree control
            //====================================================//
            hImageList = ImageList_Create(16, 16, ILC_COLOR16, 2, 10);					  // Macro: 16x16:16bit with 2 pics [array]
            hBitMap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TREE));					  // load the picture from the resource
            ImageList_Add(hImageList, hBitMap, NULL);								      // Macro: Attach the image, to the image list
            DeleteObject(hBitMap);													  // no need it after loading the bitmap
            SendDlgItemMessage(hWnd, IDC_TREE1, TVM_SETIMAGELIST, 0, (LPARAM)hImageList); // put it onto the tree control

            InitPopupMenu(hWnd);
            hProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 3, 470, 527, 25, hWnd, NULL, hInst, NULL);
            SendMessage(hProgress, PBM_SETSTEP, 4096, 0L);

            string strItem;
            int i, cd = CountPhysicalDrives();
            debug << "Number of drives: " << cd << endl;
            for (i = 0;i < cd; i++) {
                tvinsert.hParent=NULL;			// top most level no need handle
                tvinsert.hInsertAfter = TVI_ROOT; // work as root level
                tvinsert.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
                ostringstream oss;
                oss << "Device" << i;
                strItem = oss.str();//"Device"+i;

                TreeData* t = new TreeData(0,strItem.c_str());
                t->iData1 = i; // save disk number
                tvinsert.item.lParam = reinterpret_cast<LPARAM>(t); //TreeData pointer

                tvinsert.item.pszText = (LPSTR)strItem.c_str();
                tvinsert.item.iImage = ICON_DISK;
                tvinsert.item.iSelectedImage = ICON_DISK;

                Parent = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
                Root = Parent;
                ListPartitions(i, hWnd, &Parent, &tvinsert, &debug);
            }

            // now, for each bfs/bfs partition on all drives, list root dir
            char Text[255] = "";
            HTREEITEM CurrentItem = TreeView_GetRoot(hTree);
            tvi.mask = TVIF_TEXT|TVIF_PARAM|TVIF_CHILDREN;
            tvi.pszText = Text;
            tvi.cchTextMax = 256;
            while (CurrentItem) {
                int children = 0;
                tvi.hItem = CurrentItem;
                TreeView_GetItem(hTree, &tvi);
                debug<<"+++++++++  item: " << tvi.pszText << endl; debug.flush();
                TreeData* td = reinterpret_cast<TreeData*>(tvi.lParam);
                if (td) {
                    debug<<"Node Level = " << td->level << " name = " << td->name << " iData1 = " << td->iData1 << endl;
                    debug.flush();
                    if (td->level == 1) {
                        //mount partition
                        Volume* v = new Volume(mountID);
                        debug<<"new volume: " << v << endl;
                        status_t s = v->Mount(td->iData1, td->iData2, 0 /*readonly*/);
                        if (s == B_OK) {
                            debug << "Mount successfull\n";
                            // put volume data in partition tree node
                            volumes.push_back(*v);
                            td->extra = (Volume*)&volumes[volumes.size()-1];
                            td->volume = volumes.size()-1;
                            td->inode = -1;
                            tvi.lParam = reinterpret_cast<LPARAM>(td);
                            TreeView_SetItem(hWnd,&tvi);
                            //load dir root dir contents of this partition
                            debug << "CurrentItem = " << CurrentItem << "  currentvolume = " << td->extra << "  hTree = " << hTree << endl;
                            children = ListDirectories(hTree, &CurrentItem, &tvi, &debug, &volumes);
                        } else {
                            debug << "Mounting failed, status: " << s << endl;
                        }
                    }
                }
                //this is to prevent from iterating all children that were just added,
                // but will it continue with the next partition or the next device?
                // other option: when level 2 detected, use GetNextUpItem
                if (td->level == 2)
                    CurrentItem = GetNextUpItem(hTree, CurrentItem);
                else
                    CurrentItem = GetNext(hTree, CurrentItem);
            }

            for (i = 0;i < volumes.size(); i++) {
                Volume* vv = (Volume*)&volumes[i];

                if (vv->IsValidSuperBlock())
                    debug << "1: VALID SUPERBLOCK " << vv << endl;
                else
                    debug << "1: NO VALID SUPERBLOCK!!! " << vv << endl;

                debug.flush();
            }
        }
        break;

        case WM_LBUTTONDOWN:
        {
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        }
        break;

        case WM_NOTIFY:
        {
            case IDC_TREE1:

            if (((LPNMHDR)lParam)->code == TVN_SELCHANGING)
            {
                NMTREEVIEW*  pnmtv = (LPNMTREEVIEW) lParam;
                TreeData* td1 = reinterpret_cast<TreeData*>(pnmtv->itemOld.lParam);
                TreeData* td2 = reinterpret_cast<TreeData*>(pnmtv->itemNew.lParam);
                debug<<"TVN_SELCHANGING, old: "<<pnmtv->itemOld.hItem<<endl;
                debug<<"TVN_SELCHANGING, new: "<<pnmtv->itemNew.hItem<<endl;
                char Text[255] = "";
                memset(&tvi, 0, sizeof(tvi));
                tvi.mask = TVIF_TEXT|TVIF_PARAM|TVIF_CHILDREN;
                tvi.pszText = Text;
                tvi.cchTextMax = 256;
                tvi.hItem = pnmtv->itemNew.hItem;
                TreeView_GetItem(hTree, &tvi);
                TreeData* td = reinterpret_cast<TreeData*>(tvi.lParam);
                debug << "TVN_SELCHANGING, name: " << Text << "  " << td->inode << "  " << td->volume << endl;
                if (td->level > 1) {
                    if (tvi.cChildren == 0) {
                        ListDirectories(hTree, &tvi.hItem, &tvi, &debug, &volumes);
                    }
                }
                break;
            }

            if (((LPNMHDR)lParam)->code == NM_CLICK) { // if code == NM_CLICK - Single click on an item
                //NMTREEVIEW*  pnmtv = (LPNMTREEVIEW) lParam;
                for (unsigned int i = 0;i < volumes.size(); i++) {
                    Volume vv = volumes[i];
                    if (vv.IsValidSuperBlock())
                        debug<<"3: VALID SUPERBLOCK "<<(Volume*)&volumes[i]<<endl;
                    else
                        debug<<"3: NO VALID SUPERBLOCK!!! "<<(Volume*)&volumes[i]<<endl;
                    debug.flush();
                }
                char Text[255] = "";
                memset(&tvi, 0, sizeof(tvi));
                //tvi=pnmtv->itemNew;
                Selected = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_TREE1, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)Selected);

                if (Selected == NULL) {
                    MessageBoxA(hWnd, "No Items in TreeView", "Error", MB_OK|MB_ICONINFORMATION);
                    break;
                }
                TreeView_EnsureVisible(hWnd, Selected);
                SendDlgItemMessage(hWnd, IDC_TREE1, TVM_SELECTITEM, TVGN_CARET, (LPARAM)Selected);
                flagSelected = true;
                tvi.mask = TVIF_TEXT|TVIF_PARAM|TVIF_CHILDREN;
                tvi.pszText = Text;
                tvi.cchTextMax = 256;
                tvi.hItem = Selected;

                debug << "If the program get's here wee need to look at this code.." << endl;
                /*Was coment out
                if (SendDlgItemMessage(hWnd,IDC_TREE1,TVM_GETITEM,TVGN_CARET,(LPARAM)&tvi))
                {
                    TreeData* td = reinterpret_cast<TreeData*>(tvi.lParam);
                    if (td) {
                        debug<<"Node Level="<<td->level<<" name="<<td->name<<" iData1="<<td->iData1<<endl;
                        debug.flush();
                        if (td->level==1) {
                            //mount device if not already mounted
                            //add root dir if there are not children in this node
                            if (tvi.cChildren== 0) {
                                debug<<"Partition node has no children, mount partition" << endl;

                                Volume* v=new Volume(mountID);
                                status_t s = v->Mount(td->iData1,td->iData2, 0 );
                                if (s == B_OK) {
                                    debug<<"Mount successfull\n";
                                    // put volume data in partition tree node
                                    volumes.push_back(*v);
                                    td->extra=(Volume*)&volumes[volumes.size()-1];
                                    tvi.lParam=reinterpret_cast<LPARAM>(td);
                                    TreeView_SetItem(hWnd,&tvi);
                                    // open rootdir:
                                    bfs_open_dir(v, v->RootNode(), &iter);
                                    // second arg of bfs_read_dir is not used actually
                                    status_t ss = bfs_read_dir(v, v->RootNode(), iter, &d,sizeof(dirent), &num);
                                    //TODO: this should be the '.' entry...
                                    if (ss==B_OK && num>0) debug<<d.d_name<<endl;
                                    while (ss==B_OK && num>0) {
                                        debug<<"before readdir\n";debug.flush();
                                        ss = bfs_read_dir(v, v->RootNode(), iter, &d,sizeof(dirent), &num);
                                        if (ss==B_OK && num>0) {
                                            //debug<<"readdir success\n";debug.flush();
                                            //printf("dirent: %s, len=%i, ino=%i\n",d.d_name,d.d_reclen,d.d_ino);
                                            Inode* I = new Inode(v,d.d_ino);
                                            if (I->IsDirectory()) {
                                                debug<<d.d_name<<"(dir)" << endl;debug.flush();
                                                TreeData* data = new TreeData(2,d.d_name,d.d_ino);
                                                data->extra=v;
                                                addChild(hWnd,&Selected, d.d_name,ICON_TREE,data,&debug);
                                            }
                                            if (I->IsFile()) {
                                                debug<<d.d_name<<"(file)" << endl;debug.flush();
                                                TreeData* data = new TreeData(2,d.d_name,d.d_ino);
                                                data->extra=v;
                                                addChild(hWnd,&Selected, d.d_name,ICON_FILE,data,&debug);
                                            }
                                            if (!I->IsFile() && !I->IsDirectory()) {
                                                debug<<d.d_name<<"(?)" << endl;debug.flush();
                                                TreeData* data = new TreeData(2,d.d_name,d.d_ino);
                                                data->extra=v;
                                                addChild(hWnd,&Selected, d.d_name,ICON_OTHER,data,&debug);
                                            }
                                            free(I);
                                        }
                                        //debug<<"at end while loop\n"; debug.flush();
                                    }
                                    //TreeView_Expand(hWnd,Selected,TVE_EXPAND);
                                    //TreeView_Select(hWnd,Selected,TVGN_CARET);
                                    //TreeView_SelectDropTarget(GetDlgItem(hWnd,IDC_TREE1),Selected);
                                }
                                else{
                                    MessageBox(hWnd,"Unable to mount this partition.","Mount error",MB_OK|MB_ICONINFORMATION);
                                }
                                debug.flush();
                            }
                            else{
                                debug<<"Partition node has children, partition mounted earlier" << endl;
                                debug.flush();
                            }
                        }
                        else if (td->level>1) {
                            // we're somewhere in the partition,
                            // if dir, list contents, else do nothing
                            // if has children, do nothing
                            debug << "somewhere in the partition, volume = " << td->extra << " hWnd = " << hWnd << endl;
                            Volume* vol = (Volume*)td->extra;
                            if (tvi.cChildren == 0) {
                                hTree=GetDlgItem(hWnd,IDC_TREE1);
                                listDir(hWnd, &Selected, &tvi, &debug,&volumes);
                                Inode* II = new Inode(vol,td->inode);
                                debug<<"\tInode size: "<<II->Size()<<endl;debug.flush();

                                if (II->IsDirectory())
                                    listDir(hTree, &Selected, &tvi, &debug,&volumes);

                                debug<<"checking node type, inode="<<td->inode<<"  volume="<<vol<<endl;debug.flush();


                                debug<<"\tInode size: "<<II->Size()<<endl;debug.flush();
                                if (II->IsDirectory()) {
                                    debug<<"Level "<<td->level<<" directory, listing contents\n";debug.flush();
                                    bfs_open_dir(vol, II, &iter);
                                    status_t s1 = bfs_read_dir(vol, II, iter, &d,sizeof(dirent), &num2);
                                    Inode* ii = new Inode(vol,d.d_ino);
                                    if (s1==B_OK && num2>0 && ii->IsDirectory()) {
                                        //printf("\t%s (directory,%I64d)\n",dd.d_name,dd.d_ino);
                                        debug<<d.d_name<<"(dir)" << endl;debug.flush();
                                        TreeData* data = new TreeData(td->level+1,d.d_name,d.d_ino);
                                        data->extra=vol;
                                        addChild(hWnd,&Selected, d.d_name,ICON_TREE,data,&debug);
                                    }
                                    if (s1==B_OK && num2>0 && ii->IsFile()) {
                                        //printf("\t%s (file,%I64d)\n",dd.d_name,dd.d_ino);
                                        debug<<d.d_name<<"(file)" << endl;debug.flush();
                                        TreeData* data = new TreeData(td->level+1,d.d_name,d.d_ino);
                                        data->extra=vol;
                                        addChild(hWnd,&Selected, d.d_name,ICON_OTHER,data,&debug);
                                    }
                                    if (s1==B_OK && num2>0 && !ii->IsDirectory() && !ii->IsFile()) {
                                        //printf("\t%s (other,%I64d)\n",dd.d_name,dd.d_ino);
                                        debug<<d.d_name<<"(?)" << endl;debug.flush();
                                        TreeData* data = new TreeData(td->level+1,d.d_name,d.d_ino);
                                        data->extra=vol;
                                        addChild(hWnd,&Selected, d.d_name,ICON_OTHER,data,&debug);
                                    }
                                    free(ii);
                                    while (s1==B_OK && num2>0) {
                                        s1 = bfs_read_dir(vol, II, iter, &d,sizeof(dirent), &num2);
                                        ii = new Inode(vol,d.d_ino);
                                        if (s1==B_OK && num2>0 && ii->IsDirectory()) {
                                            //printf("\t%s (directory,%I64d)\n",dd.d_name,dd.d_ino);
                                            debug<<d.d_name<<"(dir)" << endl;debug.flush();
                                            TreeData* data = new TreeData(td->level+1,d.d_name,d.d_ino);
                                            data->extra=vol;
                                            addChild(hWnd,&Selected, d.d_name,ICON_TREE,data,&debug);
                                        }
                                        if (s1==B_OK && num2>0 && ii->IsFile()) {
                                            //printf("\t%s (file,%I64d)\n",dd.d_name,dd.d_ino);
                                            debug<<d.d_name<<"(file)" << endl;debug.flush();
                                            TreeData* data = new TreeData(td->level+1,d.d_name,d.d_ino);
                                            data->extra=vol;
                                            addChild(hWnd,&Selected, d.d_name,ICON_FILE,data,&debug);
                                        }
                                        if (s1==B_OK && num2>0 && !ii->IsDirectory() && !ii->IsFile()) {
                                            //printf("\t%s (other,%I64d)\n",dd.d_name,dd.d_ino);
                                            debug<<d.d_name<<"(?)" << endl;debug.flush();
                                            TreeData* data = new TreeData(2,d.d_name,d.d_ino);
                                            data->extra=vol;
                                            addChild(hWnd,&Selected, d.d_name,ICON_FILE,data,&debug);
                                        }
                                        free(ii);
                                    }
                                    bfs_close_dir(vol, II, &iter);
                                }
                                free(II);
                            }
                        }
                    }
                }*///Was coment out end..
                break;
            }
            if (((LPNMHDR)lParam)->code == TVN_GETINFOTIP) // if code == NM_CLICK - Single click on an item
            {
                debug << "If the program get's here wee need to look at this code.. nr 2" << endl;
                /*debug<<"infotip\n";
                debug.flush();
                LPNMTVGETINFOTIP pTip = (LPNMTVGETINFOTIP)lParam;
                HWND hTree = GetDlgItem(hWnd, IDC_TREE1);
                HTREEITEM item = pTip->hItem;

                // Get the text for the item.
                TVITEM tvitem;
                tvitem.mask = TVIF_TEXT;
                tvitem.hItem = item;
                TCHAR temp[1024];
                tvitem.pszText = "bla";
                tvitem.cchTextMax = sizeof(temp) / sizeof(TCHAR);
                TreeView_GetItem(hTree, &tvitem);

                // Copy the text to the infotip.
                wcscpy_s(pTip->pszText, pTip->cchTextMax, tvitem.pszText);
                */
                break;
            }

            if (((LPNMHDR)lParam)->code == NM_RCLICK) { // Right Click
                Selected = (HTREEITEM)SendDlgItemMessage (hWnd, IDC_TREE1, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
                if (Selected == NULL) {
                    // item may already be selected
                    Selected = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_TREE1, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)Selected);
                    if (Selected == NULL) {
                        MessageBoxA(hWnd, "No Items in TreeView", "Error", MB_OK|MB_ICONINFORMATION);
                        break;
                    }
                }

                SendDlgItemMessage(hWnd, IDC_TREE1, TVM_SELECTITEM, TVGN_CARET, (LPARAM)Selected);

                POINT p;
                p.x = LOWORD(lParam);
                p.y = HIWORD(lParam);
                //ClientToScreen(hWnd, &p);
                GetCursorPos(&p);
                TrackPopupMenu(popupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON , p.x, p.y, 0, hWnd, NULL);

                break;
            }

            if (((LPNMHDR)lParam)->code == TVN_BEGINDRAG) {
                HIMAGELIST hImg;
                LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;
                hImg=TreeView_CreateDragImage(hTree, lpnmtv->itemNew.hItem);
                ImageList_BeginDrag(hImg, 0, 0, 0);
                ImageList_DragEnter(hTree,lpnmtv->ptDrag.x,lpnmtv->ptDrag.y);
                ShowCursor(FALSE);
                SetCapture(hWnd);
                Dragging = TRUE;
            }

            if (((LPNMHDR)lParam)->code == TVN_BEGINLABELEDIT) {
                hEdit = TreeView_GetEditControl(hTree);
            }

            if (((LPNMHDR)lParam)->code == TVN_ENDLABELEDIT) {
                char Text[256] = "";
                tvi.hItem = Selected;
                SendDlgItemMessage(hWnd, IDC_TREE1, TVM_GETITEM, 0, (WPARAM)&tvi);
                GetWindowTextA(hEdit, Text, sizeof(Text));
                tvi.pszText = Text;
                SendDlgItemMessage(hWnd, IDC_TREE1, TVM_SETITEM, 0, (WPARAM)&tvi);
            }
        }
        break;

        case WM_MOUSEMOVE:
        {
            if (Dragging) {
                Pos = MAKEPOINTS(lParam);
                ImageList_DragMove(Pos.x-32, Pos.y-25); // where to draw the drag from
                ImageList_DragShowNolock(FALSE);
                tvht.pt.x = Pos.x - 20; // the highlight items should be as the same points as the drag
                tvht.pt.y = Pos.y - 20;

                if ((hitTarget = (HTREEITEM)SendMessageA(hTree, TVM_HITTEST, WPARAM(""), (LPARAM)&tvht))!=0) // if there is a hit
                    SendMessageA(hTree, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hitTarget);   // highlight it

                ImageList_DragShowNolock(TRUE);
            }
        }
        break;

        case WM_LBUTTONUP:
        {
            if (Dragging) {
                ImageList_DragLeave(hTree);
                ImageList_EndDrag();
                Selected = (HTREEITEM)SendDlgItemMessage(hWnd, IDC_TREE1, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
                SendDlgItemMessage(hWnd, IDC_TREE1, TVM_SELECTITEM, TVGN_CARET, (LPARAM)Selected);
                SendDlgItemMessage(hWnd, IDC_TREE1, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
                ReleaseCapture();
                ShowCursor(TRUE);
                Dragging = FALSE;
            }
        }
        break;


        case WM_PAINT: // constantly painting the window
        {
            return 0;
        }
        break;

        case WM_COMMAND: // Controling the Buttons
        {
            switch (LOWORD(wParam)) // what we pressed on?
            {
                case ID_props: {
                    //TODO: only if level is high enough (so not disk or partitions)
                    debug << "popupmenu -> properties" << endl;
                    char Text[255] = "";
                    memset(&tvi, 0, sizeof(tvi));
                    flagSelected = true;
                    tvi.mask = TVIF_TEXT|TVIF_PARAM|TVIF_CHILDREN;
                    tvi.pszText = Text;
                    tvi.cchTextMax = 256;
                    tvi.hItem = Selected;

                    if (SendDlgItemMessage(hWnd, IDC_TREE1, TVM_GETITEM, TVGN_CARET, (LPARAM)&tvi)) {
                        TreeData* td = reinterpret_cast<TreeData*>(tvi.lParam);
                        if (td) {
                            debug << "Node Level = " << td->level << " name = " << td->name << " inode = " << td->inode << endl;
                            //show properties dialog window
                            //DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG2), NULL, (DLGPROC)DialogProc,0);
                            if (td->level>1) // only show dialog for real files and dirs, not for devices/partitions
                                ::DialogBoxA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_DIALOG2), hWnd, (DLGPROC)PropertiesDlgProc);

                        }
                    }
                } break;

                case ID_copy:{
                    debug << "popupmenu -> copy file" << endl;
                    char Text[255] = "";
                    memset(&tvi, 0, sizeof(tvi));
                    flagSelected = true;
                    tvi.mask = TVIF_TEXT|TVIF_PARAM|TVIF_CHILDREN;
                    tvi.pszText = Text;
                    tvi.cchTextMax = 256;
                    tvi.hItem = Selected;

                    if (SendDlgItemMessage(hWnd, IDC_TREE1, TVM_GETITEM, TVGN_CARET, (LPARAM)&tvi)) {
                        TreeData* td = reinterpret_cast<TreeData*>(tvi.lParam);
                        if (td) {
                            debug << "Node Level = " << td->level << " name = " << td->name << " inode = " << td->inode << endl;
                            debug.flush();
                            if (td->level>1) {
                                //Volume* vol=(Volume*)td->extra;
                                Volume* vol = (Volume*)&volumes[td->volume];
                                Inode* II = new Inode(vol,td->inode);
                                debug << "\tInode size: " << II->Size() << endl;
                                if (II->IsFile()) {
                                    debug << "This is a file, go get it\n";
                                    debug.flush();
                                    Inode* I;
                                    void* f; // file_cookie
                                    file_cookie ff;
                                    //printf("bfs_read_vnode result: %i\n",bfs_read_vnode(v,135901,(void**)&I,true /*not used*/));
                                    debug << "bfs_read_vnode result: " << bfs_read_vnode(vol, td->inode, (void**)&I, true /*not used*/) << endl;
                                    debug << "open status: " << bfs_open(vol, I, O_RDONLY, &f) << endl;
                                    memcpy(&ff, f, sizeof(file_cookie));
                                    debug << "filesize: " << ff.last_size << endl;
                                    int64_t BytesTotal = ff.last_size;
                                    int64_t BytesLeft = ff.last_size;
                                    int64_t block = 4096;  // we will be reading in 4096 blocks
                                    int64_t offset = 0;
                                    int64_t BytesWritten = 0;
                                    SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, (long)BytesTotal/4096));
                                    //debug<<"PROGRESS: set range 0-"<<BytesTotal<<endl;
                                    SendMessage(hProgress, PBM_SETPOS, 0L, 0L);

                                    if (BytesLeft<block)
                                        block = BytesLeft; // make block smaller if file fits in it

                                    char* q = (char* )calloc(block, sizeof(char));
                                    // open file for writing
                                    ofstream file;
                                    int64_t step = 0;
                                    file.open(td->name, ios::trunc|ios::out|ios::binary);
                                    while (BytesLeft>0) {
                                        // read block
                                        //debug<<"at the start of while\n"; debug.flush();
                                        //debug<<"reading "<<block<<" bytes at offset "<<offset<<", result: "<<
                                        bfs_read(vol, I, f, offset, q, &block);
                                        //<<endl;
                                        if (file.is_open()) {
                                            //debug<<"file is open\n";
                                            /* ok, proceed with output*/
                                            file.write(q, block);
                                            BytesWritten += block;
                                            BytesLeft = BytesLeft - block;
                                            SendMessage(hProgress, PBM_SETPOS, /*BytesWritten*/++step, 0L);
                                            //debug<<"PROGRESS: setpos: "<<step<<" "<<BytesWritten<<" ("<<BytesLeft<<" bytes left)" << endl;
                                            //debug<<block<<" bytes written, bytes left: "<<BytesLeft<<endl;
                                        }

                                        offset += block;

                                        if (BytesLeft<block)
                                            block = BytesLeft;

                                        //after last block, BytesLeft should become 0 and loop will stop
                                        //debug<<"at the end of while\n"; debug.flush();
                                    }

                                    debug << "file copy done\n";
                                    file.close();
                                    bfs_close(vol, I, &ff);
                                    free(I);
                                    ostringstream oss;
                                    oss << "File copied to local directory: " << td->name << " (" << BytesWritten << "/" << BytesTotal << " bytes)";
                                    string str = oss.str();
                                    MessageBoxA(hWnd, str.c_str(), "File copied", MB_OK|MB_ICONINFORMATION);
                                    SendMessage(hProgress, PBM_SETPOS, 0L, 0L);
                                    free (q);
                                }
                                free(II);
                            }
                        }
                    }
                } break;

                case ID_about: {
                    ::DialogBoxA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDD_DIALOG3), hWnd, (DLGPROC)AboutDlgProc);
                } break;
                    /*
                case IDC_DELETE: // Generage Button is pressed
                {

                    if (flagSelected==true)
                    {
                     if (tvi.cChildren== 0)
                          SendDlgItemMessage(hWnd,IDC_TREE1,TVM_DELETEITEM,TVGN_CARET,(LPARAM)tvi.hItem);
                      flagSelected=false;
                    }
                    else{
                        MessageBox(hWnd,"Double Click Item to Delete","Message",MB_OK|MB_ICONINFORMATION);
                    }
                }
                break;

                case IDC_ADDROOT:
                {
                    tvinsert.hParent=NULL;			// top most level no need handle
                    tvinsert.hInsertAfter=TVI_ROOT; // work as root level
                    tvinsert.item.pszText="Parent Added";
                    Parent=(HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
                    UpdateWindow(hWnd);
                }
                break;

                case IDC_CHILD:
                {
                    tvinsert.hParent=Selected;			// top most level no need handle
                    tvinsert.hInsertAfter=TVI_LAST; // work as root level
                    tvinsert.item.pszText="Child Added";
                    Parent=(HTREEITEM)SendDlgItemMessage(hWnd,IDC_TREE1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
                    TreeView_SelectDropTarget(GetDlgItem(hWnd,IDC_TREE1),Parent);
                }
                break;

                case IDC_DELALL:
                {
                    int TreeCount=TreeView_GetCount(GetDlgItem(hWnd,IDC_TREE1));
                    for (int i= 0;i<TreeCount;i++)
                        TreeView_DeleteAllItems(GetDlgItem(hWnd,IDC_TREE1));
                }
                break;
                */
                default: {
                    debug << "1 Some other switch value " << LOWORD(wParam) << endl;
                } break;

            }
            break;

            case WM_CLOSE: // We closing the Dialog
            {
                EndDialog(hWnd, 0);
            }
            break;
            default: {
                debug << "2 Some other switch value " << message << endl;
            } break;
        }
        break;
    }
    return 0;
}
//-----------------------------------------------------------------------------
/*
int addChild(HWND hWnd,HTREEITEM* parent, char* t1,int icon,TreeData* td)
{
    if (strcmp(t1,"..")!= 0 && strcmp(t1,".")!= 0) {
        debug<<"AddChild: "<<t1<<endl;
        tvinsert.hParent=*parent;
        tvinsert.hInsertAfter=TVI_LAST;
        tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
        tvinsert.item.pszText=t1;
        if (td)
            tvinsert.item.lParam=reinterpret_cast<LPARAM>(td); //TreeData pointer
        if (icon==ICON_TREE) {
            tvinsert.item.iImage= 0;
            tvinsert.item.iSelectedImage=1;
        }
        if (icon==ICON_FILE) {
            tvinsert.item.iImage=2;
            tvinsert.item.iSelectedImage=2;
        }
        SendDlgItemMessage(hWnd,IDC_TREE1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
    }
    else debug<<"AddChild: ignoring '.' or '..'" << endl;
    return 0;
}
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void AddAttrInfo(char* pszAttrName, char* pszAttrData, char* type, int iAccess)
{
    LVITEMA LvItem;
    char* buff = pszAttrData;
    char pszBuff[256] = {0};
    int y = iAccess + 1;

    itoa(y, pszBuff, 10);

    LvItem.mask = LVIF_TEXT;
    LvItem.iItem = iAccess;
    LvItem.iSubItem = 0;
    LvItem.pszText = pszBuff;

    SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM) &LvItem);

    LvItem.mask = LVIF_TEXT;
    LvItem.iSubItem = 1;
    LvItem.pszText = pszAttrName;
    SendMessage(hList, LVM_SETITEM, 0, (LPARAM) &LvItem);

    LvItem.mask = LVIF_TEXT;
    LvItem.iSubItem = 2;
    LvItem.pszText = pszAttrData;
    SendMessage(hList, LVM_SETITEM, 0, (LPARAM) &LvItem);

    LvItem.mask = LVIF_TEXT;
    LvItem.iSubItem = 3;
    LvItem.pszText = type;
    SendMessage(hList, LVM_SETITEM, 0, (LPARAM) &LvItem);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void SetupListView(void)
{
    LVCOLUMNA LvCol;

    memset(&LvCol, 0, sizeof(LvCol));

    LvCol.mask = LVCF_TEXT | LVCF_WIDTH |LVCF_SUBITEM;
    LvCol.pszText = "#";
    LvCol.cx = 20;
    SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM) &LvCol);

    LvCol.mask = LVCF_TEXT | LVCF_WIDTH |LVCF_SUBITEM;
    LvCol.pszText = "Name";
    LvCol.cx = 75;
    SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM) &LvCol);

    LvCol.mask = LVCF_TEXT | LVCF_WIDTH |LVCF_SUBITEM;
    LvCol.pszText = "Data";
    LvCol.cx = 155;
    SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM) &LvCol);

    LvCol.mask = LVCF_TEXT | LVCF_WIDTH |LVCF_SUBITEM;
    LvCol.pszText = "Type";
    LvCol.cx = 155;
    SendMessage(hList, LVM_INSERTCOLUMN, 3, (LPARAM) &LvCol);

    SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) // what are we doing ?
    {
        case WM_INITDIALOG:
        {

        }

        case WM_COMMAND: // Controling the Buttons
        {
            switch (LOWORD(wParam)) // what we pressed on?
            {
                case IDOK:
                {
                    //close the dialog
                    EndDialog(hWnd, 0);
                }
                break;
            }
            break;

            case WM_CLOSE: // We closing the Dialog
            {
                EndDialog(hWnd, 0);
            }
            break;

            default: {
                debug << "3 Some other switch value "  << endl;
            } break;		}
    }
    return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

__int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, __int32 iCmdShow)
{
#ifdef __MSVC__
//	_CrtDumpMemoryLeaks();
#endif
    hInst = hInstance;
    Load_NTDLL();

    if (AllocConsole()) {
    int ifd = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
    *stdin  = *fdopen(ifd, "r");
    setvbuf( stdin, NULL, _IONBF, 0 );
    // redirect unbuffered STDIN to the console
    int ofd = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    *stdout = *fdopen(ofd, "w");
     setvbuf( stdout, NULL, _IONBF, 0 );
    // redirect unbuffered STOUT to the console
    int efd = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    *stderr = *fdopen(efd, "w");
    setvbuf( stderr, NULL, _IONBF, 0 );
    // redirect unbuffered STERR to the console
    }

    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, 0);
    debug << "Exit application.\n";
    debug.close();
    UnLoad_NTDLL();
    return 0;
}
//-----------------------------------------------------------------------------












