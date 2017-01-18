//---------------------------------------------------------------------------
#pragma hdrstop
#include <stdlib.h>
#include <assert.h>
#include "NT_Native.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

HMODULE ntdll_handle = NULL;
BOOL ntdll_loaded = false;
__NtCreateFile *NtCreateFile;
__NtReadFile *NtReadFile;
__NtWriteFile *NtWriteFile;
__NtClose *NtClose;
__RtlInitUnicodeString *RtlInitUnicodeString;

//---------------------------------------------------------------------------

void NtLoad(void)
{
if (ntdll_loaded==true)
	{
	ntdll_handle = LoadLibraryA("ntdll.dll");
	if (NULL!=ntdll_handle)
   		{
        ntdll_loaded=true;
      	// If the function address is valid, call the function.
      	NtCreateFile = (__NtCreateFile*)GetProcAddress(ntdll_handle,"NtCreateFile");
      	assert(NtCreateFile!=NULL);
      	NtReadFile = (__NtReadFile*)GetProcAddress(ntdll_handle,"NtReadFile");
      	assert(NtReadFile!=NULL);
      	NtWriteFile = (__NtWriteFile*)GetProcAddress(ntdll_handle,"NtWriteFile");
      	assert(NtWriteFile!=NULL);
      	NtClose = (__NtClose*)GetProcAddress(ntdll_handle,"NtClose");
      	assert(NtClose!=NULL);
		}
   }
return;
}

//---------------------------------------------------------------------------

void NtUnload(void)
{
if (ntdll_loaded) { FreeLibrary(ntdll_handle); ntdll_loaded = false; ntdll_handle = NULL; }

}

//---------------------------------------------------------------------------

