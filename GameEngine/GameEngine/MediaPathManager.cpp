#include "MediaPathManager.h"

#include <windows.h>

#include <iostream>
#include <fstream>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	SINGLETON_IMPL(MediaPathManager)

		/**
		 *
		 */
		void MediaPathManager::AddPath(const char* fileName_)
	{
		std::string str = fileName_;
		if (*str.rbegin() != '/' && *str.rbegin() != '\\')
		{
			str.append("/");
		}

		m_Paths.insert(str);
	}

	/**
	 *
	 */
	std::string MediaPathManager::FindMedia(const char* fileName_)
	{
		std::string str;
		std::fstream file;
		std::set<std::string>::iterator it;

		for (it = m_Paths.begin(); it != m_Paths.end(); ++it)
		{
			str.append(*it);
			str.append(fileName_);

			file.open(str.c_str());

			if (file.is_open())
			{
				file.close();
				return str;
			}

			str.clear();
		}

		return str;
	}

}
