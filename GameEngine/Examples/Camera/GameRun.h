#pragma once

#include "GameStateManager.h"

#include "Graphic.h"
#include "Pad.h"
#include "GameStateManager.h"
#include "Camera.h"
#include "Font.h"
#include "SkyBox.h"
#include "MeshX.h"

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
	cPad *m_pPad;
	cGraphic *m_pGraphic;

	cCameraFPS m_CameraFPS;
	cCameraFree m_CameraFree;
	cCameraModelViewer m_CameraModelViewer;

	cMeshX m_MeshX;
	cFontDX m_Font;
	cLight m_Light;

	int m_CameraType;
	float m_X, m_Z;
};
