#include "MemoryRead.h"
#include <Windows.h>
#include <Psapi.h>
#include <Windows.h>
#include <Psapi.h>
#include <iostream>

#include <stdio.h>
#include <WinBase.h>
#include <libloaderapi.h>
#include <winternl.h>

#define PROC_BASIC_INFO 0
#define NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME  "NtWow64QueryInformationProcess64"
#define NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME  "NtWow64ReadVirtualMemory64"

typedef UINT64 SYM;
typedef SIZE_T SIZE_T64;

template<typename _ret_t> _ret_t ReadMemory(_ret_t& ret);
template<typename _ret_t> _ret_t ReadMemory64(_ret_t& ret);
bool SetWow64Params(BOOL& b_IsWow64Process, DWORD& dw_ProcessParametersOffset, DWORD dw_CommandLineOffset);

HWND   WINDOW_HANDLE;
HANDLE PROC_HANDLE;
DWORD PROC_ID;
ULONGLONG address;
UINT64 address64;
SIZE_T bytesRead;
SIZE_T64 bytesRead64;

using namespace std;


//initialize variables for importing of essential 64 bit reading functions
//from ntdll
typedef NTSTATUS(NTAPI *FUNC_NtReadVirtualMemory64)
( 
	IN  HANDLE  ProcessHandle,
	IN  PVOID64 BaseAddress,
    OUT void*   Buffer,
	IN  ULONGLONG BufferLength,
	OUT PULONGLONG ReturnLength OPTIONAL
);
typedef NTSTATUS (NTAPI *FUNC_NtWow64QueryInformationProcess64) 
(
	IN  HANDLE ProcessHandle,
	IN  ULONG  ProcessInformationClass,
	OUT PVOID  ProcessInformation64,
	IN  ULONG  Length,
	OUT PULONG ReturnLength OPTIONAL
);

struct PROCESS_BASIC_INFORMATION64 {

	SYM Reserved1;
	SYM PebBaseAddress;
	SYM Reserved2[2];
	SYM UniqueProcessId;
	SYM Reserved3;
	/*
	NTSTATUS ExitStatus;
	ULONG64 PebBaseAddress;
	ULONG64 AffinityMask;
	LONG    BasePriority;
	UINT64  Reserved1;
	ULONG64 UniqueProcessId;
	ULONG64 InheritedFromUniqueProcessId;
	*/
};



HINSTANCE ntdll = LoadLibrary("ntdll.dll");
FUNC_NtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (FUNC_NtWow64QueryInformationProcess64)GetProcAddress(ntdll, NT_WOW64_QUERY_INFORMATION_PROCESS_64_NAME);
FUNC_NtReadVirtualMemory64 NtReadVirtualMemory64 = (FUNC_NtReadVirtualMemory64)GetProcAddress(ntdll, NT_WOW64_READ_VIRTUAL_MEMORY_64_NAME);

int Init32To64MemoryRead(const char* windowClass, const char* caption, SYM addressOffset)
{

	DWORD cbNeeded;
	DWORD dwdResult;
	HMODULE mainModule;
	BOOL enumResult;
	ULONG read_length=0;
	HINSTANCE ntdll; 
	PROCESS_BASIC_INFORMATION64 procInfo;
	ZeroMemory(&procInfo, sizeof(procInfo));
	

	
	//Get the window handle
	WINDOW_HANDLE = FindWindow(windowClass, NULL);
	if (WINDOW_HANDLE == NULL)
	{
		//Window was not foud
		return 10;
	}

	//Get the process ID
	dwdResult = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (dwdResult == 0)
	{
		//Getting Process ID failed
		return 20;
	}

	//Open the process
	PROC_HANDLE = OpenProcess(PROCESS_ALL_ACCESS, false, PROC_ID);

	if (PROC_HANDLE == NULL)
	{
		//Process failed to open
		return 30;
	}
	DWORD result;
	
 	result = NtWow64QueryInformationProcess64( PROC_HANDLE, 0, &procInfo, sizeof(procInfo), &read_length);
	
	if (result != 0)
	{
		cerr << "Query Information Process has failed" << endl;
	
		return 40;
	}
	
	PEB *peb = (PEB*)procInfo.PebBaseAddress;
	
	address64 = procInfo.PebBaseAddress + addressOffset;
	
	//* lazy debug
	cerr << address64 << endl;

 	string number;
	stringstream stristream;

	stristream << address64;
	stristream >> number;
	//*/
	UINT64 testByte = 120;
	(UINT64)ReadMemory64<UINT64>(testByte);
	cout << "RESULT: " << (UINT64)testByte << endl;
	system("PAUSE");
	return 1;
}

