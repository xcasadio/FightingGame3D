#pragma once
#include "GameStateManager.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "GameCore.h"
#include "Graphic.h"
#include "Camera.h"
#include "Font.h"
#include "Pad.h"
#include "Texture.h"
#include "MeshX.h"
#include "Character.h"
#include "WorldPosition.h"
#include "Window.h"
#include "MediaPathManager.h"
#include "FG_Export.h"

#include "Command.h"
#include "Stage.h"

#include <string>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

class FIGHTING_GAME_EXPORT GameRunStage :
	public IGameState
{
public:
	GameRunStage();
	//GameRunTest(GameStateManager *gameStateManager);
	~GameRunStage() override;

	bool Init() override;
	bool Update(float elapsedTime_) override;
	void Draw(float elapsedTime_) override;

private:
	World m_Stage;
	Graphic* m_pGraphic;
	Pad* m_pPad1;
	Light m_Light, m_Light2;
	FontDX m_Font;

	//cBufferButton m_BufferButtonP1, m_BufferButtonP2;
};

#include "DebugEngine/DebugNewOff.h"
