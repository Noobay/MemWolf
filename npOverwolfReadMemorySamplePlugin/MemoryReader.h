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


class MemoryReaderObject {

public:
		//load ntdll wow64 functions from ntdll, and declare them
	#pragma region Load from ntdll
		HINSTANCE ntdll;
		FUNC_NtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64;
	    FUNC_NtReadVirtualMemory64 NtReadVirtualMemory64;
	#pragma endregion 

	//function declarations
	#pragma region Function declarations
		
		template <typename _ret_t> _ret_t ReadMemory();
		template <typename _ret_t> _ret_t ReadMemory64();
	
		int InitMemoryRead(const char* windowClass, const char* caption, DWORD addressOffset);
		int InitMemoryRead64(const char* windowClass, const char* caption, SYM addressOffset);
		bool FinishReadingMemory();
	#pragma endregion


	//variable declarations 
	#pragma region Variable declarations
		HWND  WINDOW_HANDLE;
		HANDLE PROC_HANDLE;
		DWORD PROC_ID;
		ULONGLONG address;
		SYM address64;
		SIZE_T bytesRead;
		SIZE_T64 bytesRead64;
	#pragma endregion

	MemoryReaderObject();
	
};

#endif