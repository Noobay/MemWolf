/**
* Copyright (c) 2016 Adar "Noobay" Grof
* 
*	This file is part of MemWolf.
*
*	MemWolf is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   MemWolf program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <winternl.h>
#include <sstream>


#define PROC_BASIC_INFO 0
#define NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME  "NtWow64QueryInformationProcess64"
#define NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME  "NtWow64ReadVirtualMemory64"


//definitions for essential 64 bit memory mangling functions from ntdll
#pragma region ntdll function definitions
typedef NTSTATUS(NTAPI *FUNC_NtReadVirtualMemory64)
(
	IN  HANDLE  ProcessHandle,
	IN  PVOID64 BaseAddress,
	OUT void*   Buffer,
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

//64-bit data structures and data types essential for conversion for use with 64-bit functions.
//NOTE: Some structures are entirely different from their 32-bit counterparts (not only in sizes, but in structure itself). 
#pragma region 64bit structures and types

//type redefinitions for a better understanding of the code(the current ones aren't necessarry) 
#pragma region 64-bit type definitions 
typedef UINT64 SYM;
typedef SIZE_T SIZE_T64;
#pragma endregion

//structure redfinitions for 64-bit functions
#pragma region 64-bit structure redefinitinos
typedef struct _UNICODE_STRING64
{
	USHORT	Length;
	USHORT	MaximumLength;
	ULONG	Reserved;
	ULONG64 Buffer;
} UNICODE_STRING64, *PUNICODE_STRING64;

typedef struct __RTL_USER_PROCESS_PARAMETERS {
	BYTE           Reserved1[16];
	PVOID64          Reserved2[10];
	UNICODE_STRING64 ImagePathName;
	UNICODE_STRING64 CommandLine;
} RTL_USER_PROCESS_PARAMETERS64, *PRTL_USER_PROCESS_PARAMETERS64;

typedef struct __PEB_LDR_DATA
{
	BYTE Reserved[8];
	ULONG Length;
	UCHAR Initialized;	
	DWORD64 MainHandle;
	LIST_ENTRY64 InLoadOrderModuleList;
	LIST_ENTRY64 InMemoryOrderModuleList;
	LIST_ENTRY64 InInitializationOrderModuleList;
	DWORD64 EntryInProgress;
} PEB_LDR_DATA64, *PPEB_LDR_DATA64;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
	LIST_ENTRY64		InLoadOrderModuleList;
	LIST_ENTRY64		InMemoryOrderModuleList;
	LIST_ENTRY64		InInitializationOrderModuleList;
	ULONG64				DllBase;
	ULONG64				EntryPoint;
	ULONG				SizeOfImage;
	UNICODE_STRING64	FullDllName;
	UNICODE_STRING64	BaseDllName;
	ULONG				Flags;
	USHORT				LoadCount;
} LDR_DATA_TABLE_ENTRY64, *PLDR_DATA_TABLE_ENTRY64;

typedef struct __PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[21];
	PPEB_LDR_DATA64 LoaderData;
	PRTL_USER_PROCESS_PARAMETERS64 ProcessParameters;
	BYTE Reserved3[520];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved4[136];
	ULONG64 SessionId;
}PEB64, *PPEB64;

struct PROCESS_BASIC_INFORMATION64 {

    SYM Reserved1;
    SYM PebBaseAddress;
	SYM Reserved2[2];
	SYM UniqueProcessId;
	SYM Reserved3;
};
#pragma endregion 
#pragma endregion

//load ntdll and wow64 functions from ntdll, and declare them
#pragma region Load from ntdll
HINSTANCE ntdll = LoadLibrary("ntdll.dll");
FUNC_NtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (FUNC_NtWow64QueryInformationProcess64)GetProcAddress(ntdll, NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME);
FUNC_NtReadVirtualMemory64 NtReadVirtualMemory64 = (FUNC_NtReadVirtualMemory64)GetProcAddress(ntdll, NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME);
#pragma endregion 

//function declarations
#pragma region Function declarations
template<typename _ret_t> _ret_t ReadMemory();
template<typename _ret_t> _ret_t ReadMemory64();
#pragma endregion


//global variables 
#pragma region Global Variables
HWND   WINDOW_HANDLE;
HANDLE PROC_HANDLE;
DWORD PROC_ID;
ULONGLONG address;
SYM address64;
SIZE_T bytesRead;
SIZE_T64 bytesRead64;
#pragma endregion

using namespace std;
