#include "Logger.h"

#include <ctime>
#include <cstdarg>

namespace GameEngine
{
	ILogger* ILogger::s_Instance = NULL;

	ILogger::~ILogger()
	{

	}

	void ILogger::SetLogger(ILogger* Logger)
	{
		Destroy();
		s_Instance = Logger;
	}

	void ILogger::Destroy()
	{
		delete s_Instance;
		s_Instance = NULL;
	}

	void ILogger::Log(const char* Format, ...)
	{
		//std::assert(s_Instance != NULL);

		char sBuffer[512];
		va_list Params;
		va_start(Params, Format);
		vsprintf_s(sBuffer, 512, Format, Params);
		va_end(Params);

		s_Instance->Write(sBuffer);
	}

	ILogger& ILogger::Log()
	{
		return *s_Instance;
	}

	std::string ILogger::CurrentDate() const
	{
		char sTime[24];
		time_t CurrentTime = time(NULL);
		tm m;
		localtime_s(&m, &CurrentTime);
		strftime(sTime, sizeof(sTime), "%d/%m/%Y", &m);
		//strftime(sTime, sizeof(sTime), "%d/%m/%Y", localtime(&CurrentTime));

		return sTime;
	}

	std::string ILogger::CurrentTime() const
	{
		char sTime[24];
		time_t CurrentTime = time(NULL);
		tm m;
		localtime_s(&m, &CurrentTime);
		strftime(sTime, sizeof(sTime), "%H:%M:%S", &m);

		return sTime;
	}

} // namespace GameEngine
