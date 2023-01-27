#include "GameRun.h"
#include "GameCore.h"
#include "Collision.h"

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
GameRun::~GameRun(void)
{
}

/**
 *
 */
bool GameRun::Init()
{
	if ( m_MeshX.Load("cube.x") == false )
	{
		MessageBox(NULL, "Erreur cube.x", "Erreur", MB_OK);
		exit(-1);
	}
	
	m_Box1.cBoundingForm::Compute( m_MeshX );
	m_Box2.cBoundingForm::Compute( m_MeshX );

	m_pPad1 = &cGameCore::Instance().GetPad(0);
	m_pGraphic = &cGameCore::Instance().GetGraphic();

	m_Font.Create("Arial");

	m_Light.SetAmbientColor(140, 140, 140);
	m_Light.SetDiffuseColor(140, 140, 140);
	m_Light.SetSpecularColor(140, 140, 140);
	m_Light.SetFalloff(1.0f);
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetRange(10000.0f);
	m_Light.SetPhi(1.0f);
	m_Light.Point(10.0f, 10.0f, -10.0f, 0.0f, 0.0f, 0.0f);

	m_Light2.SetAmbientColor(140, 140, 140);
	m_Light2.SetDiffuseColor(140, 140, 140);
	m_Light2.SetSpecularColor(140, 140, 140);
	m_Light2.SetFalloff(1.0f);
	m_Light2.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light2.SetRange(10000.0f);
	m_Light2.SetPhi(1.0f);
	m_Light2.Point(-10.0f, -10.0f, -10.0f, 0.0f, 0.0f, 0.0f);

	m_pGraphic->EnableZBuffer(true);
	m_pGraphic->EnableLighting(true);
	m_pGraphic->SetAmbientLight( 0, 0, 0);

	m_pGraphic->SetLight(0, &m_Light);
	m_pGraphic->EnableLight(0);

	m_pGraphic->SetLight(1, &m_Light2);
	m_pGraphic->EnableLight(1);
	
	m_ViewPort1.MinZ = m_ViewPort2.MinZ = m_ViewPort3.MinZ = m_ViewPort4.MinZ = 0.0f;
	m_ViewPort1.MaxZ = m_ViewPort2.MaxZ = m_ViewPort3.MaxZ = m_ViewPort4.MaxZ = 1.0f;
	m_ViewPort1.Width = m_ViewPort2.Width = m_ViewPort3.Width = m_ViewPort4.Width = m_pGraphic->GetWidth() / 2;
	m_ViewPort1.Height = m_ViewPort2.Height = m_ViewPort3.Height = m_ViewPort4.Height = m_pGraphic->GetHeight() / 2;

	m_ViewPort1.X = 0;
	m_ViewPort1.Y = 0;
	m_ViewPort2.X = m_pGraphic->GetWidth() / 2;
	m_ViewPort2.Y = 0;
	m_ViewPort3.X = 0;
	m_ViewPort3.Y = m_pGraphic->GetHeight() / 2;
	m_ViewPort4.X = m_pGraphic->GetWidth() / 2;
	m_ViewPort4.Y = m_pGraphic->GetHeight() / 2;

	m_CubeSelected = 0;

	m_Collide = false;

	ZeroMemory( &m_Mat1, sizeof( D3DMATERIAL9 ) );
	ZeroMemory( &m_Mat2, sizeof( D3DMATERIAL9 ) );

	m_Mat1.Ambient.a = m_Mat1.Diffuse.a = 1.0f;
	m_Mat1.Ambient.r = m_Mat1.Diffuse.r = 1.0f;

	m_Mat2.Ambient.a = m_Mat2.Diffuse.a = 1.0f;
	m_Mat2.Ambient.b = m_Mat2.Diffuse.b = 1.0f;
	return true;
}

/**
 *
 */
