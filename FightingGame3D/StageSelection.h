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
class FIGHTING_GAME_EXPORT StageSelection :
	public IGameState
{
public:
	/**
	 *
	 */
	StageSelection();

	/**
	 *
	 */
	~StageSelection() override;

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
	Texture m_Tex;
};

#include "DebugEngine/DebugNewOff.h"
