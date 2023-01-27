#pragma once

#include "GameStateManager.h"

#include "Graphic.h"
#include "Pad.h"
#include "GameStateManager.h"
#include "Camera.h"
#include "Font.h"
#include "SkyBox.h"
#include "MeshX.h"
#include "WorldPosition.h"

using namespace GameEngine;

class GameRun :
	public IGameState
{
public:
	GameRun(void);
	~GameRun(void);

	bool Init();
	bool Update( float elapsedTime_ );
	void Draw( float elapsedTime_ );

private:
	cGraphic *m_pGraphic;
	cPad *m_pPad1;
	cLight m_Light, m_Light2;
	cFontDX m_Font;
	cMeshX m_MeshX;
	cBoundingBox m_Box1, m_Box2;
	cCameraFree m_CameraFree;
	D3DVIEWPORT9 m_ViewPort1, m_ViewPort2, m_ViewPort3, m_ViewPort4;

	bool m_Collide;
	int m_CubeSelected;

	cWorldPosition m_Pos1, m_Pos2;
	D3DMATERIAL9 m_Mat1, m_Mat2;
};
