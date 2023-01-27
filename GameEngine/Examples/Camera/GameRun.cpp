#include "GameRun.h"
#include "GameCore.h"


#include <d3dx9math.h>

using namespace GameEngine;

/**
 *
 */
GameRun::GameRun()
{
}

/**
 *
 */
GameRun::~GameRun()
{
}

/**
 *
 */
bool GameRun::Init()
{
	if ( m_MeshX.Load("cube1.x") == false )
	{
		MessageBox(NULL, "Erreur cube1.x", "Erreur", MB_OK);
		exit(-1);
	}

	m_pPad = &cGameCore::Instance().GetPad(0);
	m_pGraphic = &cGameCore::Instance().GetGraphic();

	m_CameraFree.Point( 0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 0.0f);

	m_CameraFPS.Point( 0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 0.0f);
	m_CameraFPS.SetSpeed(1.0f);
	m_CameraFPS.SetSensivity(1.0f);

	m_CameraModelViewer.SetRadius(5.0f);

	m_Font.Create("Arial");

	m_Light.SetAmbientColor(255, 255, 255);
	m_Light.SetDiffuseColor(255, 255, 255);
	m_Light.SetSpecularColor(255, 255, 255);
	m_Light.SetFalloff(1.0f);
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetRange(10000.0f);
	m_Light.SetPhi(1.0f);
	m_Light.Point(0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 0.0f);

	m_pGraphic->EnableZBuffer(true);
	m_pGraphic->EnableLighting(true);
	m_pGraphic->SetAmbientLight(100, 100, 100);

	m_pGraphic->SetLight(0, &m_Light);
	m_pGraphic->EnableLight(0);
	
	m_CameraType = 1;

	return true;
}

/**
 *
 */
void GameRun::Draw( float elapsedTime_ )
{
	static char buf[64];

	sprintf_s(buf, 64, "FPS : %d", cGameCore::Instance().GetGraphic().GetFPS());
	m_Font.Print( buf, 0, 0);

	switch ( m_CameraType )
	{
	case 0:
		m_pGraphic->SetCamera(&m_CameraFPS);

		sprintf_s(buf, 64, "camera : FPS" );
		m_Font.Print( buf, 0, 15);
		sprintf_s(buf, 64, "Souris : deplacer le regard de la camera" );
		m_Font.Print( buf, 0, 30);
		sprintf_s(buf, 64, "Z, Q, S, D : deplacer la camera" );
		m_Font.Print( buf, 0, 45);
		break;

	case 1: 
		m_pGraphic->SetCamera(&m_CameraFree);

		sprintf_s(buf, 64, "camera : fixe" );
		m_Font.Print( buf, 0, 15);
		break;

	case 2:
		m_pGraphic->SetCamera(&m_CameraModelViewer);

		sprintf_s(buf, 64, "camera : ModelViewer" );
		m_Font.Print( buf, 0, 15);
		sprintf_s(buf, 64, "Molette souris : zoom" );
		m_Font.Print( buf, 0, 30);
		sprintf_s(buf, 64, "Z, Q, S, D : tourner la camera" );
		m_Font.Print( buf, 0, 45);
		break;
	}

	sprintf_s(buf, 64, "Entrée pour changer de type de caméra" );
	m_Font.Print( buf, 0, 60);

	sprintf_s(buf, 64, "Echappe pour quitter");
	m_Font.Print(buf, 0,  530);

	m_MeshX.Render();
}

/**
 *
 */
bool GameRun::Update( float elapsedTime_ )
{
	cGameCore::Instance().ReadAllInputDevice();

	if ( m_pPad->ButtonPressed(PAD_CANCEL) )
	{
		SetDestroy( true );
		return true;
	}

	if ( m_pPad->ButtonPressed(PAD_FIRE) )
	{
		m_CameraType++;
		
		if ( m_CameraType > 2)
		{
			m_CameraType = 0;
		}
	}

	if ( m_pPad->ButtonHeld(PAD_UP) )
	{
		if ( m_CameraType == 0)
		{
			m_CameraFPS.Forward( elapsedTime_ );
		}
		else if ( m_CameraType == 2)
		{
			m_CameraModelViewer.Rotate( D3DXToRadian(-0.1f), 0.0f, 0.0f);
		}
	}

	if ( m_pPad->ButtonHeld(PAD_DOWN) )
	{
		if ( m_CameraType == 0)
		{
			m_CameraFPS.Backward( elapsedTime_ );
		}
		else if ( m_CameraType == 2)
		{
			m_CameraModelViewer.Rotate( D3DXToRadian(0.1f), 0.0f, 0.0f);
		}
	}

	if ( m_pPad->ButtonHeld(PAD_LEFT) )
	{
		if ( m_CameraType == 0)
		{
			m_CameraFPS.StrafeLeft( elapsedTime_ );
		}
		else if ( m_CameraType == 2)
		{
			m_CameraModelViewer.Rotate( 0.0f, D3DXToRadian(0.1f), 0.0f);
		}
	}

	if ( m_pPad->ButtonHeld(PAD_RIGHT) )
	{
		if ( m_CameraType == 0)
		{
			m_CameraFPS.StrafeRight( elapsedTime_ );
		}
		else if ( m_CameraType == 2)
		{
			m_CameraModelViewer.Rotate( 0.0f, D3DXToRadian(-0.1f), 0.0f);
		}
	}

	if ( m_pPad->MouseWheel() )
	{
		if ( m_CameraType == 2)
		{
			m_pPad->MouseWheel() > 0 ? m_CameraModelViewer.Forward() : m_CameraModelViewer.Backward();
		}
	}

	if ( m_CameraType == 2)
	{
		if ( m_pPad->MouseButton( MOUSE_MBUTTON ) )
		{
			m_CameraModelViewer.Move( (float) m_pPad->GetMouseMotionY(), (float) m_pPad->GetMouseMotionX(), 0.0f);
		}
	}

	if ( m_CameraType == 0)
	{
		m_CameraFPS.Look( (float) m_pPad->GetMouseMotionX() * 5.0f, (float) m_pPad->GetMouseMotionY() * 5.0f, elapsedTime_ );
	}

	return true;
}

