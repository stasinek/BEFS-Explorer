/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
*  ----------------------------------------------------------------------------
*  "THE BEER-WARE LICENSE" (Revision 42):
*  <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
*  can do whatever you want with this stuff. If we meet some day, and you think
*  this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
*  ----------------------------------------------------------------------------
*/
//-----------------------------------------------------------------------------
#include <GUI_DiskFunc.h>
#include <sstream>
//-----------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------

char* __stdcall GetLastErrorText(DWORD nErrorCode)
{
	// Ask Windows to prepare a standard message for a GetLastErrorText(GetLastError()) code:
	char* msg;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, nErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
	// Return the message
	if (!msg) return("Unknown error");
	else
		return(msg);
}
//-----------------------------------------------------------------------------

__int32 __stdcall CountPhysicalDrives()
{
    debug << "CountPhysicalDrives" << endl;
	__int32 index = 0;
	HANDLE hDevice; // handle to the drive to be examined
	char drive[60];
	do
    {
		sprintf(drive, PHYSICALDRIVE, index);
		hDevice = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_WRITE_THROUGH, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            if (index==0)
            {
                printf("Error in CountPhysicalDrives: could not open disk %i (error %s)\n", index, GetLastErrorText(GetLastError()));
                debug<<"Error in CountPhysicalDrives: could not open disk " << index << ", error " << GetLastErrorText(GetLastError()) << endl;
		    }
            return index;
	    }
		CloseHandle(hDevice);
		index++;
    }
	while (1);
    return index;
}
//-----------------------------------------------------------------------------

__int32 __stdcall ListDirectories(HWND hWnd, HTREEITEM* node, TVITEMA* tvi, ofstream* a_debug, vector<Volume>* volumes)
{
	void* iter;
    dirent_t d;
	__int32 children = 0;
	uint32_t num;
	TreeData* td = reinterpret_cast<TreeData*>(tvi->lParam);
	Volume* vv = (Volume*)td->extra;
	Volume* v = (Volume*)(&volumes->at(td->volume));
    *a_debug << "ListDir: " << tvi->pszText << " volume = " << v << " vol.Name = " << v->Name() << " hWnd = " << hWnd << endl;
     a_debug->flush();
    //if (vv->IsValidSuperBlock())* a_debug<<"2: VALID SUPERBLOCK " << vv <<" td->volume = " << td->volume << endl;
    //else* a_debug << "2: NO VALID SUPERBLOCK!!! " << v << " td->volume=" << td->volume << endl;
     a_debug->flush();

	if (v->IsValidSuperBlock())
        *a_debug<<"4: VALID SUPERBLOCK " << v << endl;
	else
        *a_debug<<"4: NO VALID SUPERBLOCK!!! " << v << endl;

     a_debug->flush();
	Inode* II;

	if (td->inode!=-1)
		II = new Inode(v,td->inode);
	else //we're reading the root directory
		II = v->RootNode();

	//bfs_open_dir(v, v->RootNode(), &iter);

	if (II->IsDirectory()) {
		bfs_open_dir(v, II, &iter);
        *a_debug << "after opendir...  inode: " << td->inode << endl;
        a_debug->flush();
		// second arg of bfs_read_dir is not used actually
		//status_t ss = bfs_read_dir(v, v->RootNode(), iter, &d,sizeof(dirent), &num);
        status_t ss = bfs_read_dir(v, II, iter, &d,sizeof(dirent_t), &num);
        *a_debug << "after readdir..." << endl;
        a_debug->flush();
		//TODO: this should be the '.' entry...
		if (ss==B_OK && num>0)
            *a_debug<<d.d_name<<endl;

        a_debug->flush();
		while (ss == B_OK && num > 0) {
            *a_debug << "before readdir\n";
            a_debug->flush();
            ss = bfs_read_dir(v, v->RootNode(), iter, &d,sizeof(dirent_t), &num);
			if (ss == B_OK && num > 0) {
                //a_debug<<"readdir success\n";a_debug.flush();
				//printf("dirent: %s, len=%i, ino=%i\n",d.d_name,d.d_reclen,d.d_ino);
				Inode* I = new Inode(v, d.d_ino);

				if (I->IsDirectory()) {
                    *a_debug << d.d_name << "(dir)" << endl;
                    a_debug->flush();
					TreeData* data = new TreeData(td->level+1, d.d_name, d.d_ino);
					data->extra = v;
					data->volume = td->volume;
                    AddChild(hWnd, node, d.d_name, ICON_TREE, data, a_debug);
					children++;
				}

				if (I->IsFile()) {
                    *a_debug << d.d_name << "(file)" << endl;
                    a_debug->flush();
					TreeData* data = new TreeData(2, d.d_name, d.d_ino);
					data->extra = v;
					data->volume = td->volume;
                    AddChild(hWnd, node, d.d_name, ICON_FILE, data, a_debug);
					children++;
				}

				if (!I->IsFile() && !I->IsDirectory()) {
                    *a_debug << d.d_name << "(?)" << endl;
                    a_debug->flush();
					TreeData* data = new TreeData(2, d.d_name, d.d_ino);
					data->extra = v;
					data->volume = td->volume;
                    AddChild(hWnd, node, d.d_name, ICON_OTHER, data,a_debug);
					children++;
				}
				free(I);
			}
            //a_debug<<"at end while loop\n"; a_debug.flush();
		}
	}
	return children;
}
//-----------------------------------------------------------------------------

