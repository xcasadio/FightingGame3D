#pragma once
#include "GameStateManager.h"

#include "Font.h"
#include "Graphic.h"
#include "Pad.h"
#include "Logger/Logger.h"

using namespace GameEngine;

class GameRun :
	public IGameState
{
public:
	GameRun();
	~GameRun();

	bool Init();
	bool Update( float elapsedTime_ );
	void Draw( float elapsedTime_ );

private:
	cGraphic *m_pGraphic;
	cPad *m_pPad;
	cFontDX m_Font;
};
