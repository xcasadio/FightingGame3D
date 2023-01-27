#pragma once

#include "GameStateManager.h"

#include "GameCore.h"
#include "Graphic.h"
#include "Pad.h"
#include "GameStateManager.h"
#include "Camera.h"
#include "Font.h"
#include "SkyBox.h"
#include "ParticleEngine/ParticleSystem.h"
#include "MeshX.h"
#include "MeshXAnimated.h"

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
	cPad *m_pPad1;
	cLight m_Light, m_Light2, m_Light3;
	cFontDX m_Font;
	cMeshXAnimated m_Mesh;
	cCameraFree m_CameraFree;
	D3DXMATRIX m_MatWorld;

	int m_CurrentAnimation;
	bool m_Pause;
};
