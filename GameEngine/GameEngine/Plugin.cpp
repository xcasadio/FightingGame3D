
#include "Plugin.h"
#include "DebugEngine/Exception.h"

#include <string>

namespace GameEngine
{
	Plugin::Plugin(const std::string& Filename)
	{
		m_Library = LoadLibrary(Filename.c_str());
		if (!m_Library)
		{
			throw Exception(std::string("Can't load the plugin : ").append(Filename));
		}

		auto LoadFunc = reinterpret_cast<PtrFunc>(GetProcAddress(m_Library, "LoadPlugin"));
		if (!LoadFunc)
		{
			throw Exception(std::string("Can't find the function 'LoadPlugin' in the plugin : ").append(Filename));
		}

		LoadFunc();
	}

	Plugin::~Plugin()
	{
		auto UnloadFunc = reinterpret_cast<PtrFunc>(GetProcAddress(m_Library, "UnloadPlugin"));
		if (!UnloadFunc)
		{
			throw Exception("Can't find the function 'UnloadPlugin' in the plugin");
		}
		UnloadFunc();

		if (m_Library)
		{
			FreeLibrary(m_Library);
		}
	}

} // namespace GameEngine
