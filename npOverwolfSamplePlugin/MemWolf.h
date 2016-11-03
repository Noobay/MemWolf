#ifndef MEM_WOLF_H
#define MEM_WOLF_H

#include "MemoryReader.h"

class MemWolfObject : public MemoryReaderObject
{
public:
	bool IsLootAvailable();
	bool IsLootAvailable64();

	MemWolfObject();
};
#endif