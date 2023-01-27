#ifndef LOGGERFILE_H
#define LOGGERFILE_H

#include "Config/Export.h"
#include "Logger.h"

#include <fstream>

namespace GameEngine
{
    class GAME_ENGINE_EXPORT cLoggerFile : public ILogger
    {
    public :
        cLoggerFile(const std::string& Filename = "Output.log");
        ~cLoggerFile();

    private :
        virtual void Write(const std::string& Message);
        std::ofstream m_File;
    };

} // namespace GameEngine

#endif // LOGGERFILE_H
