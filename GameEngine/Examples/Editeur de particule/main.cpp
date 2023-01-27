#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "GameCore.h"
#include "Window.h"
#include "Graphic.h"
#include "Light.h"
#include "Camera.h"
#include "Font.h"
#include "MeshX.h"
#include "Pad.h"
#include "ParticleEngine/ParticleSystem.h"
#include "ResourceManager.h"
#include "MediaPathManager.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

//variable globale
cLight				g_Light;
POINT				g_MousePosition;
HWND				g_hWnd;
HINSTANCE			g_hInst;
HANDLE				g_hThread;
cFontDX				g_font;
OPENFILENAME		g_ofn;
char				g_FileName[MAX_PATH];
cParticleSystem		g_Particle;
bool				g_ParticleSystemIsLoaded = false;
cCameraModelViewer	g_cameraModelViewer;
cPad				g_Pad;
D3DXVECTOR3			g_vSpread = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
bool				g_bParticleStart = false;

DWORD				g_BackgroundColor = 0;

bool				g_DisplayGrid = true;

cMeshX				g_MeshBoundingBox, g_MeshBoundingSphere;

BOOL CALLBACK	MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY	SetParticleDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY	ColorDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI	MyThreadFunction(LPVOID lpParam);

void			FillMainDialogItem();

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	g_hInst = hInst;

	cMediaPathManager::Instance().AddPath( "./" );

	if ( cGameCore::Instance().GetGraphic().Init() == false)
		return -1;

	//init
	g_cameraModelViewer.SetRadius(50.0f);
	g_cameraModelViewer.GetArcBall()->SetWindow(512, 512, 50.f); 
	g_cameraModelViewer.GetArcBall()->SetOffset(290, 10);

	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, MainDialogProc);

	cGameCore::Instance().GetGraphic().Shutdown();

	return 0;
}

