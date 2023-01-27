#ifndef _MEDIAPATHMANAGER_H_
#define _MEDIAPATHMANAGER_H_

#include "Utilities/Singleton.h"
#include "Config/Export.h"

#include <string>
#include <set>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	class GAME_ENGINE_EXPORT MediaPathManager :
		public ISingleton<MediaPathManager>
	{
		MAKE_SINGLETON(MediaPathManager);
	public:
		//MediaPathManager(void);
		//~MediaPathManager(void);

		/**
		 *
		 */
		void AddPath( const char *fileName_ );

		/**
		 *
		 */
		std::string FindMedia( const char *fileName_ );

	private:
		std::set<std::string> m_Paths;
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif //_MEDIAPATHMANAGER_H_
