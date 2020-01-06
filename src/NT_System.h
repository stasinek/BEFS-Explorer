#ifndef NT_SYSTEM_H
#define NT_SYSTEM_H
//---------------------------------------------------------------------------
#ifndef _In_
#define _In_
#define _Inout_
#define _In_opt_
#define _Out_
#endif
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <windows.h>
#include <winioctl.h>
//---------------------------------------------------------------------------
#define NTSTATUS __int32
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
//---------------------------------------------------------------------------
#define OBJ_CASE_INSENSITIVE   0x00000040
#define InitializeObjectAttributes(p,n,a,r,s)\
{ \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
    (p)->RootDirectory = r; \
    (p)->Attributes = a; \
    (p)->ObjectName = n; \
    (p)->SecurityDescriptor = s; \
    (p)->SecurityQualityOfService = NULL; \
}
typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID    Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
//---------------------------------------------------------------------------
typedef struct _LSA_UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;
//---------------------------------------------------------------------------
typedef struct _OBJECT_ATTRIBUTES {
  ULONG           Length;
  HANDLE          RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG           Attributes;
  PVOID           SecurityDescriptor;
  PVOID           SecurityQualityOfService;
}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
//---------------------------------------------------------------------------
#define 	FILE_OPEN   0x00000001
#define 	FILE_SYNCHRONOUS_IO_NONALERT   0x00000020
#if !defined(__MSVC__)
typedef NTSTATUS __stdcall (*__NtCreateFile)(
      PHANDLE            FileHandle,
      ACCESS_MASK        DesiredAccess,
      POBJECT_ATTRIBUTES ObjectAttributes,
      PIO_STATUS_BLOCK   IoStatusBlock,
   PLARGE_INTEGER     AllocationSize,
       ULONG              FileAttributes,
       ULONG              ShareAccess,
       ULONG              CreateDisposition,
       ULONG              CreateOptions,
       PVOID              EaBuffer,
       ULONG              EaLength
);
extern __NtCreateFile NtCreateFile;
//---------------------------------------------------------------------------
typedef VOID(NTAPI * 	PIO_APC_ROUTINE )(_In_ PVOID ApcContext, _In_ PIO_STATUS_BLOCK IoStatusBlock, _In_ ULONG Reserved);
typedef NTSTATUS __stdcall (*__NtReadFile)(
  _In_     HANDLE           FileHandle,
  _In_opt_ HANDLE           Event,
  _In_opt_ PIO_APC_ROUTINE  ApcRoutine,
  _In_opt_ PVOID            ApcContext,
  _Out_    PIO_STATUS_BLOCK IoStatusBlock,
  _Out_    PVOID            Buffer,
  _In_     ULONG            Length,
  _In_opt_ PLARGE_INTEGER   ByteOffset,
  _In_opt_ PULONG           Key
);
extern __NtReadFile NtReadFile;
//---------------------------------------------------------------------------
typedef NTSTATUS __stdcall (*__NtWriteFile)(
  _In_     HANDLE           FileHandle,
  _In_opt_ HANDLE           Event,
  _In_opt_ PIO_APC_ROUTINE  ApcRoutine,
  _In_opt_ PVOID            ApcContext,
  _Out_    PIO_STATUS_BLOCK IoStatusBlock,
  _In_     PVOID            Buffer,
  _In_     ULONG            Length,
  _In_opt_ PLARGE_INTEGER   ByteOffset,
  _In_opt_ PULONG           Key
);
extern __NtWriteFile NtWriteFile;
//---------------------------------------------------------------------------
typedef NTSTATUS __stdcall (*__NtClose)(
  _In_ HANDLE Handle
);
extern __NtClose NtClose;
//---------------------------------------------------------------------------
typedef VOID __stdcall (*__RtlInitUnicodeString)(
  _Inout_  PUNICODE_STRING DestinationString,
  _In_opt_ PCWSTR          SourceString
);
extern __RtlInitUnicodeString RtlInitUnicodeString;
//---------------------------------------------------------------------------
int __stdcall Load_NTDLL(void);
int __stdcall UnLoad_NTDLL(void);
#endif
//---------------------------------------------------------------------------

#endif
