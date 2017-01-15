/* Author: Peter Speybrouck - peter.speybrouck@gmail.com
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 */


#ifndef NATIVE_H
#define NATIVE_H

//
// NT Function calls, renamed NtXXXX from their ZwXXXX counterparts in NTDDK.H
//
extern "C" 
NTSYSAPI
NTSTATUS
NTAPI
NtReadFile(HANDLE FileHandle,
	HANDLE Event OPTIONAL,
	PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	PVOID ApcContext OPTIONAL,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER ByteOffset OPTIONAL,
	PULONG Key OPTIONAL);

extern "C" 
NTSYSAPI
NTSTATUS
NTAPI
NtWriteFile(HANDLE FileHandle,
	HANDLE Event OPTIONAL,
	PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	PVOID ApcContext OPTIONAL,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID Buffer,
	ULONG Length,
	PLARGE_INTEGER ByteOffset OPTIONAL,
	PULONG Key OPTIONAL);

extern "C" 
NTSYSAPI
NTSTATUS
NTAPI
NtClose(HANDLE Handle);

extern "C" 
NTSYSAPI
NTSTATUS
NTAPI
NtCreateFile(PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize OPTIONAL,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer OPTIONAL,
	ULONG EaLength);

#endif