BOOL CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	cGraphic &graphic = cGameCore::Instance().GetGraphic();

	switch(uMsg) {
    case WM_INITDIALOG:
		g_hWnd = hWnd;
		
		if ( graphic.SetMode(GetDlgItem(hWnd, IDC_PREVIEW), true, true) == false )
			return -1;

		if ( !cGameCore::Instance().CreateInputDevice( hWnd, g_hInst, true ) )
			return -1;

		for ( int i = 0; i < MAX_PAD; i++ )
		{
			if ( !cGameCore::Instance().GetPad(i).Load("pad.xml") )
				MessageBox(NULL, "Erreur pad load()", "Erreur", MB_OK);
		}

		if ( g_font.Create("Arial") == false )
			return -1;

		graphic.SetAmbientLight(0, 0, 0);
		graphic.EnableLighting(true);
		graphic.EnableZBuffer(true);

		g_Light.SetAmbientColor(128, 128, 128);
		g_Light.SetDiffuseColor(128, 128, 128);
		g_Light.SetSpecularColor(255, 255, 255);
		g_Light.SetAttenuation0(1.0f);
		g_Light.SetFalloff(0.0f);
		g_Light.Move(0.0f, 0.0, -10.0f);
		
		graphic.SetLight(0, &g_Light);
		graphic.EnableLight(0, true);

		SendMessage( GetDlgItem(hWnd, IDC_CHECK_CONTINOUS), BM_SETCHECK, BST_CHECKED, 0);

		ZeroMemory(&g_ofn, sizeof(OPENFILENAME));
		g_ofn.lStructSize = sizeof(OPENFILENAME);
		g_ofn.nMaxFile = MAX_PATH;
		g_ofn.nMaxFileTitle = MAX_PATH;
		g_ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;

		g_hWnd = hWnd;
		
		g_hThread = CreateThread( 
				NULL,
				0,
				MyThreadFunction,
				NULL,
				0,
				NULL);

		if (g_hThread == NULL) 
		{
			exit(-1);
		}

		g_Particle.SetEmissionPerSec(100);
		g_Particle.SetContinuous(true);
		g_Particle.SetParticleMax(200);
		g_Particle.SetParticleLife( 300, 1000);
		g_Particle.SetSpeed( 3.0f, 6.0f);
		g_Particle.SetSize( 1.0f, 2.0f);
		g_Particle.SetColorStart( 1.0f, 1.0f, 1.0f, 1.0f);
		g_Particle.SetColorEnd( 1.0f, 1.0f, 0.0f, 0.0f);
		g_Particle.SetSpread( D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		g_Particle.SetDirection( D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_LIFE), "3000" );

		SendMessage( GetDlgItem(hWnd, IDC_CHECK_CONTINOUS), BM_SETCHECK, BST_CHECKED, 0 );

		//TODO : menu grille checked
		//SendMessage( GetDlgItem(hWnd, ID_OPTION_GRILLE), BM_SETCHECK, BST_CHECKED, 0 );

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_EMISSION), "100" );

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PARTICLE_MAX), "200" );

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_LIFE_MIN), "300" );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_LIFE_MAX), "1000" );

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPEED_MIN), "3.0" );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPEED_MAX), "6.0" );

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SIZE_MIN), "1.0" );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SIZE_MAX), "2.0" );

		//TODO : couleur
		//COLOR_WINDOWFRAME

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPREAD_X), "0.0" );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPREAD_Y), "0.0" );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPREAD_Z), "0.0" );

		return TRUE;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case ID_FICHIER_CHARGER:	
			{
				g_ofn.hwndOwner   = hWnd;
				g_ofn.lpstrFile   = g_FileName;
				g_ofn.lpstrTitle  = "Load particle File";
				g_ofn.lpstrFilter = "Particle files (*.particle)\0*.particle\0All Files (*.*)\0*.*\0\0";
				g_ofn.lpstrDefExt = "particle";

				if(!GetOpenFileName(&g_ofn))
					return FALSE;

				SuspendThread(g_hThread);

				if ( g_Particle.LoadSerialized(g_ofn.lpstrFile) )
				{
					if ( g_Particle.GetParticleVisualObject() != NULL )
					{
						g_ParticleSystemIsLoaded = true;
						FillMainDialogItem();
					}
					else
					{
						g_ParticleSystemIsLoaded = false;
					}
				}
				
				ResumeThread(g_hThread);
			}
			break;

		case ID_FICHIER_SAUVEGARDER:	
			{
				g_ofn.hwndOwner   = hWnd;
				g_ofn.lpstrFile   = g_FileName;
				g_ofn.lpstrTitle  = "Save particle File";
				g_ofn.lpstrFilter = "Particle files (*.particle)\0*.particle\0All Files (*.*)\0*.*\0\0";
				g_ofn.lpstrDefExt = "particle";

				if(!GetSaveFileName(&g_ofn))
					return FALSE;

				SuspendThread(g_hThread);

				g_Particle.SaveSerialized(g_ofn.lpstrFile);
				
				ResumeThread(g_hThread);
			}
			break;

		case ID_OPTION_COULEURFOND:
			{
				COLORREF acrCustClr[16];
				CHOOSECOLOR choosecolor;
				ZeroMemory(&choosecolor, sizeof(CHOOSECOLOR));

				choosecolor.Flags = CC_FULLOPEN;
				choosecolor.hwndOwner = g_hWnd;
				choosecolor.lpCustColors = (LPDWORD) acrCustClr;
				choosecolor.lStructSize = sizeof(CHOOSECOLOR);

				if ( ChooseColor(&choosecolor) )
				{
					g_BackgroundColor = D3DCOLOR_ARGB(255, GetRValue(choosecolor.rgbResult), GetGValue(choosecolor.rgbResult), GetBValue(choosecolor.rgbResult) ); 
				}
			}
			break;

		case ID_OPTION_GRILLE:
			{
				
			}
			break;

		case IDC_BUTTON_COLOR_START:
			{
				int p = 0;
				if ( DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_COLOR), hWnd, ColorDialogProc, (LPARAM) (&p)) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_COLOR start", "Erreur", MB_OK);
				}
			}
			break;

		case IDC_BUTTON_COLOR_END:
			{
				int p = 1;
				if ( DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_COLOR), hWnd, ColorDialogProc, (LPARAM) (&p)) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_COLOR end", "Erreur", MB_OK);
				}
			}
			break;

		case IDC_BUTTON_SET_PARTICLE:
			if ( DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_PARTICLE), hWnd, SetParticleDialogProc) <= 0)
			{
				MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_PARTICLE", "Erreur", MB_OK);
			}
			break;

		case IDC_EDIT_LIFE:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				g_Particle.SetLife( GetDlgItemInt( g_hWnd, IDC_EDIT_LIFE, NULL, false) );
			}
			break;

		case IDC_EDIT_EMISSION:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				g_Particle.SetEmissionPerSec( GetDlgItemInt( g_hWnd, IDC_EDIT_EMISSION, NULL, false) );
			}
			break;

		case IDC_EDIT_PARTICLE_MAX:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				g_Particle.SetParticleMax( GetDlgItemInt( g_hWnd, IDC_EDIT_PARTICLE_MAX, NULL, false) );
			}
			break;

		//TODO continuous

		case IDC_EDIT_LIFE_MIN:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				g_Particle.SetParticleLife( GetDlgItemInt( g_hWnd, IDC_EDIT_LIFE_MIN, NULL, false), 
											GetDlgItemInt( g_hWnd, IDC_EDIT_LIFE_MAX, NULL, false) );
			}
			break;

		case IDC_EDIT_LIFE_MAX:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				g_Particle.SetParticleLife( GetDlgItemInt( g_hWnd, IDC_EDIT_LIFE_MIN, NULL, false), 
											GetDlgItemInt( g_hWnd, IDC_EDIT_LIFE_MAX, NULL, false) );
			}
			break;

		case IDC_EDIT_SPEED_MIN: case IDC_EDIT_SPEED_MAX:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				char buf[64];
				float a, b;

				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SPEED_MIN), buf, 63);
				a = (float) atof(buf);

				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SPEED_MAX), buf, 63);
				b = (float) atof(buf);

				g_Particle.SetSpeed( a, b);
			}
			break;

		case IDC_EDIT_SIZE_MIN: case IDC_EDIT_SIZE_MAX:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				char buf[64];
				float a, b;

				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SIZE_MIN), buf, 63);
				a = (float) atof(buf);

				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SIZE_MAX), buf, 63);
				b = (float) atof(buf);

				g_Particle.SetSize( a, b);
			}
			break;

		case IDC_EDIT_SPREAD_X:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				char buf[64];
				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SPREAD_X), buf, 63);
				g_vSpread.x = (float) D3DXToRadian( atof(buf) );

				g_Particle.SetSpread( g_vSpread );
			}
			break;

		case IDC_EDIT_SPREAD_Y:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				char buf[64];
				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SPREAD_Y), buf, 63);
				g_vSpread.y = (float) D3DXToRadian( atof(buf) );

				g_Particle.SetSpread( g_vSpread );
			}
			break;

		case IDC_EDIT_SPREAD_Z:
			if ( HIWORD(wParam) == EN_CHANGE )
			{
				char buf[64];
				GetWindowText( GetDlgItem( g_hWnd, IDC_EDIT_SPREAD_Z), buf, 63);
				g_vSpread.z = (float) D3DXToRadian( atof(buf) );

				g_Particle.SetSpread( g_vSpread );
			}
			break;

		case IDC_BUTTON_START:
			// pour efectuer le start dans le thread sinon
			// il y a des problemes de synchronisation
			g_bParticleStart = true;
			break;

		case IDC_BUTTON_UPDATE:
			{
				if ( SendMessage( GetDlgItem(hWnd, IDC_CHECK_CONTINOUS), BM_GETCHECK, 0, 0 ) )
				{
					g_Particle.SetContinuous(true);
				}
				else
				{
					g_Particle.SetContinuous(false);
				}
			}
			break;
		} // fin switch COMMAND
		break;

	case WM_CLOSE:

		if ( g_hThread )
		{
			CloseHandle(g_hThread);
		}

		g_hThread = NULL;

		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return FALSE;//g_cameraModelViewer.GetArcBall()->HandleMessages( hWnd, uMsg, wParam, lParam);
}


DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
	cGraphic *graphic = &cGameCore::Instance().GetGraphic();

	cFontDX font;
	font.Create("Courrier New", 10);
	char buf[128];

	while(graphic->BeginFrame())
	{
		graphic->Clear(g_BackgroundColor);
		if ( graphic->BeginScene() )
		{
			graphic->SetCamera(&g_cameraModelViewer);

			if ( g_ParticleSystemIsLoaded )
			{
				g_Particle.Render();
			}

			//TODO : afficher grille
			if ( g_DisplayGrid )
			{
				
			}
			
			sprintf_s(buf, 128, "Cliquez et utilisez la souris pour pivoter autour du systeme de particule");
			font.Print( buf, 0, 500, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 255) );

			if ( !g_ParticleSystemIsLoaded )
			{
				sprintf_s(buf, 128, "Pas de particule chargée" );
				font.Print( buf, 0, 0, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 255) );
			}
			else
			{
				sprintf_s(buf, 128, "FPS %d", graphic->GetFPS() );
				font.Print( buf, 0, 0, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 255) );

				sprintf_s(buf, 128, "nb particle %d", g_Particle.GetNumParticle() );
				font.Print( buf, 0, 10, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 255) );
			}

			graphic->EndScene();
			
			graphic->Display();
		}

		float time = (float) graphic->FrameTime() / 1000.0f;

		if ( g_ParticleSystemIsLoaded )
		{
			g_Particle.Update(time);
		}

		cGameCore::Instance().ReadAllInputDevice();

		if ( g_ParticleSystemIsLoaded )
		{
			if ( ( g_Pad.GetMouseMotionX() || g_Pad.GetMouseMotionY() )
				&& g_Pad.MouseX() > 290 && g_Pad.MouseX() < 777
				&& g_Pad.MouseY() > 10 && g_Pad.MouseY() < 545
				&& g_Pad.MouseButton( MOUSE_LBUTTON)
				)
			{
				g_cameraModelViewer.Rotate( D3DXToRadian( (float) g_Pad.GetMouseMotionY() / 5.0f), D3DXToRadian( (float) -g_Pad.GetMouseMotionX() / 5.0f), 0.0f );
			}

			if ( g_Pad.MouseWheel() != 0 )
			{
				g_Pad.MouseWheel() > 0 ? g_cameraModelViewer.Forward() : g_cameraModelViewer.Backward();
			}
		}

		if ( g_bParticleStart )
		{
			g_Particle.Start();
			g_bParticleStart = false;
		}

		graphic->EndFrame();

		if ( !g_ParticleSystemIsLoaded )
		{
			Sleep(17);
		}
    }

    ExitThread(0); 
} 

