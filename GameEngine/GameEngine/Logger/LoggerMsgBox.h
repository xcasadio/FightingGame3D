#ifndef LOGGERMSGBOX_H
#define LOGGERMSGBOX_H

#include "Logger.h"

namespace GameEngine
{
	class cLoggerMsgBox : public ILogger
	{
	private:
		virtual void Write(const std::string& Message);
	};

} // namespace GameEngine

#endif // LOGGERMSGBOX_H
