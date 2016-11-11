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

#include "MemoryReader.h"

MemoryReaderObject::MemoryReaderObject()
{
	ntdll = LoadLibrary("ntdll.dll");
	NtWow64QueryInformationProcess64 = (FUNC_NtWow64QueryInformationProcess64)GetProcAddress(ntdll, NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME);
	NtReadVirtualMemory64 = (FUNC_NtReadVirtualMemory64)GetProcAddress(ntdll, NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME);
}

#pragma region initialization functions
//Read 32-bit 
int MemoryReaderObject::InitMemoryRead64(const char* windowClass, const char* caption, SYM addressOffset)
{

	DWORD result;
	BOOL enumResult;
	ULONG read_length=0;
	PROCESS_BASIC_INFORMATION64 procInfo;
	PPEB_LDR_DATA64 pld;

//allocate memory for function-scope variables
#pragma region allocate memory 
	pld = (PPEB_LDR_DATA64)malloc(sizeof(PEB_LDR_DATA64));
#pragma endregion
	 
	ZeroMemory(&procInfo, sizeof(procInfo));
	
	//Get the window handle
	WINDOW_HANDLE = FindWindow(windowClass, NULL);
	if (WINDOW_HANDLE == NULL)
	{
		//Window was not foud

		return 10;
	}

	//Get the process ID
	result = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (result == 0)
	{
		//Getting Process ID failed

		return 20;
	}

	//Open the process
	PROC_HANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, PROC_ID);

	address64 = addressOffset;
	if (PROC_HANDLE == NULL)
	{
		//Process failed to ope

		return 30;
	}
	
	//Query the process information
 	result = NtWow64QueryInformationProcess64( PROC_HANDLE, 0, &procInfo, sizeof(procInfo), &read_length);
	if (result != 0)
	{
		//Failed to query process information

		return 40;
	}

	//Read the PEB LDR data from memory and store it
	NtReadVirtualMemory64(PROC_HANDLE, (PVOID64)(procInfo.PebBaseAddress/* + sizeof(BYTE) * 8*/), pld,  sizeof(PEB_LDR_DATA64), NULL);
	//Retrieve the main handle's adress from the process' LDR 
	//and use the offset to reach the destination address 
	address64 = (SYM)(pld->MainHandle+addressOffset);

#pragma region release pointers
	//release pointers
	free(pld);
#pragma endregion

	return 1;
}


/*
Initiate Memory Reading, 
get the application's main process Handle, an5d entry point from it's main module.
we have to compile on x64(64bit) to be able to read 64bit and 32bit(x86) processes.
since Overwolf is a 32bit application, this will not be able to inherently read x64 processes since EnumProcessModules will not succeed!
*/
int MemoryReaderObject::InitMemoryRead(const char* windowClass, const char* caption, DWORD addressOffset)
{

	DWORD result;
	DWORD cbNeeded;
	HMODULE mainModule;
	BOOL enumResult;

	//Get the window handle
	WINDOW_HANDLE = FindWindow(windowClass, NULL);
	if (WINDOW_HANDLE == NULL)
	{
		//Window was not foud
		return 10;
	}

	//Get the process ID
	result = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (result == 0)
	{
		//Getting Process ID failed
		return 20;
	}

	//Open the process
	PROC_HANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, PROC_ID);

	if (PROC_HANDLE == NULL)
	{
		//Process failed to open
		return 30;
	}


	/*
	*Get the Main Module-
	*first entry in the returned HMODULE array from
	*EnumProcessModules
	*/

	enumResult = EnumProcessModulesEx(PROC_HANDLE, &mainModule, sizeof(mainModule), &cbNeeded, LIST_MODULES_64BIT);

	if (enumResult == 0)
	{
		//Failed enumerating process modules
		return 40;
	}
	
	//offset the requested memory address from the application's main module base address
	address = (unsigned long long)((UINT_PTR)mainModule  + addressOffset);

	return 1;
}
#pragma endregion

#pragma region Memory Reading Functions


template<typename _ret_t> _ret_t MemoryReaderObject::ReadMemory()
{ 
	//prevent the creation of a null pointer
	assert(!std::is_pointer<_ret_t>::value, "The type _ret_t must not be a pointer!");
	_ret_t ret = NULL;
	BOOL result = ReadProcessMemory(PROC_HANDLE, (void*)address, &ret, sizeof(ret), &bytesRead);

	if (result != 0)
	{
		//NtReadVirtualMemory Failed
	}
	return ret;
 }

template <typename _ret_t> _ret_t MemoryReaderObject::ReadMemory64()
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


//TODO: Move these to a seperate file
#pragma region dll template exports
template __declspec(dllexport) byte MemoryReaderObject::ReadMemory<byte>();
template __declspec(dllexport) int MemoryReaderObject::ReadMemory<int>();
template __declspec(dllexport) WORD MemoryReaderObject::ReadMemory<WORD>();
template __declspec(dllexport) DWORD MemoryReaderObject::ReadMemory<DWORD>();

template __declspec(dllexport) byte MemoryReaderObject::ReadMemory64<byte>();
template __declspec(dllexport) int MemoryReaderObject::ReadMemory64<int>();
template __declspec(dllexport) WORD MemoryReaderObject::ReadMemory64<WORD>();
template __declspec(dllexport) DWORD MemoryReaderObject::ReadMemory64<DWORD>();
#pragma endregion

#pragma endregion

bool MemoryReaderObject::FinishReadingMemory() 
{
	return CloseHandle(PROC_HANDLE);
}
