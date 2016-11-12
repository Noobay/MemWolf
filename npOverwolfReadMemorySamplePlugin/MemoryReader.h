/**
*   Copyright (c) 2016 Adar "Noobay" Grof
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

#ifndef MEM_READ_H
#define MEM_READ_H


#include "winternl64.h"
#include "wow64imp.h"
#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <assert.h>

#define PROC_BASIC_INFO 0
#define NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME  "NtWow64QueryInformationProcess64"
#define NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME  "NtWow64ReadVirtualMemory64"
#define NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME  "NtWow64WriteVirtualMemory64"

class MemoryOperationObject {

public:
	//load ntdll wow64 functions from ntdll, and declare them
#pragma region Load from ntdll
	HINSTANCE ntdll;
	FUNC_NtWow64QueryInformationProcess64 NtQueryInformationProcess64;
	FUNC_NtWow64ReadVirtualMemory64 NtReadVirtualMemory64;
	FUNC_NtWow64WriteVirtualMemory64 NtWriteVirtualMemory64;
#pragma endregion 

	//function declarations
#pragma region Member function declarations		
	int InitMemoryOperation(const char* windowClass, const char* caption, DWORD addressOffset);
	int InitMemoryOperation64(const char* windowClass, const char* caption, SYM addressOffset);
	bool FinishReadingMemory();
#pragma endregion

#pragma region Member template function declarations+definitions
	template<typename _ret_t> inline _ret_t ReadMemory()
	{
		//prevent the creation of a null pointer
		assert(!std::is_pointer<_ret_t>::value, "The type _ret_t must not be a pointer!");
		_ret_t ret = NULL;
		BOOL result = ReadProcessMemory(PROC_HANDLE, (void*)address, &ret, sizeof(ret), &bytesRead);

		if (result != 0)
		{
			//ReadProcessMemory Failed
		}
		return ret;
	}
	template <typename _ret_t>  _ret_t ReadMemory64()
	{
		//prevent the creation of a null pointer
		//TODO: Check if an "assert" is the best solution here
		assert(!std::is_pointer<_ret_t>::value, "The type _ret_t must not be a pointer!");
		_ret_t ret = NULL;
		NTSTATUS result = NtReadVirtualMemory64(PROC_HANDLE, (PVOID64)(address64), &ret, sizeof(ret), NULL);

		if (result != 0)
		{
			//NtReadVirtualMemory64 Failed
		}
		return ret;
	}

	template <typename _write_t> void WriteMemory(_write_t w_data, ULONG byte_length)
	{
		BOOL result = WriteProcessMemory(PROC_HANDLE, (void*)address, w_data, byte_length, NULL);

		if (result != 0)
		{
			//WriteProcessMemory Failed
		}
	}
	template <typename _write_t> void WriteMemory64(_write_t w_data, ULONG byte_length)
	{
		NTSTATUS result = NtWriteVirtualMemory64(PROC_HANDLE, (PVOID64)(address64), w_data, byte_length, NULL);
		if (result != 0)
		{
			//NtWriteProcessMemory64 Failed
		}
	}
#pragma endregion

	//variables 
#pragma region Member variables
	HWND  WINDOW_HANDLE;
	HANDLE PROC_HANDLE;
	DWORD PROC_ID;
	ULONGLONG address;
	SYM address64;
	SIZE_T bytesRead;
	SIZE_T64 bytesRead64;
#pragma endregion

MemoryOperationObject();

};

#endif