__int32 __stdcall AddChild(HWND hWnd, HTREEITEM* parent, char* t1, __int32 icon, TreeData* td, std::ofstream* a_debug)
{
    *a_debug << "addChild: hWnd=" << hWnd << endl;
    *a_debug << "addChild: parent=" << *parent << ", level = " << td->level << endl;
    TVINSERTSTRUCTA tvinsert;   // struct to config out tree control
	if (strcmp(t1,"..")!= 0 && strcmp(t1,".")!= 0) {
        *a_debug<<"AddChild: " << t1 << " volume = " << td->extra << endl;
		tvinsert.hParent = *parent;
		tvinsert.hInsertAfter = TVI_LAST;
		tvinsert.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
		tvinsert.item.pszText = t1;
		if (td)
			tvinsert.item.lParam = reinterpret_cast<LPARAM>(td); //TreeData pointer

		if (icon == ICON_TREE) {
			tvinsert.item.iImage = 0;
			tvinsert.item.iSelectedImage = 1;
		}

		if (icon==ICON_FILE) {
			tvinsert.item.iImage = 2;
			tvinsert.item.iSelectedImage = 2;
		}
		//SendDlgItemMessage(hWnd,IDC_TREE1,TVM_INSERTITEM,0,(LPARAM)&tvinsert);
		TreeView_InsertItem(hWnd, (LPARAM)&tvinsert);
	} else
        *a_debug<<"AddChild: ignoring '.' or '..'" << endl;
	return 0;
}
//-----------------------------------------------------------------------------

