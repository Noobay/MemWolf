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

#include "MemWolf.h"


bool MemWolfObject::IsLootAvailable()
{
	
	//64 means loot is available to us while 0 is none
	if ((byte)ReadMemory<byte>() == 64)
	{
		return true;
	}
	
	return false;
}

bool MemWolfObject::IsLootAvailable64()
{

	//64 means loot is available to us while 0 is none
	if ((byte)ReadMemory64<byte>() == 64)
	{
		return true;
	}

	return false;
}

MemWolfObject::MemWolfObject()
{

}