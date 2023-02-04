#include "StringUtilities.h"

namespace GameEngine
{
	std::string cStringUtilities::GetFileName(std::string str_)
	{
		size_t found = str_.find_last_of("/\\");
		return str_.substr(found + 1);
	}

	std::string cStringUtilities::GetDirectory(std::string str_)
	{
		size_t found = str_.find_last_of("/\\");

		return str_.substr(0, found);
	}
} // GameEngine