__int32 __stdcall ListPartitions(__int32 disk, HWND h, HTREEITEM* node, TVINSERTSTRUCTA* s, std::ofstream* a_debug)
{
    *a_debug << "listPartitions" << endl;
	__int32 i, count = 0;
	DWORD dwBytes;
	PARTITION* PartitionTbl;
	DRIVEPACKET stDrive;

	BYTE szSector[512];

	WORD wDrive = 0;

	char szTmpStr[64];

	DWORD dwMainPrevRelSector = 0;
	DWORD dwPrevRelSector = 0;
	char drive[60];
	sprintf(drive, PHYSICALDRIVE, disk);
    *a_debug << "DRIVE = " << drive << endl;
	HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_WRITE_THROUGH, NULL);

	if (hDrive == INVALID_HANDLE_VALUE) {
		printf("\tError in listPartitions: could not open disk %i (error id %i, %s)\n", disk, GetLastError(), GetLastErrorText(GetLastError()));
        *a_debug<<"Error in listPartitions: could not open disk " << disk << ", error id " << GetLastError() << ", " << GetLastErrorText(GetLastError()) << endl;
		return GetLastError();
	}

	__int32 nRet = ReadFile(hDrive, szSector, 512, (DWORD*)&dwBytes ,NULL);

	if (!nRet) {
        printf("\tRead error in listPartitions: id %i %s\n", (__int32)GetLastError, GetLastErrorText(GetLastError()));
        *a_debug << "Read error in listPartitions: " << GetLastErrorText(GetLastError()) << endl;
		return GetLastError();
	}

	dwPrevRelSector = 0;
	dwMainPrevRelSector = 0;

	PartitionTbl = (PARTITION*)(szSector + 0x1BE); //0x1be=446

	__int32 nr_of_Loops = 4;

	for (i = 0; i < nr_of_Loops; i++) { /// scanning partitions in the physical disk
		stDrive.wCylinder = PartitionTbl->chCylinder;
		stDrive.wHead = PartitionTbl->chHead;
		stDrive.wSector = PartitionTbl->chSector;
		stDrive.dwNumSectors = PartitionTbl->dwNumberSectors;
		stDrive.wType = ((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X)) ? EXTENDED_PART:BOOT_RECORD;

		if ((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X)) {
			dwMainPrevRelSector = PartitionTbl->dwRelativeSector;
			stDrive.dwNTRelativeSector = dwMainPrevRelSector;
		} else {
			stDrive.dwNTRelativeSector = dwMainPrevRelSector + PartitionTbl->dwRelativeSector;
		}

		if (stDrive.wType == EXTENDED_PART)
			break;

		if (PartitionTbl->chType == 0)
			break;

		switch(PartitionTbl->chType)
		{
			case PART_DOS2_FAT:
				strcpy(szTmpStr, "FAT12");
				break;
			case PART_DOSX13:
			case PART_DOS4_FAT:
			case PART_DOS3_FAT:
				strcpy(szTmpStr, "FAT16");
				break;
			case PART_DOS32X:
			case PART_DOS32:
				strcpy(szTmpStr, "FAT32");			//Normal FAT32
				break;
			case PART_NTFS:
				strcpy(szTmpStr, "NTFS");	// NTFS
				break;
			case PART_EXT2:
				strcpy(szTmpStr, "EXT2/3");	//ext2 or ext3 partition
				break;
			case PART_SWAP:
				strcpy(szTmpStr, "Linux SWAP");	//linux swap partition
				break;
			case PART_SKYFS:
			{
				strcpy(szTmpStr, "SKYFS");	//skyfs partition
				s->hParent=*node;
				s->item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE| TVIF_PARAM;
				s->item.lParam= 0;
				ostringstream oss;
				oss << "Partition" << wDrive;
				string t = oss.str();
				TreeData* td = new TreeData(1,t.c_str());
				td->iData1 = disk;
				td->iData2 = wDrive;   // save partition number in an __int32
				s->item.lParam=reinterpret_cast<LPARAM>(td);
				oss << " ("<< szTmpStr<< ")";
				t = oss.str();
                s->item.pszText=(LPSTR)t.c_str();
				SendDlgItemMessage(h,IDC_TREE1,TVM_INSERTITEM,0,(LPARAM)s);
			}
				break;
			case PART_BEFS:
				{
				strcpy(szTmpStr, "BEFS");	//bfs partition
				s->hParent = *node;
				s->item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE| TVIF_PARAM;
				s->item.lParam = 0;
				ostringstream oss;
				oss << "Partition" << wDrive;
				string t = oss.str();
				TreeData* td = new TreeData(1, t.c_str());
				td->iData1 = disk;
				td->iData2 = wDrive;   // save partition number in an __int32
				s->item.lParam = reinterpret_cast<LPARAM>(td);
				oss << " ("<< szTmpStr<< ")";
				t = oss.str();
                s->item.pszText = (LPSTR)t.c_str();
				SendDlgItemMessage(h, IDC_TREE1, TVM_INSERTITEM, 0 , (LPARAM)s);
			}
				break;
			case PART_BEFS1:
			{
				strcpy(szTmpStr, "BEFS?");	//bfs partition
				s->hParent = *node;
				s->item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE| TVIF_PARAM;
				s->item.lParam = 0;
				ostringstream oss;
				oss << "Partition" << wDrive;
				string t = oss.str();
				TreeData* td = new TreeData(1, t.c_str());
				td->iData1 = disk;
				td->iData2 = wDrive;   // save partition number in an __int32
				s->item.lParam = reinterpret_cast<LPARAM>(td);
				oss << " ("<< szTmpStr<< ")";
				t = oss.str();
                s->item.pszText = (LPSTR)t.c_str();
				SendDlgItemMessage(h, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)s);
			}
				break;
			default:
				strcpy(szTmpStr, "Unknown");
				break;
		}

		printf("\t[%d] %s Drive, Primary (0x%x)\n", wDrive, szTmpStr, PartitionTbl->chType);
        *a_debug<<"\t[" << wDrive << "] " << szTmpStr << " Drive, Primary (" << (__int32)PartitionTbl->chType << ")" << endl;
		PartitionTbl++;
		wDrive++;
	}

	if (i == nr_of_Loops)
		return ERROR_SUCCESS;

	for (__int32 LogiHard = 0; LogiHard < 50; LogiHard++) { // scanning extended partitions

		if (stDrive.wType == EXTENDED_PART) {
			LARGE_INTEGER n64Pos;

			n64Pos.QuadPart = ((LONGLONG) stDrive.dwNTRelativeSector)*  512;

			__int32 nRet = SetFilePointer(hDrive, n64Pos.LowPart,&n64Pos.HighPart, FILE_BEGIN);
            if (nRet == (__int32)0xffffffff)
				return GetLastError();

			dwBytes = 0;

			nRet = ReadFile(hDrive, szSector, 512, (DWORD* ) &dwBytes, NULL);

			if (!nRet)
				return GetLastError();

			if (dwBytes != 512)
				return ERROR_READ_FAULT;

			PartitionTbl = (PARTITION* ) (szSector+0x1BE);

			for (i = 0; i < nr_of_Loops; i++) {
				stDrive.wCylinder = PartitionTbl->chCylinder;
				stDrive.wHead = PartitionTbl->chHead;
				stDrive.dwNumSectors = PartitionTbl->dwNumberSectors;
				stDrive.wSector = PartitionTbl->chSector;
				stDrive.dwRelativeSector = 0;
				stDrive.wType = ((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X)) ? EXTENDED_PART:BOOT_RECORD;

				if ((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X))
				{
					dwPrevRelSector = PartitionTbl->dwRelativeSector;
					stDrive.dwNTRelativeSector = dwPrevRelSector + dwMainPrevRelSector;
				}
				else
				{
					stDrive.dwNTRelativeSector = dwMainPrevRelSector + dwPrevRelSector + PartitionTbl->dwRelativeSector;
				}

				if (stDrive.wType == EXTENDED_PART)
					break;

				if (PartitionTbl->chType == 0)
					break;

				switch(PartitionTbl->chType)
				{
				case PART_DOS2_FAT:
					strcpy(szTmpStr, "FAT12");
					break;
				case PART_DOSX13:
				case PART_DOS4_FAT:
				case PART_DOS3_FAT:
					strcpy(szTmpStr, "FAT16");
					break;
				case PART_DOS32X:
				case PART_DOS32:
					strcpy(szTmpStr, "FAT32");			//Normal FAT32
					break;
				case 7:
					strcpy(szTmpStr, "NTFS");
					break;
				case 131:
					strcpy(szTmpStr, "EXT2/3");	//ext2 or ext3 partition
					break;
				case 130:
					strcpy(szTmpStr, "Linux SWAP");	//linux swap partition
					break;
				case PART_SKYFS:
				{
					strcpy(szTmpStr, "SKYFS");	//skyfs partition
					s->hParent=*node;
					s->item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE| TVIF_PARAM;
					s->item.lParam= 0;
					ostringstream oss;
					oss << "Partition" << wDrive;
					string t = oss.str();
					TreeData* td = new TreeData(1,t.c_str());
					td->iData1 = disk;
					td->iData2 = wDrive;   // save partition number in an __int32
					s->item.lParam=reinterpret_cast<LPARAM>(td);
					oss << " ("<< szTmpStr<< ")";
					t = oss.str();
                    s->item.pszText=(LPSTR)t.c_str();
					SendDlgItemMessage(h,IDC_TREE1,TVM_INSERTITEM,0,(LPARAM)s);
				}
					break;
				case PART_BEFS:
				{
					strcpy(szTmpStr, "BEFS");	//bfs partition
					s->hParent = *node;
					s->item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE| TVIF_PARAM;
					s->item.lParam = 0;
					ostringstream oss;
					oss << "Partition" << wDrive;
					string t = oss.str();
					TreeData* td = new TreeData(1, t.c_str());
					td->iData1 = disk;
					td->iData2 = wDrive;   // save partition number in an __int32
					s->item.lParam = reinterpret_cast<LPARAM>(td);
					oss << " (" << szTmpStr << ")";
					t = oss.str();
                    s->item.pszText = (LPSTR)t.c_str();
					SendDlgItemMessage(h, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)s);
				}
					break;
				case PART_BEFS1:
				{
					strcpy(szTmpStr, "BEFS?");	//bfs partition
					s->hParent = *node;
					s->item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE| TVIF_PARAM;
					s->item.lParam = 0;
					ostringstream oss;
					oss << "Partition" << wDrive;
					string t = oss.str();
					TreeData* td = new TreeData(1, t.c_str());
					td->iData1 = disk;
					td->iData2 = wDrive;   // save partition number in an __int32
					s->item.lParam = reinterpret_cast<LPARAM>(td);
					oss << " (" << szTmpStr << ")";
					t = oss.str();
                    s->item.pszText = (LPSTR)t.c_str();
					SendDlgItemMessage(h, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)s);
				}
					break;
				default:
					strcpy(szTmpStr, "Unknown");
					break;
				}
				printf("\t[%d] %s Drive, Logical (0x%x)\n", wDrive, szTmpStr, PartitionTbl->chType);
                *a_debug << "\t[" << wDrive << "] " << szTmpStr << " Drive, Logical (" << (__int32)PartitionTbl->chType << ")" << endl;
				PartitionTbl++;
				wDrive++;
			}

			if (i == nr_of_Loops)
				break;
		}
	}
	CloseHandle(hDrive);
	return count;
}
//-----------------------------------------------------------------------------

