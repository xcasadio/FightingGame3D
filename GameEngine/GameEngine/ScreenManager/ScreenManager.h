#pragma once

#include "../Config/Export.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * The screen manager is a component which manages one or more GameScreen
	 * instances. It maintains a stack of screens, calls their Update and Draw
	 * methods at the appropriate times, and automatically routes input to the
	 * topmost active screen.
	 */
	class GAME_ENGINE_EXPORT ScreenManager
	{
	public:
		/**
		 *
		 */
		ScreenManager();

		/**
		 *
		 */
		~ScreenManager();

		/**
		 * If true, the manager prints out a list of all the screens
		 * each time it is updated. This can be useful for making sure
		 * everything is being added and removed at the right times.
		 */
		bool GetTraceEnabled() const;
		void SetTraceEnabled(const bool val_);

	private:
		//List<GameScreen> screens = new List<GameScreen>();
		//List<GameScreen> screensToUpdate = new List<GameScreen>();

		//InputState input = new InputState();

		//SpriteBatch spriteBatch;
		//SpriteFont font;
		//Texture2D blankTexture;

		bool m_IsInitialized;
		bool m_TraceEnabled;

	};

} //namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"
