#include "StageSelection.h"

#include "GameRunStage.h"
#include "GameCore.h"

#include "DebugEngine/DebugNew.h"

StageSelection::StageSelection()
{
	m_pGraphic = NULL;
	m_pPad1 = NULL;
}

StageSelection::~StageSelection()
{
}

bool StageSelection::Init()
{	
	m_pGraphic = &GameCore::Instance().GetGraphic();
	m_pPad1 = &GameCore::Instance().GetPad( 0 );

	/*if ( m_Tex.Load("main_menu.png") == false )
	{
		return false;
	}*/

	return true;
}

bool StageSelection::Update( float elapsedTime_ )
{
	GameCore::Instance().ReadAllInputDevice();

	if ( m_pPad1->ButtonPressed( PAD_BUTTON_8 ) )
	{
		SetDestroy( true );
		return false;
	}

	if ( m_pPad1->ButtonPressed( PAD_OK ) )
	{
		GameCore::Instance().GetGameStateManager()->Push( new GameRunStage );
	}
	
	return true;
}

void StageSelection::Draw( float elapsedTime_ )
{
	/*
	m_pGraphic->BeginSprite(D3DXSPRITE_ALPHABLEND);
	m_Tex.Blit(0,0);
	m_pGraphic->EndSprite();
	*/
}


