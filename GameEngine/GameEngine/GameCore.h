#ifndef _GAMEENGINE_H_
#define _GAMEENGINE_H_

#include "Window.h"
#include "GameStateManager.h"
#include "Pad.h"
#include "Graphic.h"

#include "Config/Export.h"
#include "Utilities/Singleton.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	#define MAX_PAD 4

	class GAME_ENGINE_EXPORT GameCore 
		: public ISingleton<GameCore>
	{
		MAKE_SINGLETON(GameCore);

	public:

		typedef struct s_window_param
		{
			unsigned int width, height, depth;
			const char *title;
			bool windowed;
		} s_window_param;	

		/**
		 *
		 */
		~GameCore();

		/** 
		 *
		 */
		bool Init( int nCmd, LPSTR szCmdLine, Window &win, s_window_param param, HWND parent = NULL, bool bPerspective = true );

		/** 
		 *
		 */
		//bool InitPad(int pad = -1, HWND hWnd = NULL);

		/** 
		 *
		 */
		void Run();

		/** 
		 *
		 */
		bool AddGameState(IGameState *gameState);

		/** 
		 *
		 */
		cGameStateManager *GetGameStateManager();

		/** 
		 *
		 */
		Window &GetWindow();

		/** 
		 *
		 */
		Graphic &GetGraphic();

		/** 
		 *
		 */
		Pad &GetPad( unsigned char index_ );

		/**
		 *
		 */
		bool CreateInputDevice( HWND hWnd, HINSTANCE hInst, bool windowed);

		/**
		 *
		 */
		void ReadAllInputDevice();
		
		/**
		 *
		 */
		void CleanUp();

		/**
		 *
		 */
		bool WindowIsClosing() const;

		/**
		 *
		 *
		static long GetClearColor();

		**
		 *
		 *
		static void SetClearColor( long color_ );*/

	protected:
		Window m_Window;
		cGameStateManager m_GameStateManager;
		Graphic m_Graphic;
		Pad m_Pad[MAX_PAD];
		
	private:
		GameCore();

		bool m_bWindowClosing;

		static long m_ClearColor;
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _GAMEENGINE_H_