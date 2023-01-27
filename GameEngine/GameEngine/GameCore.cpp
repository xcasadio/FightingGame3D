#include <stdio.h>

#include "GameCore.h"
#include "Input.h"

#ifdef _DEBUG
#	include "Logger/Logger.h"
#	include "Logger/LoggerFile.h"
#endif

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	//long GameCore::m_ClearColor(0);

	SINGLETON_IMPL(GameCore)
	
	/**
	 *
	 */
	GameCore::GameCore()
	{
		//m_GameStateManager.SetGameCore(this);
		m_bWindowClosing = false;
	}

	/**
	 *
	 */
	GameCore::~GameCore()
	{
		//m_Graphic.Shutdown();
		//m_Pad.Shutdown();

		Input::Destroy();

#ifdef _DEBUG
		ILogger::Destroy();
#endif
	}

	/**
	 *
	 */
	cGameStateManager *GameCore::GetGameStateManager()
	{ 
		return &m_GameStateManager;
	}

	/**
	 *
	 */
	bool GameCore::Init(int nCmd, LPSTR szCmdLine, Window &win, s_window_param param, HWND parent, bool bPerspective )
	{
		//TODO : interprété la ligne de commande

		//if ( !m_Window.Create(param.caption, parent) )
			//return false;
		m_Window = win;
			
		if ( !m_Graphic.Init() )
			return false;

		if ( !m_Graphic.SetMode(m_Window.GethWnd(), param.windowed, true, param.width, param.height, param.depth, bPerspective) )
			return false;

		//if ( !m_Pad.Init(m_Window.GethWnd(), m_Window.GethInst(), param.windowed) )
			//return false;
		
		return true;
	}

	/**
	 *
	 *
	bool GameCore::InitPad(int pad, HWND hWnd)
	{
		for ( int i = 0; i < MAX_PAD; i++ )
		{
			if ( !m_Pad[i].Init( (hWnd == NULL ? m_Window.GethWnd() : hWnd), \
							m_Window.GethInst(), \
							m_Graphic.GetWindowed()) )
				return false;
		}
		
		return true;
	}*/

	/**
	 *
	 */
	void GameCore::Run()
	{
		ShowWindow( m_Window.GethWnd(), SW_SHOWDEFAULT );
		UpdateWindow( m_Window.GethWnd() );

		m_Graphic.StartTime();

		while( m_Graphic.BeginFrame() && m_bWindowClosing == false )
		{
			if ( m_GameStateManager.Run() == false )
			{
				break;
			}

			m_Graphic.EndFrame();
		}

		//cInput::Destroy();
	}

	/**
	 *
	 */
	bool GameCore::AddGameState(IGameState *gameState)
	{
		if ( gameState == NULL)
			return false;

		m_GameStateManager.Push(gameState);
		return true;
	}

	/**
	 *
	 */
	Window &GameCore::GetWindow()
	{
		return m_Window;
	}

	/**
	 *
	 */
	Graphic &GameCore::GetGraphic()
	{
		return m_Graphic;
	}
	
	/**
	 *
	 */
	Pad &GameCore::GetPad( unsigned char index_ )
	{
		return m_Pad[index_];
	}

	/**
	 *
	 */
	void GameCore::ReadAllInputDevice()
	{
		for ( int i = 0; i < MAX_INPUT_DEVICE; i++ )
		{
			if ( Input::g_pInputDevice[i] != NULL )
			{
				if ( Input::g_pInputDevice[i]->Acquire() )
				{
					if ( Input::g_pInputDevice[i]->Read() ==false )
					{
						//Maybe another app acquire the input
					}
				}
				else
				{
					//TODO : erreur
					exit(-1);
				}
			}
		}

		for ( int i = 0; i < MAX_PAD; i++ )
		{
			m_Pad[i].Read();
		}
	}

	/**
	 *
	 */
	bool GameCore::CreateInputDevice( HWND hWnd, HINSTANCE hInst, bool windowed)
	{
		if ( !Input::Instance().Init(hWnd, hInst) )
		{
			Window::Error( false, "Echec de la création de l'input");
			return false;
		}

		Input::g_pInputDevice[INPUT_DEVICE_MOUSE] = new InputDevice;

		if ( !Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->Create(InputDevice::INPUT_DEVICE_TYPE_MOUSE, windowed) )
		{
			Window::Error( false, "Echec de la création de la souris");
			return false;
		}

		Input::g_pInputDevice[INPUT_DEVICE_KEYBOARD] = new InputDevice;

		if ( !Input::g_pInputDevice[INPUT_DEVICE_KEYBOARD]->Create(InputDevice::INPUT_DEVICE_TYPE_KEYBOARD, windowed) )
		{ 
			Window::Error( false, "Echec de la création du clavier");
			return false;
		}

		for ( int i = INPUT_DEVICE_JOYSTICK1; i <= INPUT_DEVICE_JOYSTICK4; i++ )
		{
			Input::g_pInputDevice[i] = NULL;
			Input::g_pInputDevice[i] = new InputDevice();

			if ( !Input::g_pInputDevice[i]->Create( InputDevice::INPUT_DEVICE_TYPE_JOYSTICK, windowed) )
			{
				delete Input::g_pInputDevice[i];
				Input::g_pInputDevice[i] = NULL;

#ifdef _DEBUG
				ILogger::Log() << "joystick absent :" << i << "\n";
#endif
			}
		}
		
		return true;
	}

	/**
	 *
	 */
	void GameCore::CleanUp()
	{
		m_bWindowClosing = true;
	}

	/**
	 *
	 */
	bool GameCore::WindowIsClosing() const
	{
		return m_bWindowClosing;
	}

	/**
	 *
	 *
	long GameCore::GetClearColor()
	{
		return m_ClearColor;
	}

	**
	 *
	 *
	void GameCore::SetClearColor( long color_ )
	{
		m_ClearColor = color_;
	}*/

} // namespace GameEgine
