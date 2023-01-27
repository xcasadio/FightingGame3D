#pragma once

#include "GameStateManager.h"
#include "Graphic.h"
#include "Pad.h"
#include "Texture.h"
#include "Config/Export.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

/**
 *
 */
class GAME_ENGINE_EXPORT StageSelection :
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
	~StageSelection();

	/**
	 *
	 */
	bool Init();

	/**
	 *
	 */
	bool Update( float elapsedTime_ );

	/**
	 *
	 */
	void Draw( float elapsedTime_ );

private:
	Graphic *m_pGraphic;
	Pad *m_pPad1;
	Texture m_Tex;
};

#include "DebugEngine/DebugNewOff.h"
