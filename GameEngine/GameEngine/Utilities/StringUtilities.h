#pragma once

#include "../Config/Export.h"

#include <string>

namespace GameEngine
{
	class GAME_ENGINE_EXPORT cStringUtilities
	{
	public:
		static std::string GetFileName(std::string str_);
		static std::string GetDirectory(std::string str_);
	};

} // fin namespace GameEngine
