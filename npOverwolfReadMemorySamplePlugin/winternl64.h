/**
*   Copyright (c) 2016 Adar Grof
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

#ifndef WINTERNAL64_H
#define WINTERNAL64_H

#include <windows.h>

//64-bit data structures and data types essential for conversion for use with 64-bit functions.
//NOTE: Some structures are entirely different from their 32-bit counterparts (not only in sizes, but in structure itself). 
#pragma region 64-bit structures and types

//type definitions for a better understanding of the code 
#pragma region 64-bit type definitions 
typedef UINT64 SYM;
typedef SIZE_T SIZE_T64;
//this one is taken from winternl.h, used in __PEB.
typedef void(__stdcall *PPS_POST_PROCESS_INIT_ROUTINE)(void);
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

#endif