#include "LoggerDebug.h"

#include <windows.h>

namespace GameEngine
{
	void cLoggerDebug::Write(const std::string& Message)
	{
		OutputDebugString((Message + '\n').c_str());
	}

} // namespace GameEngine
