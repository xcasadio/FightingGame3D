#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "GameCore.h"
#include "Graphic.h"
#include "Light.h"
#include "Camera.h"
#include "Font.h"
#include "MeshX.h"
#include "Pad.h"
#include "MediaPathManager.h"


using namespace GameEngine;

//variable globale
cLight			g_Light;
POINT			g_MousePosition;
HWND			g_hWnd;
HINSTANCE		g_hInst;
HANDLE			g_hThread;
OPENFILENAME	g_ofn;
char			g_FileName[MAX_PATH];
cMeshX			g_mesh;
cBoundingBox	g_BoundingBox;
cBoundingSphere	g_BoundingSphere;
bool			g_MeshIsLoaded = false;
cWorldPosition	g_WorldPos_BoundingBox, g_WorldPos_BoundingSphere, g_WorldPos;

cCameraModelViewer g_cameraModelViewer;

bool g_bDisplayBoundingBox = false;
bool g_bDisplayBoundingSphere = false;

cMeshX g_MeshBoundingBox, g_MeshBoundingSphere;

bool g_DrawTexture = true, g_DrawWhiteMat = false;
D3DMATERIAL9 g_MaterialWhite = {	1.0f, 1.0f, 1.0f, 1.0f, 
									1.0f, 1.0f, 1.0f, 1.0f, 
									1.0f, 1.0f, 1.0f, 1.0f, 
									0.0f, 0.0f, 0.0f, 0.0f, 
									0.0f };

BOOL CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI MyThreadFunction(LPVOID lpParam);

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	g_hInst = hInst;

	if ( cGameCore::Instance().GetGraphic().Init() == false)
		return -1;

	cMediaPathManager::Instance().AddPath( "./" );

	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, MainDialogProc);

	cGameCore::Instance().GetGraphic().Shutdown();

	cGameCore::Destroy();
	cMediaPathManager::Destroy();

	return 0;
}

BOOL CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	cGraphic &graphic = cGameCore::Instance().GetGraphic();

	switch(uMsg) {
    case WM_INITDIALOG:
		
		if ( graphic.SetMode(GetDlgItem(hWnd, IDC_PREVIEW), true, true) == false )
			return -1;

		if ( !cGameCore::Instance().CreateInputDevice( hWnd, g_hInst, true ) )
			return -1;

		for ( int i = 0; i < MAX_PAD; i++ )
		{
			if ( !cGameCore::Instance().GetPad(i).Load("pad.xml") )
				MessageBox(NULL, "Erreur pad load()", "Erreur", MB_OK);
		}

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

		if ( g_MeshBoundingBox.Load("cube.x") == false )
		{
			cWindow::Error( false, "Impossible d'ouvrir cube.x");
		}

		if ( g_MeshBoundingSphere.Load("sphere_radius1_segment32.X") == false )
		{
			cWindow::Error( false, "Impossible d'ouvrir sphere_radius1_segment32.X");
		}

		SendMessage( GetDlgItem(hWnd, IDC_COMBODISPLAYMODE),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"Solid") );

		SendMessage( GetDlgItem(hWnd, IDC_COMBODISPLAYMODE),
                        CB_ADDSTRING,
                        0,
                        reinterpret_cast<LPARAM>((LPCTSTR)"Fil de fer") );

		SendMessage( GetDlgItem(hWnd, IDC_COMBODISPLAYMODE), CB_SETCURSEL, 0, 0);

		SendMessage( GetDlgItem(hWnd, IDC_CHECKDISPLAYTEXTURE), BM_SETCHECK, BST_CHECKED, 0);

		// Initialize the save/load dialog box info
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

		return TRUE;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case ID_FICHIER_OUVRIR:	
			{
				g_ofn.hwndOwner   = hWnd;
				g_ofn.lpstrFile   = g_FileName;
				g_ofn.lpstrTitle  = "Load Mexh File";
				g_ofn.lpstrFilter = "Direct X Files (*.x)\0*.x\0All Files (*.*)\0*.*\0\0";
				g_ofn.lpstrDefExt = "x";

				if(!GetOpenFileName(&g_ofn))
					return FALSE;

				SuspendThread(g_hThread);

				if ( (g_MeshIsLoaded = g_mesh.Load(g_ofn.lpstrFile)) == false)
				{
					ResumeThread(g_hThread);
					return FALSE;
				}

				cBoundingForm *pBounding = &g_BoundingBox;
				pBounding->Compute(g_mesh);

				pBounding = &g_BoundingSphere;
				pBounding->Compute(g_mesh);

				//TODO calcul camera
				float length = g_BoundingBox.GetMax().x - g_BoundingBox.GetMin().x;
				length = length > (g_BoundingBox.GetMax().y - g_BoundingBox.GetMin().y) ? length : (g_BoundingBox.GetMax().y - g_BoundingBox.GetMin().y);
				g_cameraModelViewer.SetRadius( (length * 2.0f) / tan(graphic.GetFieldOfView()) );

				//TODO : utiliser D3DXMatrixTransform
				g_WorldPos_BoundingBox.Move( (g_BoundingBox.GetMax().x - g_BoundingBox.GetMin().x) / 2.0f,
												(g_BoundingBox.GetMax().y - g_BoundingBox.GetMin().y) / 2.0f,
												(g_BoundingBox.GetMax().z - g_BoundingBox.GetMin().z) / 2.0f );

				g_WorldPos_BoundingBox.Scale( (g_BoundingBox.GetMax().x - g_BoundingBox.GetMin().x),
												(g_BoundingBox.GetMax().y - g_BoundingBox.GetMin().y),
												(g_BoundingBox.GetMax().z - g_BoundingBox.GetMin().z) );

				g_WorldPos_BoundingSphere.Move( g_BoundingSphere.GetCenter().x,
												g_BoundingSphere.GetCenter().y,
												g_BoundingSphere.GetCenter().z);
				
				g_WorldPos_BoundingSphere.Scale( g_BoundingSphere.GetRadius(),
												g_BoundingSphere.GetRadius(),
												g_BoundingSphere.GetRadius());
				
				ResumeThread(g_hThread);
			}
			break;

		case ID_FICHIER_FERMER:
			{
				if ( g_hThread )
				{
					CloseHandle(g_hThread);
				}

				g_hThread = NULL;

				EndDialog(hWnd, TRUE);
			}
			break;

		case IDC_COMBODISPLAYMODE:	
			{
				int sel = SendMessage(GetDlgItem(hWnd, IDC_COMBODISPLAYMODE), CB_GETCURSEL, 0, 0);

				if ( sel == 0 )
				{
					cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				}
				else if ( sel == 1 )
				{
					cGameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				}
			}
			break;

		case IDC_CHECKDISPLAYTEXTURE:	
			{
				if ( SendMessage(GetDlgItem(hWnd, IDC_CHECKDISPLAYTEXTURE), BM_GETCHECK, 0, 0) == TRUE)
				{
					g_DrawTexture = true;
					SendMessage( GetDlgItem(hWnd, IDC_CHECKWHITEMATERIAL), BM_SETCHECK, BST_UNCHECKED, 0);
					g_DrawWhiteMat = false;
				}
				else
				{
					g_DrawTexture = false;
				}
			}
			break;

		case IDC_CHECKWHITEMATERIAL:	
			{
				if ( SendMessage(GetDlgItem(hWnd, IDC_CHECKWHITEMATERIAL), BM_GETCHECK, 0, 0) == TRUE)
				{
					g_DrawWhiteMat = true;
					SendMessage( GetDlgItem(hWnd, IDC_CHECKDISPLAYTEXTURE), BM_SETCHECK, BST_UNCHECKED, 0);
					g_DrawTexture = false;
				}
				else
				{
					g_DrawWhiteMat = false;
				}
			}
			break;

		case IDC_CHECKDISPLAYBOUNDINGBOX:
			g_bDisplayBoundingBox = SendMessage(GetDlgItem(hWnd, IDC_CHECKDISPLAYBOUNDINGBOX), BM_GETCHECK, 0, 0) ? true : false;
			break;

		case IDC_CHECKDISPLAYBOUNDINGSPHERE:
			g_bDisplayBoundingSphere = SendMessage(GetDlgItem(hWnd, IDC_CHECKDISPLAYBOUNDINGSPHERE), BM_GETCHECK, 0, 0) ? true : false;
			break;

		}
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

	return FALSE;
}


DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
	cGraphic *graphic = &cGameCore::Instance().GetGraphic();

	cFontDX font;
	font.Create("Courrier New", 10);
	char buf[64];

	cPad *pad = &cGameCore::Instance().GetPad(0);

	while(1)
	{
		graphic->Clear(0xff000000);
		if ( graphic->BeginScene() )
		{
			graphic->SetCamera(&g_cameraModelViewer);
			graphic->SetWorldPosition(&g_WorldPos);

			if ( g_MeshIsLoaded )
			{
				g_mesh.Render( g_DrawWhiteMat ? &g_MaterialWhite : NULL, g_DrawTexture);
			}
			
			sprintf_s(buf, 64, "%d - %d", pad->MouseX(), pad->MouseY());
			font.Print( buf, 0, 0, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 255) );

			if ( g_bDisplayBoundingBox )
			{
				graphic->SetWorldPosition(&g_WorldPos_BoundingBox);
				g_MeshBoundingBox.Render();
			}

			if ( g_bDisplayBoundingSphere )
			{
				graphic->SetWorldPosition(&g_WorldPos_BoundingSphere);
				g_MeshBoundingSphere.Render();
			}

			graphic->EndScene();
		}

		graphic->Display();

		cGameCore::Instance().ReadAllInputDevice();

		if ( g_mesh.IsLoaded() )
		{
			if ( ( pad->GetMouseMotionX() || pad->GetMouseMotionY() )
				&& pad->MouseX() > 11 && pad->MouseX() < 520
				&& pad->MouseY() > 11 && pad->MouseY() < 564
				&& pad->MouseButton( MOUSE_LBUTTON )
				)
			{
				g_cameraModelViewer.Rotate( D3DXToRadian( (float) pad->GetMouseMotionY() / 5.0f), D3DXToRadian( (float) -pad->GetMouseMotionX() / 5.0f), 0.0f );
			}

			if ( pad->MouseWheel() != 0 )
			{
				pad->MouseWheel() > 0 ? g_cameraModelViewer.Forward() : g_cameraModelViewer.Backward();
			}
		}

		graphic->EndFrame();

		Sleep(17);
    }

    ExitThread(0); 
} 


