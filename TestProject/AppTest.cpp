#include "MemoryReader.h"
#include <iostream>

int main()
{
	MemoryReaderObject mrObject;
	mrObject.InitMemoryRead64("ArenaNet_Dx_Window_Class", NULL, 0x1D2CB6B);
	std::cout << (short)mrObject.ReadMemory64<BYTE>() << std::endl;
	system("PAUSE");
}