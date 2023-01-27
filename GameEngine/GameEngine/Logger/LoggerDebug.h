#ifndef LOGGERDEBUG_H
#define LOGGERDEBUG_H

#include "Logger.h"

namespace GameEngine
{
	class GAME_ENGINE_EXPORT cLoggerDebug : public ILogger
	{
	private:
		virtual void Write(const std::string& Message);
	};

} // namespace GameEngine

#endif // LOGGERDEBUG_H