/*
Initiate Memory Reading, 
get the application's main process Handle, and entry point from it's main module.
we have to compile on x64(64bit) to be able to read 64bit and 32bit(x86) processes.
since Overwolf is a 32bit application, this will not be able to inherently read x64 processes since EnumProcessModules will not succeed!
*/

int InitReadMemory(const char* windowClass, const char* caption, DWORD addressOffset)
{

	DWORD cbNeeded;
	DWORD dwdResult;
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
	dwdResult = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (dwdResult == 0)
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

	//Check if the main Process is a Wow64Process, if it is, fill up the necesarry options
	BOOL b_IsWow64Proc;
	DWORD procOffset;
	DWORD cmdOffset;
	SetWow64Params(b_IsWow64Proc, procOffset, cmdOffset);
	/*
	*Get the Main Module-
	*first entry in the returned HMODULE array from
	*EnumProcessModules
	*/

	system("PAUSE");
	cout << GetLastError() << endl;
	enumResult = EnumProcessModulesEx(PROC_HANDLE, &mainModule, sizeof(mainModule), &cbNeeded, LIST_MODULES_64BIT);
	cout << GetLastError() << endl;

	if (enumResult == 0)
	{
		system("explorer http://www.error.com");
		//Failed enumerating process modules
		return 40;
	}



	
	//offset the requested memory address from the application's base address
	address = (unsigned long long)((UINT_PTR)mainModule  + addressOffset);

	string number;
	stringstream stristream;

	stristream << address;
	stristream >> number;

	byte testByte = 0 ;

	(byte)ReadMemory<byte>(testByte);
	cout << "the value: " << (short)testByte  << "\r\nwas returned from memory address: " << number << "\r\nexecutable located in: " << mainModule << endl;
	system("PAUSE");

	return 1;
}


template<typename _ret_t> _ret_t ReadMemory(_ret_t& ret)
{
	
	ReadProcessMemory(PROC_HANDLE, (void*)address, &ret, sizeof(ret), &bytesRead);

	return ret;
	
 };

 
template <typename _ret_t> _ret_t ReadMemory64(_ret_t& ret)
{
    
	NTSTATUS result = NtReadVirtualMemory64(PROC_HANDLE, (PVOID64)(0x7FF71D7605AB), &ret, sizeof(_ret_t), NULL);

	return ret;
 };

 /*
bool isLootAvailable()
{
	if (IsWindowVisible(WINDOW_HANDLE))
	{
		//64 means loot is available to us while 0 is none
		if ((WORD)ReadMemory<WORD>() == 64)
		{
			return true;
		}
	}
	return false;
}
*/

bool FinishReadingMemory()
{
	return CloseHandle(PROC_HANDLE);
}


bool SetWow64Params(BOOL& b_IsWow64Process, DWORD& dw_ProcessParametersOffset, DWORD dw_CommandLineOffset)
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	IsWow64Process(GetCurrentProcess(), &b_IsWow64Process);

	if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		dw_ProcessParametersOffset =  0x20;
		dw_CommandLineOffset = 0x70;
	}
	else
	{
		dw_ProcessParametersOffset = 0x10;
		dw_CommandLineOffset = 0x40;
	}
}
w
using namespace std;
int main()
{
		
	Init32To64MemoryRead("ArenaNet_Dx_Window_Class", NULL, (UINT64)0x1D205AB);
	system("PAUSE");

}
