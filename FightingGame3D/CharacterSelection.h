#pragma once

#include "GameStateManager.h"
#include "Graphic.h"
#include "Pad.h"
#include "Texture.h"
#include "FG_Export.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

/**
 *
 */
class FIGHTING_GAME_EXPORT CharacterSelection :
	public IGameState
{
public:
	/**
	 *
	 */
	CharacterSelection();

	/**
	 *
	 */
	~CharacterSelection() override;

	/**
	 *
	 */
	bool Init() override;

	/**
	 *
	 */
	bool Update(float elapsedTime_) override;

	/**
	 *
	 */
	void Draw(float elapsedTime_) override;

private:
	Graphic* m_pGraphic;
	Pad* m_pPad1;
	Texture m_Bg, m_Icone[12], m_Select[3];
	POINT m_SelectCoord[2];
	bool m_bActiveCharac[12];
	int m_ChoiceStep;
};

#include "DebugEngine/DebugNewOff.h"