BOOL APIENTRY  SetParticleDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static IParticleVisualObject *pParticleVisualObject = NULL;

	//TODO : alphaEnabled

	switch(uMsg) {
    case WM_INITDIALOG:
		{
			//combo source
			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_SRCALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_SRCCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVSRCALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVSRCCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_DESTALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_DESTCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVDESTALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVDESTCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_ONE") );


			//combo destination
			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_SRCALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_SRCCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVSRCALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVSRCCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_DESTALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_DESTCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVDESTALPHA") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_INVDESTCOLOR") );

			SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"D3DBLEND_ONE") );

			if ( g_Particle.GetParticleVisualObject() == NULL )
			{
				SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 0, 0);
				SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND), CB_SETCURSEL, 0, 0);
				SendMessage( GetDlgItem(hWnd, IDC_CHECK_LIGHTING), BM_SETCHECK, 1, 0 );
				SendMessage( GetDlgItem(hWnd, IDC_CHECK_ZWRITE), BM_SETCHECK, 1, 0 );
			}
			else
			{
				//name
				//g_Particle.GetParticleVisualObject()->

				//checkBox
				if ( g_Particle.GetParticleVisualObject()->GetLighting() )
				{
					SendMessage( GetDlgItem(hWnd, IDC_CHECK_LIGHTING), BM_SETCHECK, 1, 0 );
				}
				else
				{
					SendMessage( GetDlgItem(hWnd, IDC_CHECK_LIGHTING), BM_SETCHECK, 0, 0 );
				}

				if ( g_Particle.GetParticleVisualObject()->GetZWrite() )
				{
					SendMessage( GetDlgItem(hWnd, IDC_CHECK_ZWRITE), BM_SETCHECK, 1, 0 );
				}
				else
				{
					SendMessage( GetDlgItem(hWnd, IDC_CHECK_ZWRITE), BM_SETCHECK, 0, 0 );
				}

				//Combo
				switch ( g_Particle.GetParticleVisualObject()->GetSrcBlend()  )
				{
				case D3DBLEND_SRCALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 0, 0);
					break;

				case D3DBLEND_SRCCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 1, 0);
					break;

				case D3DBLEND_INVSRCALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 2, 0);
					break;

				case D3DBLEND_INVSRCCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 3, 0);
					break;

				case D3DBLEND_DESTALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 4, 0);
					break;

				case D3DBLEND_DESTCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 5, 0);
					break;

				case D3DBLEND_INVDESTALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 6, 0);
					break;

				case D3DBLEND_INVDESTCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 7, 0);
					break;

				case D3DBLEND_ONE:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_SETCURSEL, 8, 0);
					break;
				}

				switch ( g_Particle.GetParticleVisualObject()->GetDestBlend()  )
				{
				case D3DBLEND_SRCALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 0, 0);
					break;

				case D3DBLEND_SRCCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 1, 0);
					break;

				case D3DBLEND_INVSRCALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 2, 0);
					break;

				case D3DBLEND_INVSRCCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 3, 0);
					break;

				case D3DBLEND_DESTALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 4, 0);
					break;

				case D3DBLEND_DESTCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 5, 0);
					break;

				case D3DBLEND_INVDESTALPHA:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 6, 0);
					break;

				case D3DBLEND_INVDESTCOLOR:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 7, 0);
					break;

				case D3DBLEND_ONE:
					SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_SETCURSEL, 8, 0);
					break;
				}
			} // fin else g_Particle.GetParticleVisualObject() == NULL
			
			return TRUE;
		}

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_CHECK_LIGHTING:
			if ( HIWORD(wParam) == BN_CLICKED)
			{
				if ( pParticleVisualObject )
				{
					pParticleVisualObject->SetLighting( IsDlgButtonChecked(hWnd, IDC_CHECK_LIGHTING) == BST_CHECKED ? true : false );
				}
			}
			break;

		case IDC_CHECK_ZWRITE:
			if ( HIWORD(wParam) == BN_CLICKED)
			{
				if ( pParticleVisualObject )
				{
					pParticleVisualObject->SetZWrite( IsDlgButtonChecked(hWnd, IDC_CHECK_ZWRITE) == BST_CHECKED ? true : false );
				}
			}
			break;

		case IDC_BUTTON_FILE:
			{
				//TODO - supprimer pParticleVisualObject ou le faire directement dans la classe

				g_ofn.hwndOwner   = hWnd;
				g_ofn.lpstrFile   = g_FileName;
				g_ofn.lpstrTitle  = "Load Mexh File";
				g_ofn.lpstrFilter = "Picture Files (jpg, bmp, png)\0*.jpg;*.bmp;*.png\0Direct X Files (*.x)\0*.x;*.X\0All Files (*.*)\0*.*\0\0";
				g_ofn.lpstrDefExt = "x";

				if(!GetOpenFileName(&g_ofn))
					return FALSE;

				if ( pParticleVisualObject )
				{
					delete pParticleVisualObject;
					pParticleVisualObject = NULL;
					g_ParticleSystemIsLoaded = false;
				}

				std::string str(g_ofn.lpstrFile), str2;
				size_t found;

				found = str.find_last_of(".");
				//str.substr(0,found); //folder
				str2 = str.substr(found+1); //file

				if ( str2.compare("x") == 0 || str2.compare("X") == 0 )
				{
					//pParticleVisualObject
					//pParticleVisualObject = new cMeshX;
					//cMeshX *pMesh = dynamic_cast<cMeshX *> (pParticleVisualObject);

					//if ( pMesh )
					//{
						cMeshX *pMesh = cResourceManager::Instance().Get<cMeshX>( str.c_str() );

						if ( pMesh == NULL )
						{
							pMesh = new cMeshX;

							if ( pMesh->Load( str.c_str() ) == false )
							{
								cWindow::Error( false, "Erreur lors du chargement du fichier .x : %s", str.c_str() );
								g_ParticleSystemIsLoaded = false;
								return FALSE;
							}

							cResourceManager::Instance().Add( str.c_str(), pMesh);
						}

						pParticleVisualObject = dynamic_cast<IParticleVisualObject *> (pMesh);

						g_Particle.SetVisualObject( pMesh );
						g_ParticleSystemIsLoaded = true;
					/*}
					else
					{
						cWindow::Error( false, "Erreur lors du dynamic_cast<cMeshX *>");
						return FALSE;
					}*/
				}
				else if ( str2.compare("jpg") == 0 || str2.compare("JPG") == 0 ||
					str2.compare("png") == 0 || str2.compare("PNG") == 0 ||
					str2.compare("bmp") == 0 || str2.compare("BMP") == 0 )
				{
					pParticleVisualObject = new cParticleBillBoard;
					cParticleBillBoard *pParticleBillBoard = dynamic_cast<cParticleBillBoard *> (pParticleVisualObject);
					
					if ( pParticleBillBoard )
					{
						if ( pParticleBillBoard->Load(str.c_str()) )
						{
							g_Particle.SetVisualObject(pParticleBillBoard);
							g_ParticleSystemIsLoaded = true;
						}
						else
						{
							cWindow::Error( false, "Erreur lors du chargement du fichier image : %s", str.c_str() );
							return FALSE;
						}
					}
					else
					{
						cWindow::Error( false, "Erreur lors du dynamic_cast<cParticleBillBoard *>");
						return FALSE;
					}
				}
				else
				{
					cWindow::Error( false, "Format Inconnu, choisir : .x, .png, .jpg ou .bmp");
					return FALSE;
				}

				if ( g_ParticleSystemIsLoaded )
				{
					SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PARTICLE_FILE_NAME), str.c_str());
					g_Particle.Start();
				}
			}
			break;

		case IDC_BUTTON_SET_PARTICLE_OK:
			{
				//TODO affecter les valeurs au IParticleVisualObject
				DWORD src = D3DBLEND_SRCALPHA, dest = D3DBLEND_SRCALPHA;

				//src_mode
				switch ( SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_GETCURSEL, 0, 0) )
				{
				case 0:
					src = D3DBLEND_SRCALPHA;
					break;

				case 1:
					src = D3DBLEND_SRCCOLOR;
					break;

				case 2:
					src = D3DBLEND_INVSRCALPHA;
					break;

				case 3:
					src = D3DBLEND_INVSRCCOLOR;
					break;

				case 4:
					src = D3DBLEND_DESTALPHA;
					break;

				case 5:
					src = D3DBLEND_DESTCOLOR;
					break;

				case 6:
					src = D3DBLEND_INVDESTALPHA;
					break;

				case 7:
					src = D3DBLEND_INVDESTCOLOR;
					break;

				case 8:
					src = D3DBLEND_ONE;
					break;
				}

				//dest_mode
				switch ( SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BEND), CB_GETCURSEL, 0, 0) )
				{
				case 0:
					dest = D3DBLEND_SRCALPHA;
					break;

				case 1:
					dest = D3DBLEND_SRCCOLOR;
					break;

				case 2:
					dest = D3DBLEND_INVSRCALPHA;
					break;

				case 3:
					dest = D3DBLEND_INVSRCCOLOR;
					break;

				case 4:
					dest = D3DBLEND_DESTALPHA;
					break;

				case 5:
					dest = D3DBLEND_DESTCOLOR;
					break;

				case 6:
					dest = D3DBLEND_INVDESTALPHA;
					break;

				case 7:
					dest = D3DBLEND_INVDESTCOLOR;
					break;

				case 8:
					dest = D3DBLEND_ONE;
					break;
				}

				if ( pParticleVisualObject )
				{
					pParticleVisualObject->SetAlphaEnabled(true);
					pParticleVisualObject->SetDestBlend( (DWORD) dest);
					pParticleVisualObject->SetSrcBlend( (DWORD) src);

					if ( SendMessage( GetDlgItem(hWnd, IDC_CHECK_ZWRITE), BM_GETCHECK, 0, 0) )
					{
						pParticleVisualObject->SetZWrite(true);
					}
					else
					{
						pParticleVisualObject->SetZWrite(false);
					}

					if ( SendMessage( GetDlgItem(hWnd, IDC_CHECK_LIGHTING), BM_GETCHECK, 0, 0) )
					{
						pParticleVisualObject->SetLighting(true);
					}
					else
					{
						pParticleVisualObject->SetLighting(false);
					}
				}

				EndDialog( hWnd, TRUE);
			}
			break;

		case IDC_COMBO_SRC_BLEND:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{	
				if ( pParticleVisualObject == NULL )
				{
					return FALSE;
				}

				switch ( SendMessage( GetDlgItem(hWnd, IDC_COMBO_SRC_BEND), CB_GETCURSEL, 0, 0) )
				{
				case 0:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_SRCALPHA);
					break;

				case 1:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_SRCCOLOR);
					break;

				case 2:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_INVSRCALPHA);
					break;

				case 3:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_INVSRCCOLOR);
					break;

				case 4:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_DESTALPHA);
					break;

				case 5:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_DESTCOLOR);
					break;

				case 6:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_INVDESTALPHA);
					break;

				case 7:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_INVDESTCOLOR);
					break;

				case 8:
					pParticleVisualObject->SetSrcBlend(D3DBLEND_ONE);
					break;
				}
			}
			break;

		case IDC_COMBO_DEST_BLEND:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{		
				if ( pParticleVisualObject == NULL )
				{
					return FALSE;
				}

				switch ( SendMessage( GetDlgItem(hWnd, IDC_COMBO_DEST_BLEND), CB_GETCURSEL, 0, 0) )
				{
				case 0:
					pParticleVisualObject->SetDestBlend(D3DBLEND_SRCALPHA);
					break;

				case 1:
					pParticleVisualObject->SetDestBlend(D3DBLEND_SRCCOLOR);
					break;

				case 2:
					pParticleVisualObject->SetDestBlend(D3DBLEND_INVSRCALPHA);
					break;

				case 3:
					pParticleVisualObject->SetDestBlend(D3DBLEND_INVSRCCOLOR);
					break;

				case 4:
					pParticleVisualObject->SetDestBlend(D3DBLEND_DESTALPHA);
					break;

				case 5:
					pParticleVisualObject->SetDestBlend(D3DBLEND_DESTCOLOR);
					break;

				case 6:
					pParticleVisualObject->SetDestBlend(D3DBLEND_INVDESTALPHA);
					break;

				case 7:
					pParticleVisualObject->SetDestBlend(D3DBLEND_INVDESTCOLOR);
					break;

				case 8:
					pParticleVisualObject->SetDestBlend(D3DBLEND_ONE);
					break;
				}
			}
			break;
		}
		break;

	case WM_CLOSE:
		pParticleVisualObject = NULL;
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		EndDialog(hWnd, TRUE);
		break;
	}

	return FALSE;
}

