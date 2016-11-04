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
	if (ntdll == NULL)
	{
		ntdll = LoadLibrary("ntdll.dll");
		NtReadVirtualMemory64 = (FUNC_NtReadVirtualMemory64)GetProcAddress(ntdll, NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME);
	}
	if (NtWow64QueryInformationProcess64 == NULL)
	{
		NtWow64QueryInformationProcess64 = (FUNC_NtWow64QueryInformationProcess64)GetProcAddress(ntdll, NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME);
	}
	if (NtReadVirtualMemory64 == NULL)
	{
		NtReadVirtualMemory64 = (FUNC_NtReadVirtualMemory64)GetProcAddress(ntdll, NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME);
	}

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
		MessageBox(GetActiveWindow(), "Failed in Retrieving Window Handle", "Error Getting HWND", MB_OK);

		return 10;
	}

	//Get the process ID
	result = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (result == 0)
	{
		//Getting Process ID failed
		MessageBox(GetActiveWindow(), "Failed in Retrieving Process ID", "Error Getting PROCID", MB_OK);

		return 20;
	}

	//Open the process
	PROC_HANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, PROC_ID);

	address64 = addressOffset;
	if (PROC_HANDLE == NULL)
	{
		//Process failed to ope
		MessageBox(GetActiveWindow(), "Process Failed to Open", "Error Opening Process", MB_OK);

		return 30;
	}
	
	//Query the process information
 	result = NtWow64QueryInformationProcess64( PROC_HANDLE, 0, &procInfo, sizeof(procInfo), &read_length);
	if (result != 0)
	{
		//Failed to query process information
		MessageBox(GetActiveWindow(), "Failed Querying Process Information", "Error Querying PROCINFO", MB_OK);

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
		MessageBox(GetActiveWindow(), "Failed in Retrieving Window Handle", "Error Getting HWND", MB_OK);

		return 10;
	}

	//Get the process ID
	result = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (result == 0)
	{
		//Getting Process ID failed
		MessageBox(GetActiveWindow(), "Failed in Retrieving Process ID", "Error Getting PROCID", MB_OK);

		return 20;
	}

	//Open the process
	PROC_HANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, PROC_ID);

	if (PROC_HANDLE == NULL)
	{
		//Process failed to open
		MessageBox(GetActiveWindow(), "Process Failed to Open", "Error Opening Process", MB_OK);

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
		MessageBox(GetActiveWindow(),"Failed Enumerating Process Modules", "Error Reading Memory", MB_OK);
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
		MessageBox(GetActiveWindow(), "Failed Reading Memory", "Memory Reading Error", MB_OK);
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
		MessageBox(GetActiveWindow(), "Failed Reading Memory From 64-bit Process", "64 bit Memory Reading Error", MB_OK);
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
