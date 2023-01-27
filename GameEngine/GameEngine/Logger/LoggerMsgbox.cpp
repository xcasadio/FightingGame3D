#include "LoggerMsgBox.h"
#include <windows.h>

namespace GameEngine
{
	void cLoggerMsgBox::Write(const std::string& Message)
	{
		MessageBox(NULL, Message.c_str(), "GameEngine::Engine", MB_OK);
	}

} // namespace GameEngine
