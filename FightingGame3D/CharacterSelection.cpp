#include "CharacterSelection.h"

#include "StageSelection.h"
#include "GameRunStage.h"
#include "GameCore.h"
#include "ResourceManager.h"
#include "GlobalVariable.h"

#include "DebugEngine/DebugNew.h"

/**
 *
 */
CharacterSelection::CharacterSelection()
{
	m_pGraphic = NULL;
	m_pPad1 = NULL;
}

/**
 *
 */
CharacterSelection::~CharacterSelection()
{
}

/**
 *
 */
bool CharacterSelection::Init()
{	
	m_pGraphic = &GameCore::Instance().GetGraphic();
	m_pPad1 = &GameCore::Instance().GetPad( 0 );

	if ( m_Bg.Load( "character selection background.png" ) == false )
	{
		Window::Error( true, "impossible de charger 'character selection background.png'" );
		return false;
	}
	
	if ( m_Icone[0].Load( "icon player.png" ) == false )
	{
		Window::Error( true, "impossible de charger 'icon player.png'" );
		return false;
	}

	for ( int i = 1; i < 12; i++ )
	{
		if ( m_Icone[i].Load( "icon unknown.png" ) == false )
		{
			Window::Error( true, "impossible de charger 'icon unknown.png'" );
			return false;
		}
	}

	if ( m_Select[0].Load( "cadre selection P1.png" ) == false )
	{
		Window::Error( true, "impossible de charger 'cadre selection P1.png'" );
		return false;
	}

	if ( m_Select[1].Load( "cadre selection P2.png" ) == false )
	{
		return false;
	}

	if ( m_Select[2].Load( "cadre selection CPU.png" ) == false )
	{
		Window::Error( true, "impossible de charger 'cadre selection P2.png'" );
		return false;
	}

	m_SelectCoord[0].x = 0;
	m_SelectCoord[0].y = 0;

	m_SelectCoord[1].x = 5;
	m_SelectCoord[1].y = 1;

	m_bActiveCharac[0] = true;

	for ( int i = 1; i < 12; i++ )
	{
		m_bActiveCharac[i] = false;
	}

	m_ChoiceStep = 0;

	return true;
}

/**
 *
 */
bool CharacterSelection::Update( float elapsedTime_ )
{
	GameCore::Instance().ReadAllInputDevice();

	if ( m_pPad1->ButtonPressed( PAD_CANCEL ) )
	{
		SetDestroy( true );
		return false;
	}

	if ( m_pPad1->ButtonPressed( PAD_OK ) )
	{
		if ( m_ChoiceStep == 0 )
		{
			if ( m_bActiveCharac[ m_SelectCoord[0].x + m_SelectCoord[0].y * 6 ] )
			{
				GlobalVariable *pP1Choosen = ResourceManager::Instance().Get<GlobalVariable> ( GLOBAL_VAR_P1_CHOOSEN );
				pP1Choosen->SetVariable( m_SelectCoord[0].x + m_SelectCoord[0].y * 6 );	
				m_ChoiceStep = 1;
			}
		}
		else if ( m_ChoiceStep == 1 )
		{
			if ( m_bActiveCharac[ m_SelectCoord[1].x + m_SelectCoord[1].y * 6 ] )
			{
				GlobalVariable *pP2Choosen = ResourceManager::Instance().Get<GlobalVariable> ( GLOBAL_VAR_P2_CHOOSEN );
				pP2Choosen->SetVariable( m_SelectCoord[1].x + m_SelectCoord[1].y * 6 );

				SetDestroy( true );
				GameCore::Instance().GetGameStateManager()->Push( new /*StageSelection*/GameRunStage );
			}
		}
	}

	if ( m_pPad1->ButtonPressed( PAD_UP ) )
	{
		m_SelectCoord[m_ChoiceStep].y = 1 - m_SelectCoord[m_ChoiceStep].y;
	}

	if ( m_pPad1->ButtonPressed( PAD_DOWN ) )
	{
		m_SelectCoord[m_ChoiceStep].y = 1 - m_SelectCoord[m_ChoiceStep].y;
	}

	if ( m_pPad1->ButtonPressed( PAD_RIGHT ) )
	{
		if ( m_SelectCoord[m_ChoiceStep].x == 5 )
		{
			m_SelectCoord[m_ChoiceStep].x = 0;
		}
		else
		{
			m_SelectCoord[m_ChoiceStep].x++;
		}
	}

	if ( m_pPad1->ButtonPressed( PAD_LEFT ) )
	{
		if ( m_SelectCoord[m_ChoiceStep].x == 0 )
		{
			m_SelectCoord[m_ChoiceStep].x = 5;
		}
		else
		{
			m_SelectCoord[m_ChoiceStep].x--;
		}
	}
	
	return true;
}

/**
 *
 */
void CharacterSelection::Draw( float elapsedTime_ )
{
	m_pGraphic->Clear(0);

	m_pGraphic->BeginSprite(D3DXSPRITE_ALPHABLEND);

	m_Bg.Blit( 0,0 );
	
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 6; j++ )
		{
			m_Icone[i * 6 + j].Blit( 129 + 128 * j, 480 + i * 128 );
		}
	}

	m_Select[0].Blit( 129 + m_SelectCoord[0].x * 128, 480 + m_SelectCoord[0].y * 128 );
	m_Select[1].Blit( 129 + m_SelectCoord[1].x * 128, 480 + m_SelectCoord[1].y * 128 );
	
	m_pGraphic->EndSprite();
}

