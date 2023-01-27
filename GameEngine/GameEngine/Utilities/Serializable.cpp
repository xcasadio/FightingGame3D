#include "Serializable.h"

#include <iostream>
#include <fstream>

#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	bool ISerializable::LoadSerialized(const char* fileName_)
	{
		std::fstream fs(fileName_, std::ios::binary | std::ios::in);

		if (fs.is_open() == false)
		{
			return false;
		}

		int i = Read(fs);

		if (i <= 0)
		{
			return false;
		}

		fs.close();
		return true;
	}

	bool ISerializable::SaveSerialized(const char* fileName_)
	{
		std::fstream fs(fileName_, std::ios::binary | std::ios::out);

		*this >> fs;

		fs.close();

		return true;
	}

} // namespace GameEngine
