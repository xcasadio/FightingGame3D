#include "GameRun.h"

#include "Utilities/Macros.h"

#include <d3dx9math.h>
#include <d3dx9anim.h>

using namespace GameEngine;

GameRun::GameRun()
{
}

GameRun::~GameRun(void)
{
}

bool GameRun::Init()
{
	m_pGraphic = &cGameCore::Instance().GetGraphic();
	m_pPad1 = &cGameCore::Instance().GetPad(0);

	m_Mesh.Load("biped fighting game.X");

	m_pGraphic->EnableZBuffer(true);
	m_pGraphic->EnableLighting(true);
	m_pGraphic->SetAmbientLight(0, 0, 0);

	m_CameraFree.Point( 0.0f, 5.0f, -30.0f, 0.0f, 5.0f, 0.0f);

	m_Font.Create("Arial");
	
	cLight m_Light, m_Light2, m_Light3;
	m_Light.SetFalloff(1.0f);
	m_Light.SetAmbientColor(255,255,255);
	m_Light.SetDiffuseColor(255,255,255);
	m_Light.SetSpecularColor(255,255,255);
	m_Light.SetRange(10000.0f);
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.Point( -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_Light.SetPhi(1.0f);

	m_Light2.SetFalloff(1.0f);
	m_Light2.SetAmbientColor(255,255,255);
	m_Light2.SetDiffuseColor(255,255,255);
	m_Light2.SetSpecularColor(255,255,255);
	m_Light2.SetRange(10000.0f);
	m_Light2.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light2.Point(100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_Light2.SetPhi(1.0f);

	m_Light3.SetFalloff(1.0f);
	m_Light3.SetAmbientColor(255,255,255);
	m_Light3.SetDiffuseColor(255,255,255);
	m_Light3.SetSpecularColor(255,255,255);
	m_Light3.SetRange(10000.0f);
	m_Light3.SetType(D3DLIGHT_POINT);
	m_Light3.Point(0.0f, 100.0f, -50.0f, 0.0f, 0.0f, 0.0f);
	m_Light3.SetPhi(1.0f);

	m_pGraphic->SetLight(0, &m_Light);
	m_pGraphic->SetLight(1, &m_Light2);
	m_pGraphic->SetLight(2, &m_Light3);
	m_pGraphic->EnableLight(0);
	m_pGraphic->EnableLight(1);
	m_pGraphic->EnableLight(2);
	
	D3DXMatrixIdentity( &m_MatWorld );

	float p = 0.0f;
	m_CurrentAnimation = 0;
	m_Pause = false;

	return true;
}

bool GameRun::Update( float elapsedTime_ )
{
	cGameCore::Instance().ReadAllInputDevice();

	if ( m_pPad1->ButtonPressed(PAD_CANCEL) )
	{
		SetDestroy(true);
		return true;
	}

	if ( m_pPad1->ButtonPressed(PAD_LEFT) )
	{
		m_CurrentAnimation++;

		if ( m_CurrentAnimation == m_Mesh.GetNumberOfAnimationSet() )
		{
			m_CurrentAnimation = 0;
		}

		m_Mesh.SetAnimationSet( m_CurrentAnimation );
	}

	if ( m_pPad1->ButtonHeld(PAD_DOWN) )
	{
		//m_Mesh.GetAnimationController()->SetTrackPosition(m_Mesh.GetCurrentTrack(), 0.0f);
		m_Pause = true;
	}
	else
	{
		m_Pause = false;
	}
	
	if ( m_Pause == false )
	{
		m_Mesh.Update( elapsedTime_, &m_MatWorld );
	}

	return true;
}

void GameRun::Draw( float elapsedTime_ )
{
	char buf[64];

	m_pGraphic->SetCamera( &m_CameraFree);

	m_Mesh.Draw();

	sprintf_s( buf, sizeof(buf), "FPS : %d", m_pGraphic->GetFPS() );
	m_Font.Print( buf, 0, 0);

	sprintf_s(buf, sizeof(buf), "Animation : %d - %s", m_Mesh.GetCurrentAnimationSet(), m_Mesh.GetAnimationSetName(m_Mesh.GetCurrentAnimationSet()).c_str() );
	m_Font.Print( buf, 0, 20);

	sprintf_s( buf, sizeof(buf), "Time %f - %f - %f", m_Mesh.GetAnimationController()->GetTime(), m_Mesh.GetCurrentAnimationTime(), m_Mesh.GetCurrentAnimationTimeMax() );
	m_Font.Print( buf, 0, 40);

	sprintf_s( buf, sizeof(buf), "'Q' : changer d'animation" );
	m_Font.Print( buf, 0, 60);

	sprintf_s( buf, sizeof(buf), "'S' : maintenir pour figer l'animation" );
	m_Font.Print( buf, 0, 80);

	sprintf_s( buf, sizeof(buf), "Echappe pour quitter");
	m_Font.Print( buf, 0,  530);
}
