#ifndef LOGGER_H
#define LOGGER_H

#include "Config/Export.h"
#include <sstream>
#include <string>

namespace GameEngine
{
	class GAME_ENGINE_EXPORT ILogger
	{
	public:
		virtual ~ILogger();

		static void SetLogger(ILogger* Logger);
		static void Destroy();
		static void Log(const char* Format, ...);
		static ILogger& Log();

		template <class T> ILogger& operator <<(const T& ToLog);

	protected:
		std::string CurrentDate() const;
		std::string CurrentTime() const;

	private:
		virtual void Write(const std::string& Message) = 0;

		static ILogger* s_Instance;
	};

	template <class T> ILogger& ILogger::operator <<(const T& ToLog)
	{
		std::ostringstream Stream;
		Stream << ToLog;
		Write(Stream.str());

		return Log();
	}

} // namespace GameEngine


#endif // LOGGER_H
