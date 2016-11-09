#include <Windows.h>
#include <Psapi.h>

HANDLE PROC_HANDLE;
HWND   WINDOW_HANDLE;

UINT_PTR address;
SIZE_T bytesRead;

void InitReadMemory(char windowClass[], char caption[], int long addressOffset)
{
	DWORD PROC_ID;
	DWORD cbNeeded;
	HMODULE mainModule;
	HRESULT result;

	//Get the application handle
	WINDOW_HANDLE = FindWindow(windowClass, caption);

	result = GetWindowThreadProcessId(WINDOW_HANDLE, &PROC_ID);

	PROC_HANDLE = OpenProcess(PROCESS_ALL_ACCESS, false, PROC_ID);

	/*
  	 *	Get the Main Module-
	 *	first entry in the returned HMODULE array from
	 *	EnumProcessModules
	 */
	EnumProcessModules(PROC_HANDLE, &mainModule, sizeof(HMODULE), &cbNeeded);


	//offset the requested memory address from the application's base address
	address = ((UINT_PTR)mainModule + addressOffset);

}

template<typename _ret_t> _ret_t ReadMemory()
{
	_ret_t ret;
	ReadProcessMemory(PROC_HANDLE, (void*)(address), &ret, sizeof(_ret_t), &bytesRead);

	return ret;
}

bool isLootAvailable()
{
	if (IsWindowVisible(WINDOW_HANDLE))
	{
		//64 means loot is available to us while 0 is none
		if ((DWORD)ReadMemory<DWORD>() == 64)
		{
			return true;
		}
	}
	return false;
}

void FinishReadingMemory()
{
	CloseHandle(PROC_HANDLE);
}

