#include "MemWolf.h"


bool MemWolfObject::IsLootAvailable()
{
	if (IsWindowVisible(WINDOW_HANDLE))
	{
		//64 means loot is available to us while 0 is none
		if ((byte)ReadMemory<byte>() == 64)
		{
			return true;
		}
	}
	return false;
}

bool MemWolfObject::IsLootAvailable64()
{
	if (IsWindowVisible(WINDOW_HANDLE))
	{
		//64 means loot is available to us while 0 is none
		if ((byte)ReadMemory64<byte>() == 64)
		{
			return true;
		}
	}
	return false;

}

MemWolfObject::MemWolfObject()
{

}