void GameRun::Draw( float elapsedTime_ )
{
	static char buf[64];

	/////////////////   ViewPort 1
	m_pGraphic->GetDeviceCOM()->SetViewport( &m_ViewPort1 );

	m_CameraFree.Point( 0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 0.0f);
	m_pGraphic->SetCamera(&m_CameraFree);

	m_pGraphic->Clear(0);

	m_pGraphic->BeginScene();

	sprintf_s(buf, 64, "FPS : %d", cGameCore::Instance().GetGraphic().GetFPS());
	m_Font.Print( buf, 0, 0);

	if ( m_Collide )
	{
		sprintf_s(buf, 64, "Collision" );
		m_Font.Print( buf, 0, 15);
	}

	sprintf_s(buf, 64, "Echappe pour quitter");
	m_Font.Print(buf, 0,  530);

	m_pGraphic->SetWorldPosition( &m_Pos1 );
	m_MeshX.Render( &m_Mat1, false );

	m_pGraphic->SetWorldPosition( &m_Pos2 );
	m_MeshX.Render( &m_Mat2, false );

	m_pGraphic->EndScene();

	/////////////////   ViewPort 2
	m_pGraphic->GetDeviceCOM()->SetViewport( &m_ViewPort2 );

	m_CameraFree.Point( 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_pGraphic->SetCamera(&m_CameraFree);

	m_pGraphic->Clear(0);

	m_pGraphic->BeginScene();

	m_pGraphic->SetWorldPosition( &m_Pos1 );
	m_MeshX.Render( &m_Mat1, false );

	m_pGraphic->SetWorldPosition( &m_Pos2 );
	m_MeshX.Render( &m_Mat2, false );

	m_pGraphic->EndScene();

	/////////////////   ViewPort 3
	m_pGraphic->GetDeviceCOM()->SetViewport( &m_ViewPort3 );

	m_CameraFree.Point( 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_pGraphic->SetCamera(&m_CameraFree);

	m_pGraphic->Clear(0);

	m_pGraphic->BeginScene();

	m_pGraphic->SetWorldPosition( &m_Pos1 );
	m_MeshX.Render( &m_Mat1, false );

	m_pGraphic->SetWorldPosition( &m_Pos2 );
	m_MeshX.Render( &m_Mat2, false );

	m_pGraphic->EndScene();

	/////////////////   ViewPort 4
	m_pGraphic->GetDeviceCOM()->SetViewport( &m_ViewPort4 );

	m_CameraFree.Point( 5.0f, 2.5f, -5.0f, 0.0f, 0.0f, 0.0f);
	m_pGraphic->SetCamera(&m_CameraFree);

	m_pGraphic->Clear(0);

	m_pGraphic->BeginScene();

	m_pGraphic->SetWorldPosition( &m_Pos1 );
	m_MeshX.Render( &m_Mat1, false );

	m_pGraphic->SetWorldPosition( &m_Pos2 );
	m_MeshX.Render( &m_Mat2, false );
}

/**
 *
 */
bool GameRun::Update( float elapsedTime_ )
{
	cGameCore::Instance().ReadAllInputDevice();

	if ( m_pPad1->ButtonPressed(PAD_CANCEL) )
	{
		SetDestroy( true );
		return true;
	}

	if ( m_pPad1->ButtonPressed(PAD_FIRE) )
	{
		m_CubeSelected = 1 - m_CubeSelected;
	}

	if ( m_pPad1->ButtonHeld(PAD_UP) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.MoveRel( 0.0f, 0.0f, 1.0f * elapsedTime_ );
		}
		else
		{
			m_Pos2.MoveRel( 0.0f, 0.0f, 1.0f * elapsedTime_ );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_DOWN) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.MoveRel( 0.0f, 0.0f, -1.0f * elapsedTime_ );
		}
		else
		{
			m_Pos2.MoveRel( 0.0f, 0.0f, -1.0f * elapsedTime_ );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_LEFT) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.MoveRel( -1.0f * elapsedTime_, 0.0f, 0.0f );
		}
		else
		{
			m_Pos2.MoveRel( -1.0f * elapsedTime_, 0.0f, 0.0f );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_RIGHT) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.MoveRel( 1.0f * elapsedTime_, 0.0f, 0.0f );
		}
		else
		{
			m_Pos2.MoveRel( 1.0f * elapsedTime_, 0.0f, 0.0f );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_BUTTON_8) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.MoveRel( 0.0f, 1.0f * elapsedTime_, 0.0f );
		}
		else
		{
			m_Pos2.MoveRel( 0.0f, 1.0f * elapsedTime_, 0.0f );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_BUTTON_7) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.MoveRel( 0.0f, -1.0f * elapsedTime_, 0.0f );
		}
		else
		{
			m_Pos2.MoveRel( 0.0f, -1.0f * elapsedTime_, 0.0f );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_BUTTON_6) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.RotateRel( 0.0f, D3DXToRadian( 20.0f ) * elapsedTime_, 0.0f );
		}
		else
		{
			m_Pos2.RotateRel( 0.0f, D3DXToRadian( 20.0f ) * elapsedTime_, 0.0f );
		}
	}

	if ( m_pPad1->ButtonHeld(PAD_BUTTON_5) )
	{
		if ( m_CubeSelected == 0 )
		{
			m_Pos1.RotateRel( D3DXToRadian( 20.0f ) * elapsedTime_, 0.0f, 0.0f );
		}
		else
		{
			m_Pos2.RotateRel( D3DXToRadian( 20.0f ) * elapsedTime_, 0.0f, 0.0f );
		}
	}

	m_Collide = cCollision::OrientedBoundingBox( m_Box1, *m_Pos1.GetMatrix(), m_Box2, *m_Pos2.GetMatrix() );

	return true;
}
