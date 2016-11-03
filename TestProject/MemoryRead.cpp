#include "MemoryRead.h"

//Read 32-bit 
int Init32To64MemoryRead(const char* windowClass, const char* caption, SYM addressOffset)
{
	
	DWORD cbNeeded;
	DWORD dwdResult;
	BOOL enumResult;
	ULONG read_length=0;
	HINSTANCE ntdll; 
	PROCESS_BASIC_INFORMATION64 procInfo;
	PPEB_LDR_DATA64 pld;

//allocate memory for function-scope variables
#pragma allocate memory 
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
	dwdResult = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	if (dwdResult == 0)
	{
		//Getting Process ID failed
		return 20;
	}

	//Open the process
	PROC_HANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, PROC_ID);

	address64 = addressOffset;
	if (PROC_HANDLE == NULL)
	{
		//Process failed to open
		return 30;
	}
	DWORD result;
	
	//Query the process information
 	result = NtWow64QueryInformationProcess64( PROC_HANDLE, 0, &procInfo, sizeof(procInfo), &read_length);
	if (result != 0)
	{
		//Failed to query process information
		return 40;
	}
	
	NtReadVirtualMemory64(PROC_HANDLE, (PVOID64)(procInfo.PebBaseAddress/* + sizeof(BYTE) * 8*/), pld,  sizeof(PEB_LDR_DATA64), NULL);
		
	byte value = 0;

	address64 = (SYM)(pld->MainHandle+addressOffset);

	(byte)ReadMemory64<byte>();
 
 	cout << "RESULT: " << (short)value << endl;
	cout << address64 << endl;
#pragma region release pointers
	//release pointers
	free(pld);
#pragma endregion

	return 1;
}


//Memory Reading Functions
#pragma region Memory Reading Functions
/*
Initiate Memory Reading, 
get the application's main process Handle, an5d entry point from it's main module.
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

	(byte)ReadMemory<byte>();
	cout << "the value: " << (short)testByte  << "\r\nwas returned from memory address: " << number << "\r\nexecutable located in: " << mainModule << endl;
	system("PAUSE");

	return 1;
}
#pragma endregion

#pragma region Memory Reading Functions
template<typename _ret_t> _ret_t ReadMemory()
{ 
	static_assert(!std::_Is_pointer<_ret_t>::value, "The type _ret_t must not be a pointer!");
	_ret_t ret = NULL;
	/* DEBUG 
	cout << GetLastError() << endl;*/
	ReadProcessMemory(PROC_HANDLE, (void*)address, &ret, sizeof(ret), &bytesRead);
	/*cout << GetLastError() << endl;*/
	return ret;
	
 }
 
template <typename _ret_t> _ret_t ReadMemory64()
{
	static_assert(!std::_Is_pointer<_ret_t>::value, "The type _ret_t must not be a pointer!");
	_ret_t ret = NULL;
	NTSTATUS result = NtReadVirtualMemory64(PROC_HANDLE, (PVOID64)(address64), &ret, sizeof(ret), NULL);

	///* Debug # too lazy for breakpoints0101
	if (result != 0)
	{
		cerr << "ReadMemory Failed.\r\nAddress: " << address64 << "\r\nSize: " << sizeof(_ret_t) << "\r\nResult: " << result << endl;
		cerr << "NtReadVirtualMemory64 has failed" << endl;
		system("PAUSE");

	} //*/

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


int main()
{
	//InitReadMemory("ArenaNet_Dx_Window_Class", NULL, (UINT)0x1D205AB);
	Init32To64MemoryRead("ArenaNet_Dx_Window_Class", NULL, (UINT64)0x1D205AB);
	system("PAUSE");

}
