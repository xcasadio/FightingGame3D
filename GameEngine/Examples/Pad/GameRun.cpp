#include "GameRun.h"

#include "GameCore.h"

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
	cGraphic *m_pGraphic = &cGameCore::Instance().GetGraphic();
	cPad *m_pPad = &cGameCore::Instance().GetPad( 0 );

	if ( m_Font.Create("Times News Roman") == false)
	{
		MessageBox(NULL, "Erreur de chargement de la m_Font", "Erreur", MB_OK);
	}

	return true;
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
	
	return true;
}

/**
 *
 */
void GameRun::Draw( float elapsedTime_ )
{
	static char buf[512];
	int offSetY = 100;
	int i;

	sprintf_s(buf, 512, "Position de la souris (x-y) : %d-%d", m_pPad->MouseX(), m_pPad->MouseY());
	m_Font.Print(buf, 0, 0);

	sprintf_s(buf, 512, "Molette souris : %d", m_pPad->MouseWheel() );
	m_Font.Print(buf, 0, 15);
	
	for ( i=0; i<3; i++)
	{
		sprintf_s(buf, 512, "Mouse Bouton %d : %d", i, m_pPad->MouseButton( (eMouseButton) i) ? 1 : 0 );
		m_Font.Print(buf, 0, 30 + i * 15);
	}

	sprintf_s(buf, 512, "Bouton maintenu");
	m_Font.Print(buf, 0, offSetY - 15);

	for ( i=0; i<MAX_PAD_BUTTON; i++)
	{
		sprintf_s(buf, "Bouton %d : %d", i, m_pPad->ButtonHeld( (ePadButton) i) ? 1 : 0 );
		m_Font.Print(buf, 0, offSetY + i * 15);
	}

	sprintf_s(buf, 512, "Bouton Relaché");
	m_Font.Print(buf, 300, offSetY - 15);

	for ( i=0; i<MAX_PAD_BUTTON; i++)
	{
		sprintf_s(buf, "Bouton %d : %d", i, m_pPad->ButtonReleased( (ePadButton) i) ? 1 : 0 );
		m_Font.Print(buf, 300, offSetY + i * 15);
	}

	sprintf_s(buf, 512, "Bouton Pressé");
	m_Font.Print(buf, 600, offSetY - 15);

	for ( i=0; i<MAX_PAD_BUTTON; i++)
	{
		sprintf_s(buf, "Bouton %d : %d", i, m_pPad->ButtonPressed( (ePadButton) i) ? 1 : 0 );
		m_Font.Print(buf, 600, offSetY + i * 15);
	}

	sprintf_s(buf, 512, "Bouton : Z, Q, S, D, Return, Y, U, I, O, P, L");
	m_Font.Print(buf, 0,  500);

	sprintf_s(buf, 512, "Echappe pour quitter");
	m_Font.Print(buf, 0,  530);
}

