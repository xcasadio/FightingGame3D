#include "MainMenu.h"

#include "GameCore.h"
//#include "MeshX.h"
//#include "Character.h"

#include "CharacterSelection.h"
#include "GameRunStage.h"

#include "DebugEngine/DebugNew.h"

/**
 *
 */
MainMenu::MainMenu()
{
	m_pGraphic = NULL;
	m_pPad1 = NULL;
	m_ChoosenMenu = MENU_ARCADE;
}

/**
 *
 */
MainMenu::~MainMenu()
{

}

/**
 *
 */
bool MainMenu::Init()
{	
	m_RectArcade.left = 0;
	m_RectArcade.top = 0;
	m_RectArcade.right = 128;
	m_RectArcade.bottom = 32;

	m_RectVersus.left = 0;
	m_RectVersus.top = 47;
	m_RectVersus.right = 128;
	m_RectVersus.bottom = 80;

	m_RectQuit.left = 0;
	m_RectQuit.top = 94;
	m_RectQuit.right = 128;
	m_RectQuit.bottom = 128;

	m_pGraphic = &GameCore::Instance().GetGraphic();
	m_pPad1 = &GameCore::Instance().GetPad( 0 );

	if ( m_Bg.Load("main_menu.png") == false )
	{
		Window::Error( true, "impossible de charger 'main_menu.png'" );
		return false;
	}

	if ( m_Text.Load("main_menu_text.png") == false )
	{
		Window::Error( true, "impossible de charger 'main_menu_text.png'" );
		return false;
	}

	return true;
}

/**
 *
 */
bool MainMenu::Update( float elapsedTime_ )
{
	GameCore::Instance().ReadAllInputDevice();

	if ( m_pPad1->ButtonPressed( PAD_DOWN ) )
	{
		if ( m_ChoosenMenu == MENU_QUIT )
		{
			m_ChoosenMenu = MENU_ARCADE;
		}
		else
		{
			m_ChoosenMenu++;
		}
	}

	if ( m_pPad1->ButtonPressed( PAD_UP ) )
	{
		if ( m_ChoosenMenu == MENU_ARCADE )
		{
			m_ChoosenMenu = MENU_QUIT;
		}
		else
		{
			m_ChoosenMenu--;
		}
	}

	/*if ( m_pPad1->ButtonPressed( PAD_CANCEL ) )
	{
	}*/

	if ( m_pPad1->ButtonPressed( PAD_OK ) )
	{
		switch ( m_ChoosenMenu )
		{
		case MENU_ARCADE:
			GameCore::Instance().GetGameStateManager()->Push( new CharacterSelection/*GameRunStage*/ );
			break;

		case MENU_VERSUS:
			GameCore::Instance().GetGameStateManager()->Push( new GameRunStage );
			break;

		case MENU_QUIT:
			SetDestroy( true );
			return false;
			break;
		}
	}
	
	return true;
}

/**
 *
 */
void MainMenu::Draw( float elapsedTime_ )
{
	DWORD arcadeColor = 0xffffffff, versusColor = 0xffffffff, quitColor = 0xffffffff;

	m_pGraphic->Clear( 0xffffffff );

	switch ( m_ChoosenMenu )
	{
	case MENU_ARCADE:
		arcadeColor = 0xffff0000;
		break;

	case MENU_VERSUS:
		versusColor = 0xffff0000;
		break;

	case MENU_QUIT:
		quitColor = 0xffff0000;
		break;
	}

	m_pGraphic->BeginSprite(D3DXSPRITE_ALPHABLEND);

	int center = (int) ( 512.0f - (float) (m_RectArcade.right - m_RectArcade.left) / 2.0f );

	m_Bg.Blit( 0, 0 );
	m_Text.Blit( center, 500, m_RectArcade.left, m_RectArcade.top, m_RectArcade.right - m_RectArcade.left, m_RectArcade.bottom - m_RectArcade.top, 1.0f, 1.0f, 0.0f, arcadeColor );
	m_Text.Blit( center, 550, m_RectVersus.left, m_RectVersus.top, m_RectVersus.right - m_RectVersus.left, m_RectVersus.bottom - m_RectVersus.top, 1.0f, 1.0f, 0.0f, versusColor );
	m_Text.Blit( center, 600, m_RectQuit.left, m_RectQuit.top, m_RectQuit.right - m_RectQuit.left, m_RectQuit.bottom - m_RectQuit.top, 1.0f, 1.0f, 0.0f, quitColor );
	
	m_pGraphic->EndSprite();
}
