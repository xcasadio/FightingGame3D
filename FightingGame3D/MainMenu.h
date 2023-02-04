#pragma once
#include "GameStateManager.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Graphic.h"
#include "Pad.h"
#include "Texture.h"
#include "FG_Export.h"

using namespace GameEngine;

#include "DebugEngine/DebugNew.h"

typedef enum eMenuID
{
	MENU_ARCADE,
	MENU_VERSUS,
	MENU_QUIT,

	MAX_MENU_ID
} eMenuID;

/**
 *
 */
class FIGHTING_GAME_EXPORT MainMenu :
	public IGameState
{
public:
	MainMenu();
	~MainMenu() override;

	bool Init() override;
	bool Update(float elapsedTime_) override;
	void Draw(float elapsedTime_) override;

private:
	Graphic* m_pGraphic;
	Pad* m_pPad1;

	Texture m_Text, m_Bg;
	int m_ChoosenMenu;

	//pour les textures
	RECT m_RectArcade, m_RectVersus, m_RectQuit;
};

#include "DebugEngine/DebugNewOff.h"