BOOL APIENTRY  ColorDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COLORREF color, emissiveColor;
	static int type = 0;

	switch(uMsg) {
    case WM_INITDIALOG:
		{
			type = *((int *) lParam);
			SetWindowText( GetDlgItem(hWnd, IDC_EDIT_ALPHA), "255" );
		}
		break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDOK:
			{
				float alpha;
				char buf[64];

				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_ALPHA), buf, 63);
				alpha = (float)  atof(buf) / 255.0f;

				if ( type == 0 )
				{
					g_Particle.SetColorStart( alpha, 
										(float) GetRValue(color) / 255.0f,
										(float) GetGValue(color) / 255.0f,
										(float) GetBValue(color) / 255.0f ); 
				}
				else
				{
					g_Particle.SetColorEnd( alpha, 
										(float) GetRValue(color) / 255.0f,
										(float) GetGValue(color) / 255.0f,
										(float) GetBValue(color) / 255.0f );
				}

				// TODO : emissive color

				EndDialog(hWnd, TRUE);
			}
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;

		case IDC_BUTTON_COLOR:
			{
				COLORREF acrCustClr[16];
				CHOOSECOLOR choosecolor;
				ZeroMemory(&choosecolor, sizeof(CHOOSECOLOR));

				choosecolor.Flags = CC_FULLOPEN;
				choosecolor.hwndOwner = g_hWnd;
				choosecolor.lpCustColors = (LPDWORD) acrCustClr;
				choosecolor.lStructSize = sizeof(CHOOSECOLOR);

				if ( ChooseColor(&choosecolor) )
				{
					color = choosecolor.rgbResult; 
				}
			}
			break;

		case IDC_BUTTON_EMISSIVE_COLOR:
			{
				COLORREF acrCustClr[16];
				CHOOSECOLOR choosecolor;
				ZeroMemory(&choosecolor, sizeof(CHOOSECOLOR));

				choosecolor.Flags = CC_FULLOPEN;
				choosecolor.hwndOwner = g_hWnd;
				choosecolor.lpCustColors = (LPDWORD) acrCustClr;
				choosecolor.lStructSize = sizeof(CHOOSECOLOR);

				if ( ChooseColor(&choosecolor) )
				{
					emissiveColor = choosecolor.rgbResult; 
				}
			}
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		EndDialog(hWnd, TRUE);
		break;
	}

	return FALSE;
}

