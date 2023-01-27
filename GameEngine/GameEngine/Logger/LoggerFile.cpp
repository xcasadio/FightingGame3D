#include "LoggerFile.h"

namespace GameEngine
{
	cLoggerFile::cLoggerFile(const std::string& Filename) :
		m_File(Filename.c_str())
	{
		if (!m_File)
			throw std::exception("Impossible d'accéder en écriture");

		m_File << "  ===========================================" << std::endl;
		m_File << "   GameEngine ::Engine v0.1 - Event log - " << CurrentDate() << std::endl;
		m_File << "  ===========================================" << std::endl << std::endl;
	}

	cLoggerFile::~cLoggerFile()
	{
		m_File << std::endl;
		m_File << "  ===========================================" << std::endl;
		m_File << "   GameEngine::Engine closed at " << CurrentTime() << " - Goodbye ! " << std::endl;
		m_File << "  ===========================================" << std::endl;
	}

	void cLoggerFile::Write(const std::string& Message)
	{
		//Assert(m_File.is_open());

		if (!m_File.is_open())
			return;

		m_File << Message << std::flush;
	}

} // namespace GameEngine
