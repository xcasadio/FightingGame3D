#pragma once

#include "Config/Export.h"
#include "Logger.h"

#include <fstream>

namespace GameEngine
{
	class GAME_ENGINE_EXPORT LoggerFile : public ILogger
	{
	public:
		LoggerFile(const std::string& Filename = "Output.log");
		~LoggerFile() override;

	private:
		virtual void Write(const std::string& Message);
		std::ofstream m_File;
	};

} // namespace GameEngine