void FillMainDialogItem()
{
	char buf[64];
	unsigned int a, b;
	float c, d;
	D3DXVECTOR3 vect;

	sprintf_s( buf, 64, "%d", g_Particle.GetLife() );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_LIFE), buf );
	
	SendMessage( GetDlgItem(g_hWnd, IDC_CHECK_CONTINOUS), BM_SETCHECK, g_Particle.GetContinuous() ? BST_CHECKED : BST_UNCHECKED, 0 );

	sprintf_s( buf, 64, "%d", g_Particle.GetEmissionPerSec() );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_EMISSION), buf );

	sprintf_s( buf, 64, "%d", g_Particle.GetParticleMax() );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_PARTICLE_MAX), buf );

	g_Particle.GetParticleLife( &a, &b );
	sprintf_s( buf, 64, "%d", a );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_LIFE_MIN), buf );
	sprintf_s( buf, 64, "%d", b );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_LIFE_MAX), buf );

	g_Particle.GetSpeed( &c, &d );
	sprintf_s( buf, 64, "%f", c );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SPEED_MIN), buf );
	sprintf_s( buf, 64, "%f", d );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SPEED_MAX), buf );

	g_Particle.GetSize( &c, &d );
	sprintf_s( buf, 64, "%f", c );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SIZE_MIN), buf );
	sprintf_s( buf, 64, "%f", d );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SIZE_MAX), buf );

	vect = g_Particle.GetSpread();
	sprintf_s( buf, 64, "%f", D3DXToDegree( vect.x ) );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SPREAD_X), buf );
	sprintf_s( buf, 64, "%f", D3DXToDegree( vect.y ) );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SPREAD_Y), buf );
	sprintf_s( buf, 64, "%f", D3DXToDegree ( vect.z ) );
	SetWindowText( GetDlgItem(g_hWnd, IDC_EDIT_SPREAD_Z), buf );
}
