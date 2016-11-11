#ifndef WOW64IMP_H
#define WOW64IMP_H

#include <Windows.h>

//definitions for essential 64 bit memory mangling functions from ntdll
#pragma region ntdll function definitions
typedef NTSTATUS(NTAPI *FUNC_NtReadVirtualMemory64)
(
	IN  HANDLE  ProcessHandle,
	IN  PVOID64 BaseAddress,
	OUT PVOID   Buffer,
	IN  ULONGLONG BufferLength,
	OUT PULONGLONG ReturnLength OPTIONAL
);
typedef NTSTATUS(NTAPI *FUNC_NtWow64QueryInformationProcess64)
(
	IN  HANDLE ProcessHandle,
	IN  ULONG  ProcessInformationClass,
	OUT PVOID  ProcessInformation64,
	IN  ULONG  Length,
	OUT PULONG ReturnLength OPTIONAL
);
#pragma endregion

#endif