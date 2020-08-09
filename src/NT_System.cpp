//---------------------------------------------------------------------------
#include <NT_System.h>
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <assert.h>
//---------------------------------------------------------------------------
static HMODULE __ntdll_handle = NULL;
static    bool __ntdll_loaded = false;
//---------------------------------------------------------------------------
__NtCreateFile NtCreateFile;
__NtReadFile NtReadFile;
__NtWriteFile NtWriteFile;
__NtClose NtClose;
__RtlInitUnicodeString RtlInitUnicodeString;
//---------------------------------------------------------------------------
int __stdcall Load_NTDLL(void)
{
if (__ntdll_loaded==true) {
    return 1;
   }
__ntdll_handle = LoadLibraryA("ntdll.dll");
 if (__ntdll_handle!=NULL)
    {
    // If the function address is valid, set the function ptrs.
    NtCreateFile = (__NtCreateFile)GetProcAddress(__ntdll_handle,"NtCreateFile");
        assert(NtCreateFile!=NULL);
    NtReadFile = (__NtReadFile)GetProcAddress(__ntdll_handle,"NtReadFile");
        assert(NtReadFile!=NULL);
    NtWriteFile = (__NtWriteFile)GetProcAddress(__ntdll_handle,"NtWriteFile");
        assert(NtWriteFile!=NULL);
    NtClose = (__NtClose)GetProcAddress(__ntdll_handle,"NtClose");
        assert(NtClose!=NULL);
    __ntdll_loaded = true;
    return 0;
   }
    return 1;
}
//---------------------------------------------------------------------------

int __stdcall UnLoad_NTDLL(void)
{
if (__ntdll_loaded==true) {
    FreeLibrary(__ntdll_handle);
    __ntdll_loaded = false; __ntdll_handle = NULL;
   }
return 0;
}


//---------------------------------------------------------------------------

