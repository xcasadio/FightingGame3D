#include "main.h"
#include "Editor.h"
/*
#include <tchar.h>
#include <commctrl.h>
*/
#include <string>

#include "MediaPathManager.h"

#include "DebugEngine/DebugNew.h"

//===================================================================================================
//===================================================================================================
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	g_hInst = hInst;

	if ( GameCore::Instance().GetGraphic().Init() == false)
		return -1;

	g_cameraModelViewer.SetRadius(30.0f);

	char buf[512];
	GetCurrentDirectory( sizeof(buf), buf );
	MediaPathManager::Instance().AddPath( buf );

	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG1), NULL, MainDialogProc);

	GameCore::Instance().GetGraphic().Shutdown();
	MediaPathManager::Destroy();
	GameCore::Destroy();

	return 0;
}

//===================================================================================================
//===================================================================================================
BOOL CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Graphic &graphic = GameCore::Instance().GetGraphic();

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		// Initialize the save/load dialog box info
		ZeroMemory(&g_ofn, sizeof(OPENFILENAME));
		g_ofn.lStructSize = sizeof(OPENFILENAME);
		g_ofn.nMaxFile = MAX_PATH;
		g_ofn.nMaxFileTitle = MAX_PATH;
		g_ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;

		g_hWnd = hWnd;

		return CharacterEditor::MsgInitTabControlDialog(g_hWnd);
		break;

	case WM_NOTIFY :
	{
		// mise à jour de l'affichage des dialogues enfants
		LPNMHDR lpnmhdr = (LPNMHDR) lParam;
		if( lpnmhdr->code == TCN_SELCHANGE )
		{
			CharacterEditor::UpdateChildTab(g_hWnd);
		}
		return 0;
	}

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case ID_FICHIER_NOUVEAU:
			if ( g_hThread && g_bThreadRunning)
			{
				g_bThreadEnd = true;

				if ( WAIT_FAILED == WaitForSingleObject( g_hThread, INFINITE ) )
				{
					exit( -1 );
				}

				g_bThreadRunning = false;
				g_hThread = NULL;
			}

			g_Charac.Free();
			CharacterEditor::ClearCharacterGadget();

			g_FileName.clear();

			g_bThreadEnd = false;

			g_hThread = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			GraphicThreadFunction,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			NULL);   // returns the thread identifier 

			g_bThreadRunning = true;

			if (g_hThread == NULL) 
			{
				exit(-1);
			}
			break;

		case ID_FICHIER_ENREGISTRERSOUS:
			g_ofn.hwndOwner   = hWnd;
			g_ofn.lpstrFile   = g_FileNameBuf;
			g_ofn.lpstrTitle  = "Save character File";
			g_ofn.lpstrFilter = "character files (*.charac)\0*.charac\0All Files (*.*)\0*.*\0\0";
			g_ofn.lpstrDefExt = "charac";

			if(!GetSaveFileName(&g_ofn))
				return FALSE;

			if ( g_Charac.SaveSerialized( g_ofn.lpstrFile ) == false )
			{
				char buf[512];
				sprintf_s(buf, 512, "Impossible de sauvegarder %s", g_ofn.lpstrFile);
				MessageBox(hWnd, buf, "Erreur", MB_OK);
			}

			g_FileName.clear();
			g_FileName.append( g_ofn.lpstrFile );

			break;
			
			break;

		case ID_FICHIER_ENREGISTRER:
			if ( g_FileName.size() > 0 )
			{
				if ( g_Charac.SaveSerialized( g_FileName.c_str() ) == false )
				{
					char buf[512];
					sprintf_s(buf, 512, "Impossible de sauvegarder %s", g_FileName.c_str() );
					MessageBox(hWnd, buf, "Erreur", MB_OK);
				}
			}
			else
			{
				g_ofn.hwndOwner   = hWnd;
				g_ofn.lpstrFile   = g_FileNameBuf;
				g_ofn.lpstrTitle  = "Save character File";
				g_ofn.lpstrFilter = "character files (*.charac)\0*.charac\0All Files (*.*)\0*.*\0\0";
				g_ofn.lpstrDefExt = "charac";

				if(!GetSaveFileName(&g_ofn))
					return FALSE;

				if ( g_Charac.SaveSerialized( g_ofn.lpstrFile ) == false )
				{
					char buf[512];
					sprintf_s(buf, 512, "Impossible de sauvegarder %s", g_ofn.lpstrFile);
					MessageBox(hWnd, buf, "Erreur", MB_OK);
				}

				g_FileName.clear();
				g_FileName.append( g_ofn.lpstrFile );
			}

			break;

		case ID_FICHIER_CHARGER:
			g_ofn.hwndOwner   = hWnd;
			g_ofn.lpstrFile   = g_FileNameBuf;
			g_ofn.lpstrTitle  = "Load character File";
			g_ofn.lpstrFilter = "character files (*.charac)\0*.charac\0All Files (*.*)\0*.*\0\0";
			g_ofn.lpstrDefExt = "charac";

			if(!GetOpenFileName(&g_ofn))
				return FALSE;

			if ( g_hThread && g_bThreadRunning)
			{
				g_bThreadEnd = true;

				if ( WAIT_FAILED == WaitForSingleObject( g_hThread, INFINITE ) )
				{
					exit( -1 );
				}

				g_bThreadRunning = false;
				g_hThread = NULL;
			}

			g_Charac.Free();
			g_Charac.SetCommandType( COMMAND_TYPE_PLAYER );
			g_Charac.InitCommunParameters();

			if ( g_Charac.LoadSerialized( g_ofn.lpstrFile ) == false )
			{
				char buf[512];
				sprintf_s(buf, 512, "Impossible d'ouvrir %s", g_ofn.lpstrFile );
				MessageBox(hWnd, buf, "Erreur", MB_OK);
			}
			else
			{
				CharacterEditor::InitCharacterGadget();

				g_cameraModelViewer.SetRadius( g_Charac.GetBoundingSphere().GetRadius() * 2.0f );

				g_bThreadEnd = false;

				g_hThread = CreateThread( 
				NULL,                   // default security attributes
				0,                      // use default stack size  
				GraphicThreadFunction,       // thread function name
				NULL,          // argument to thread function 
				0,                      // use default creation flags 
				NULL);   // returns the thread identifier 

				g_bThreadRunning = true;

				if (g_hThread == NULL) 
				{
					exit(-1);
				}

				g_FileName.clear();
				g_FileName.append( g_ofn.lpstrFile );
			}

			break;

		case ID_EDIT_COULEURBACKGROUND:
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

		case ID_FICHIER_QUITTER:
			PostQuitMessage(0);
			break;

		} //  WM_COMMAND switch( LOWORD(wParam) )
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		if ( g_hThread && g_bThreadRunning)
		{
			g_bThreadEnd = true;
			g_bThreadRunning = false;

			if ( WAIT_FAILED == WaitForSingleObject( g_hThread, INFINITE ) )
			{
				exit( -1 );
			}

			g_hThread = NULL;
		}

		for( int i = 0; i < MAX_CHILD_TAB; i++ )
		{
			DestroyWindow(g_aChildTab[i]);
		}
		
		break;
	}

	return FALSE;
}

//===================================================================================================
//===================================================================================================
DWORD WINAPI GraphicThreadFunction( LPVOID lpParam ) 
{ 
	Graphic *graphic = &GameCore::Instance().GetGraphic();

	graphic->SetAmbientLight(255, 255, 255);
	graphic->EnableLighting(true);
	graphic->EnableZBuffer(true);

	cLight light, light2, light3;
	light.SetAmbientColor(255, 255, 255);
	light.SetDiffuseColor(255, 255, 255);
	light.SetSpecularColor(255, 255, 255);
	light.SetFalloff(1.0f);
	light.SetType(D3DLIGHT_POINT);
	light.SetRange(10000.0f);
	light.SetPhi(1.0f);
	light.Point(50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	light2.SetAmbientColor(255, 255, 255);
	light2.SetDiffuseColor(255, 255, 255);
	light2.SetSpecularColor(255, 255, 255);
	light2.SetFalloff(1.0f);
	light2.SetType(D3DLIGHT_POINT);
	light2.SetRange(10000.0f);
	light2.SetPhi(1.0f);
	light2.Point(-50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	light3.SetAmbientColor(255, 255, 255);
	light3.SetDiffuseColor(255, 255, 255);
	light3.SetSpecularColor(255, 255, 255);
	light3.SetFalloff(1.0f);
	light3.SetType(D3DLIGHT_POINT);
	light3.SetRange(10000.0f);
	light3.SetPhi(1.0f);
	light3.Point(0.0f, 0.0f, -50.0f, 0.0f, 0.0f, 0.0f);

	graphic->SetLight( 0, &light);
	graphic->SetLight( 1, &light2);
	graphic->SetLight( 2, &light3);
	graphic->EnableLight(0);
	graphic->EnableLight(1);
	graphic->EnableLight(2);
	
	D3DXMatrixIdentity( &g_MatWorld);

	//FontDX font;
	//font.Create("Courrier New", 10);
	//char buf[64];

	MeshX meshX;
	if ( meshX.Load( MediaPathManager::Instance().FindMedia( "cube.x" ).c_str() ) == false )
	{
		MessageBox( g_hWnd, "Impossible de charger cube.x", "Erreur", MB_OK | MB_ICONEXCLAMATION );
	}

	D3DMATERIAL9 matRed, matBlue;

	ZeroMemory( &matRed, sizeof(matRed) );
	ZeroMemory( &matBlue, sizeof(matBlue) );

	matRed.Ambient.a = matRed.Diffuse.a = 1.0f;
	matRed.Ambient.r = matRed.Diffuse.r = 1.0f;
	matRed.Ambient.g = matRed.Diffuse.g = 0.0f;
	matRed.Ambient.b = matRed.Diffuse.b = 0.0f;

	matBlue.Ambient.a = matBlue.Diffuse.a = 1.0f;
	matBlue.Ambient.r = matBlue.Diffuse.r = 0.0f;
	matBlue.Ambient.g = matBlue.Diffuse.g = 0.0f;
	matBlue.Ambient.b = matBlue.Diffuse.b = 1.0f;

	while( g_bThreadEnd == false )
	{
		graphic->BeginFrame();

		graphic->Clear(g_BackgroundColor);

		if ( graphic->BeginScene() )
		{
			graphic->SetCamera(&g_cameraModelViewer);

			//sprintf_s(buf, 64, "%d - %d", g_Pad.MouseX(), g_Pad.MouseY());
			//font.Print( buf, 0, 0, 0, 0, D3DCOLOR_ARGB(255, 255, 255, 255) );

			if ( g_Charac.GetMesh()->IsLoaded() )
			{
				g_Charac.GetMesh()->Draw();
			}

			//selon le stateDef
			//+ calcul matrice
			if ( meshX.IsLoaded() && g_pStateDef )
			{
				if ( g_bShowOBBAttack )
				{
					for ( unsigned int i = 0; i < g_pStateDef->GetCollisionAtt().size(); i++ )
					{
						graphic->GetDeviceCOM()->SetTransform( D3DTS_WORLD, &g_pStateDef->GetCollisionAtt().at(i).GetMatrixForDisplay() );
						meshX.Render( &matRed, false );
					}
				}

				if ( g_bShowOBBDefense )
				{
					for ( unsigned int i = 0; i < g_pStateDef->GetCollisionDef().size(); i++ )
					{
						graphic->GetDeviceCOM()->SetTransform( D3DTS_WORLD, &g_pStateDef->GetCollisionDef().at(i).GetMatrixForDisplay() );
						meshX.Render( &matBlue, false );
					}
				}
			}

			graphic->EndScene();
		}

		graphic->Display();

		float currentTime = graphic->FrameTime() / 1000.0f;
		
		if ( g_bPlayingAnimation && g_Charac.GetMesh()->IsLoaded() )
		{
			if ( g_Charac.GetMesh()->Update( currentTime, &g_MatWorld ) )
			{
				g_bPlayingAnimation = false;
			}
		}

		GameCore::Instance().ReadAllInputDevice();
		//g_Pad.Read();

		if ( g_Charac.GetMesh()->IsLoaded() 
			&& g_Pad.MouseX() > 6 && g_Pad.MouseX() < 518
			&& g_Pad.MouseY() > 41 && g_Pad.MouseY() < 544 )
		{
			if ( ( g_Pad.GetMouseMotionX() || g_Pad.GetMouseMotionY() ) )
			{
				if ( g_Pad.MouseButton( MOUSE_LBUTTON ) )
				{
					g_cameraModelViewer.Rotate( D3DXToRadian( (float) g_Pad.GetMouseMotionY() / 5.0f), D3DXToRadian( (float) -g_Pad.GetMouseMotionX() / 5.0f), 0.0f );
				}

				if (  g_Pad.MouseButton( MOUSE_MBUTTON) )
				{
					g_cameraModelViewer.Move( (float) g_Pad.GetMouseMotionY(), (float) g_Pad.GetMouseMotionX(), 0.0f );
				}
			}

			if ( g_Pad.MouseWheel() != 0 )
			{
				g_Pad.MouseWheel() > 0 ? g_cameraModelViewer.Forward() : g_cameraModelViewer.Backward();
			}
		}

		if ( g_bPlayingAnimation == false )
		{
			Sleep(25);
		}

		graphic->EndFrame();
    }

	meshX.Free();

    ExitThread(0); 
} 

//===================================================================================================
//===================================================================================================
BOOL APIENTRY  AddStateDefDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static StateDef *s_pStateDef = NULL;
	switch(uMsg)
	{
   case WM_INITDIALOG:
		{
			if ( lParam )
			{
				s_pStateDef = (StateDef *) lParam;
				char buf[128];

				sprintf_s( buf, sizeof(buf), "%s", s_pStateDef->GetDescription() );
				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATEDEF_DESCRIPTION ), buf );

				sprintf_s( buf, sizeof(buf), "%d", g_StateDefId );
				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATEDEF_NUMERO ), buf );
			}
			else
			{
				s_pStateDef = NULL;
			}
		}
		break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDOK:
			{
				int num = 0;
				char buf[128];
				ZeroMemory(buf, sizeof(char) * 128);

				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_STATEDEF_NUMERO), buf, 127);

				if ( buf[0] == 0 )
				{
					MessageBox(hWnd, "Le stateDef doit avoir un numero", "Erreur", MB_OK);
				}
				else
				{
					char tmp[128];

					num = atoi(buf);

					ZeroMemory(buf, sizeof(char) * 128);
					GetWindowText( GetDlgItem(hWnd, IDC_EDIT_STATEDEF_DESCRIPTION), buf, 127);

					if ( s_pStateDef )
					{
						s_pStateDef->SetDescription( buf );

						if ( num != g_StateDefId )
						{
							StateDef stateDef( *s_pStateDef );

							CharacterEditor::DeleteStateDef( g_StateDefId );

							int sel = ComboBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ) );

							if ( sel == CB_ERR )
							{
								MessageBox( NULL, "Erreur selectionne IDC_COMBO_STATE_DEF", "Erreur", MB_OK );
								exit( -1 );
							}

							ComboBox_DeleteString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ), sel );
							sprintf_s( tmp, 128, "%d %s", num, buf);
							ComboBox_AddString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ), tmp );

							ComboBox_SetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ), 
								ComboBox_GetCount( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ) ) - 1 );

							g_StateDefId = num;

							g_Charac.GetStateDef()[g_StateDefId] = stateDef;
						}					
					}
					else
					{
						sprintf_s( tmp, 128, "%d %s", num, buf);					
					
						ComboBox_AddString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), tmp );

						int nbElt = SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_GETCOUNT, 0, 0);

						if ( nbElt != CB_ERR )
						{
							SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_SETCURSEL, nbElt - 1, 0);
						}

						StateDef stateDef;
						stateDef.SetDescription( buf );

						g_Charac.GetStateDef()[num] = stateDef;

						g_StateDefId = num;
						g_pStateDef = &g_Charac.GetStateDef()[num];
					}

					EndDialog(hWnd, TRUE);
				}
			}
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}

	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL APIENTRY  AddStateControllerDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// pour savoir si on fait un new sur le g_pController
	// identifie le mode add et edit
	static bool s_bControllerEdit = false;

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		{
			s_bControllerEdit = false;

			if ( lParam != NULL )
			{
				s_bControllerEdit = true;
				g_pController = (StateController *) lParam;

				g_value.type = g_pController->GetValue().type;

				switch ( g_value.type )
				{
				case STATE_CONTROLLER_VALUE_TYPE_FLOAT:
					g_value.real = g_pController->GetValue().real;
					break;

				case STATE_CONTROLLER_VALUE_TYPE_VEC3:
					g_value.vec3 = g_pController->GetValue().vec3;
					break;
				}
			}

			CharacterEditor::InitStateControllerDialogGadget( hWnd, (StateController *) lParam );
		}
		break;

	case WM_COMMAND:
		
		switch ( LOWORD(wParam) )
		{
		case IDOK:
			{
				if ( g_pController->GetTriggers().size() == 0 )
				{
					MessageBox( hWnd, "Vous devez créer au moins créer 1 Triggers", "Erreur", MB_OK);
					return FALSE;
				}

				int typeSelected = SendMessage( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_TYPE), CB_GETCURSEL, 0, 0 );

				if ( typeSelected == CB_ERR )
				{
					MessageBox( hWnd, "Type non selectionné", "Erreur", MB_OK);
					return FALSE;
				}

				char buf[64];

				//nom
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_NAME), buf, 63);
				g_pController->SetName( buf );

				//type
				g_pController->SetType( CharacterEditor::GetControllerTypeFromCBSelected(typeSelected) );

				//hitSound
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_HIT_SOUND), buf, 63);
				g_pController->SetHitSound( atoi(buf) );

				//GuardSound
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_GUARD_SOUND), buf, 63);
				g_pController->SetGuardSound( atoi(buf) );

				//value
				g_pController->SetValue( g_value );

				//attr1

				//attr2

				//damage
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_DAMAGE), buf, 63);
				g_pController->SetDamage( atoi(buf) );

				//guard damage
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_GUARD_DAMAGE), buf, 63);
				g_pController->SetGuardDamage( atoi(buf) );

				//hitHeight
				g_pController->SetHitHeight( (eHitHeight) ComboBox_GetCurSel( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_HIT_HEIGHT ) ) );

				//guardFlag

				//pauseTime1
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_PAUSE_TIME_P1), buf, 63);
				g_pController->SetPauseTimeP1( atoi(buf) );

				//pauseTime2
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_PAUSE_TIME_P2), buf, 63);
				g_pController->SetPauseTimeP2( atoi(buf) );

				//sparkNo
				GetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPARK_NO), buf, 63);
				g_pController->SetSparkNo( atoi(buf) );

				//fin
				if ( s_bControllerEdit == false )
				{
					g_pStateDef->AddStateController(*g_pController);
					g_bStateControllerAdded = true;
				}
				else
				{
					g_bStateControllerEdited = true;
				}
			}
			EndDialog(hWnd, TRUE);
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;

		case IDC_BUTTON_ADD_TRIGGER:
			if ( DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_TRIGGER), hWnd, TriggerDialogProc) <= 0)
			{
				MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_TRIGGER", "Erreur", MB_OK);
			}
			else
			{
				if ( g_pController == NULL )
				{
					MessageBox(hWnd, "IDC_BUTTON_ADD_TRIGGER : g_pController == NULL", "Erreur", MB_OK);
				}
				else
				{
					if ( g_bTriggerAdded )
					{
						SendMessage( GetDlgItem(hWnd, IDC_LIST_TRIGGER),
							LB_ADDSTRING,
							0,
							reinterpret_cast<LPARAM> ( g_pController->GetTriggers().back().ToString().c_str() ) );

						g_bTriggerAdded = false;
					}
				}
			}
			break;
		
		case IDC_BUTTON_EDIT_TRIGGER:
			{
				int sel = ListBox_GetCurSel( GetDlgItem( hWnd, IDC_LIST_TRIGGER) );

				if ( sel != LB_ERR )
				{
					Trigger *trigger = &g_pController->GetTriggers().at(sel);

					if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_TRIGGER), hWnd, TriggerDialogProc, (LPARAM) trigger ) <= 0)
					{
						MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_TRIGGER", "Erreur", MB_OK);
					}
					else if ( g_bTriggerEdited )
					{
						ListBox_ResetContent( GetDlgItem( hWnd, IDC_LIST_TRIGGER) );
						CharacterEditor::FillListBoxTriggerName( GetDlgItem( hWnd, IDC_LIST_TRIGGER), g_pController->GetTriggers() );
						/*
						if ( ListBox_SetItemData( GetDlgItem( hWnd, IDC_LIST_TRIGGER), sel, (LPARAM) g_pController->GetTriggers().at(sel).GetExpression().c_str() ) == LB_ERR )
						{
							MessageBox(hWnd, "Impossible de changer le nom de l'item ListBox Trigger", "Erreur", MB_OK );
						}
						*/

						g_bTriggerEdited = false;
					}
				}				
			}
			break;

		case IDC_BUTTON_DELETE_TRIGGER:
			{
				int sel = ListBox_GetCurSel( GetDlgItem( hWnd, IDC_LIST_TRIGGER) );

				if ( sel != LB_ERR )
				{
					g_pController->GetTriggers().erase( g_pController->GetTriggers().begin() + sel );
					ListBox_DeleteString( GetDlgItem( hWnd, IDC_LIST_TRIGGER), sel );
				}
			}
			break;

		case IDC_BUTTON_VALUE:
			{
				if ( g_pController == NULL )
				{
					MessageBox( hWnd, "g_pController == NULL", "Erreur", MB_OK );
					break;
				}

				g_value.type = g_pController->GetValue().type;
				switch ( g_value.type )
				{
				case STATE_CONTROLLER_VALUE_TYPE_FLOAT:
					g_value.real = g_pController->GetValue().real;
					break;

				case STATE_CONTROLLER_VALUE_TYPE_VEC3:
					g_value.vec3 = g_pController->GetValue().vec3;
					break;
				}

				if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_STATE_CONTROLLER_VALUE), hWnd, EditValueStateControllerDialogProc, NULL ) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_STATE_CONTROLLER_VALUE", "Erreur", MB_OK);
				}
				else if ( g_bValueChanged )
				{
					g_bValueChanged = false;

					char buf[64];

					switch ( g_value.type )
					{
					case STATE_CONTROLLER_VALUE_TYPE_FLOAT:
						sprintf_s( buf, 64, "%f", g_value.real );
						break;

					case STATE_CONTROLLER_VALUE_TYPE_VEC3:
						sprintf_s( buf, 64, "%f %f %f", g_value.vec3.x, g_value.vec3.y, g_value.vec3.z );
						break;
					}

					g_pController->SetValue( g_value );

					SetWindowText( GetDlgItem( hWnd, IDC_TEXT_VALUE), buf );
				}
			}
			break;

		case IDC_BUTTON_SPARK_POSITION:
			{
				D3DXVECTOR3 vec = g_pController->GetSparkPosition();

				if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDIT_VECTOR), hWnd, EditVectorDialogProc, (LPARAM) &vec ) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_EDIT_VECTOR", "Erreur", MB_OK);
				}
				else if ( g_bVectorEdited )
				{
					g_bVectorEdited = false;
					g_pController->SetSparkPosition( vec );
					
					char buf[128];
					sprintf_s( buf, sizeof(buf), "%f %f %f", vec.x, vec.y, vec.z );
					SetWindowText( GetDlgItem( hWnd, IDC_TEXT_SPARK_POSITION ), buf );
				}
			}
			break;

		case IDC_BUTTON_SPARK_DIRECTION:
			{
				D3DXVECTOR3 vec = g_pController->GetSparkDirection();

				if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDIT_VECTOR), hWnd, EditVectorDialogProc, (LPARAM) &vec ) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_EDIT_VECTOR", "Erreur", MB_OK);
				}
				else if ( g_bVectorEdited )
				{
					g_bVectorEdited = false;
					g_pController->SetSparkDirection( vec );

					char buf[128];
					sprintf_s( buf, sizeof(buf), "%f %f %f", vec.x, vec.y, vec.z );
					SetWindowText( GetDlgItem( hWnd, IDC_TEXT_SPARK_DIRECTION ), buf );
				}
			}
			break;

		} // WM_COMMAND switch ( LOWORD(wParam) )
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		if ( s_bControllerEdit == false )
		{
			delete g_pController;
		}

		g_pController = NULL;
		break;

	}
	
	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL APIENTRY TriggerDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// pour savoir si on fait un new sur le g_pController
	// identifie le mode add et edit
	static bool s_bTriggerEdit = false;

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		{
			//on remplir le CB d'aide
			CharacterEditor::FillCBTriggerHelper( hWnd, GetDlgItem( hWnd, IDC_COMBO_TRIGGER_KEYWORD ) );

			ComboBox_SetCurSel( GetDlgItem( hWnd, IDC_COMBO_TRIGGER_KEYWORD ), 0 );

			//std::string str;
			//char buf[512];

			//GetWindowText( GetDlgItem( hWnd, IDC_COMBO_TRIGGER_KEYWORD ), buf, sizeof(buf) );
			//str.append( buf );

			CharacterEditor::FillStaticTextTriggerHelper( GetDlgItem( hWnd, IDC_TEXT_TRIGGER_KEY_WORD_DESC), 0 );
			
			if ( lParam == NULL )
			{
				s_bTriggerEdit = false;
				g_pTrigger = new Trigger;

				CheckDlgButton( hWnd, IDC_CHECK_TRIGGER_PERSISTENCE, BST_CHECKED);
			}
			else // parametre donc edit
			{
				s_bTriggerEdit = true;
				g_pTrigger = (Trigger *) lParam;

				if ( g_pTrigger == NULL )
				{
					MessageBox( NULL, "TriggerDialogProc : dynamic_cast<Trigger *> (lParam)", "Erreur", MB_OK );
					PostQuitMessage(-1);
				}

				CheckDlgButton( hWnd, IDC_CHECK_TRIGGER_PERSISTENCE, g_pTrigger->GetPersistence() ? BST_CHECKED : BST_UNCHECKED );

				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_TRIGGER_EXPRESSION), g_pTrigger->GetExpression().c_str() );
			}
		}
		break;

	case WM_COMMAND:
		
		switch ( LOWORD(wParam) )
		{
		case IDOK:
			{
				char buf[512];

				GetWindowText( GetDlgItem( hWnd, IDC_EDIT_TRIGGER_EXPRESSION), buf, 511 );

				if ( g_pTrigger->IsExpressionValid( buf ) == false )
				{
					MessageBox( NULL, "Expression is not valid", "Erreur", MB_OK | MB_ICONWARNING);
					return FALSE;
				}
				else
				{
					if ( g_pController == NULL )
					{
						//g_pController = new StateController;
						MessageBox( NULL, "TriggerDialogProc(...) : IDOK : g_pController == NULL", "Erreur", MB_OK);
					}
					else
					{
						g_pTrigger->SetExpression( buf );
						
						if ( IsDlgButtonChecked( hWnd, IDC_CHECK_TRIGGER_PERSISTENCE ) == BST_CHECKED )
						{
							g_pTrigger->SetPersistence( true );
						}
						else
						{
							g_pTrigger->SetPersistence( false );
						}

						if ( s_bTriggerEdit == false )
						{
							g_pController->AddTrigger( *g_pTrigger );
							g_bTriggerAdded = true;
						}
						else
						{
							g_bTriggerEdited = true;
						}
					}
				}
			}
			EndDialog(hWnd, TRUE);
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;

		case IDC_COMBO_TRIGGER_KEYWORD:
			if  ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_TRIGGER_KEYWORD ) );

				if ( sel != CB_ERR )
				{
					//std::string str;
					//char buf[512];

					//GetWindowText( GetDlgItem( hWnd, IDC_COMBO_TRIGGER_KEYWORD ), buf, sizeof(buf) );
					//str.append( buf );

					CharacterEditor::FillStaticTextTriggerHelper( GetDlgItem( hWnd, IDC_TEXT_TRIGGER_KEY_WORD_DESC) , sel );
				}
			}
			break;
		}// fin switch ( LOWORD(wParam) ) 		
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		if ( s_bTriggerEdit == false )
		{
			delete g_pTrigger;
		}

		g_pTrigger = NULL;
		break;
	}
	
	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL APIENTRY EditValueStateControllerDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static eStateControllerValueType type;

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		{
			char buf[64];

			type = g_value.type;

			CharacterEditor::FillStateDefListHelper( GetDlgItem( hWnd, IDC_LIST_STATEDEF_HELPER ) );

			switch ( g_value.type )
			{
			case STATE_CONTROLLER_VALUE_TYPE_FLOAT:
				CheckDlgButton( hWnd, IDC_RADIO_VALUE_TYPE_FLOAT, BST_CHECKED);
				CheckDlgButton( hWnd, IDC_RADIO_VALUE_TYPE_VECTOR, BST_UNCHECKED);

				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_2 ), SW_HIDE );
				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_3 ), SW_HIDE );

				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_2 ), SW_HIDE );
				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_3 ), SW_HIDE );

				SetWindowText( GetDlgItem( hWnd, IDC_STATIC_TEXT_1 ), "value" );

				sprintf_s( buf, 64, "%f", g_value.real );
				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_1), buf );
				break;

			case STATE_CONTROLLER_VALUE_TYPE_VEC3:
				CheckDlgButton( hWnd, IDC_RADIO_VALUE_TYPE_FLOAT, BST_UNCHECKED);
				CheckDlgButton( hWnd, IDC_RADIO_VALUE_TYPE_VECTOR, BST_CHECKED);

				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_2 ), SW_SHOW );
				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_3 ), SW_SHOW );

				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_2 ), SW_SHOW );
				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_3 ), SW_SHOW );

				SetWindowText( GetDlgItem( hWnd, IDC_STATIC_TEXT_1 ), "x" );

				sprintf_s( buf, 64, "%f", g_value.vec3.x );
				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_1), buf );
				sprintf_s( buf, 64, "%f", g_value.vec3.y );
				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_2), buf );
				sprintf_s( buf, 64, "%f", g_value.vec3.z );
				SetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_3), buf );
				break;
			}
		}

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDC_RADIO_VALUE_TYPE_FLOAT:
			if ( IsDlgButtonChecked( hWnd, IDC_RADIO_VALUE_TYPE_FLOAT ) == BST_CHECKED )
			{
				type = STATE_CONTROLLER_VALUE_TYPE_FLOAT;

				SetWindowText( GetDlgItem( hWnd, IDC_STATIC_TEXT_1 ), "value" );

				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_2 ), SW_HIDE );
				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_3 ), SW_HIDE );

				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_2 ), SW_HIDE );
				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_3 ), SW_HIDE );
			}
			break;

		case IDC_RADIO_VALUE_TYPE_VECTOR:
			if ( IsDlgButtonChecked( hWnd, IDC_RADIO_VALUE_TYPE_VECTOR ) == BST_CHECKED )
			{
				type = STATE_CONTROLLER_VALUE_TYPE_VEC3;

				SetWindowText( GetDlgItem( hWnd, IDC_STATIC_TEXT_1 ), "x" );

				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_2 ), SW_SHOW );
				ShowWindow( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_3 ), SW_SHOW );

				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_2 ), SW_SHOW );
				ShowWindow( GetDlgItem( hWnd, IDC_STATIC_TEXT_3 ), SW_SHOW );
			}
			break;

		case IDOK:
			{
				char buf[64];
				g_value.type = type;

				switch ( type )
				{
				case STATE_CONTROLLER_VALUE_TYPE_FLOAT:
					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_1 ), buf, 63 );
					g_value.real = (float) atof( buf );
					break;

				case STATE_CONTROLLER_VALUE_TYPE_VEC3:
					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_1 ), buf, 63 );
					g_value.vec3.x = (float) atof( buf );

					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_2 ), buf, 63 );
					g_value.vec3.y = (float) atof( buf );

					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_STATE_CONTROLLER_VALUE_3 ), buf, 63 );
					g_value.vec3.z = (float) atof( buf );
					break;
				}

				g_bValueChanged = true;

				EndDialog(hWnd, TRUE);
			}
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;

		} // fin switch COMMAND
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}
	
	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL APIENTRY  EditVectorDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static D3DXVECTOR3 *sVec = NULL;

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		{
			char buf[64];
			sVec = (D3DXVECTOR3 *) lParam;
			
			sprintf_s( buf, sizeof(buf), "%f", sVec->x );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_VECTOR_X ), buf );

			sprintf_s( buf, sizeof(buf), "%f", sVec->y );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_VECTOR_Y ), buf );

			sprintf_s( buf, sizeof(buf), "%f", sVec->z );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_VECTOR_Z ), buf );
		}

	case WM_COMMAND:
		switch ( LOWORD(wParam) )
		{
		case IDOK:
			{
				char buf[64];

				GetWindowText( GetDlgItem( hWnd, IDC_EDIT_VECTOR_X ), buf, 63) ;
				sVec->x = (float) atof( buf );
				
				GetWindowText( GetDlgItem( hWnd, IDC_EDIT_VECTOR_Y ), buf, 63) ;
				sVec->y = (float) atof( buf );

				GetWindowText( GetDlgItem( hWnd, IDC_EDIT_VECTOR_Z ), buf, 63) ;
				sVec->z = (float) atof( buf );

				g_bVectorEdited = true;

				EndDialog(hWnd, TRUE);
			}
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;

		} // fin switch COMMAND
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}
	
	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL CALLBACK StateDefDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Graphic &graphic = GameCore::Instance().GetGraphic();

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		g_aChildTab[CHILD_TAB_STATEDEF] = hWnd;

		if ( graphic.SetMode( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_PREVIEW), true, true) == false )
		{
			return -1;
		}

		if ( GameCore::Instance().CreateInputDevice( g_hWnd, g_hInst, true ) == false )
		{
			return -1;
		}

		g_hThread = CreateThread( 
				NULL,                   // default security attributes
				0,                      // use default stack size  
				GraphicThreadFunction,       // thread function name
				NULL,          // argument to thread function 
				0,                      // use default creation flags 
				NULL);   // returns the thread identifier 

		g_bThreadRunning = true;

		if (g_hThread == NULL) 
		{
			exit(-1);
		}

		CharacterEditor::InitMainDialogGadget();
		return TRUE;
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_MESH_FILE:
			g_ofn.hwndOwner   = g_aChildTab[CHILD_TAB_STATEDEF];
			g_ofn.lpstrFile   = g_FileNameBuf;
			g_ofn.lpstrTitle  = "Load Mexh File";
			g_ofn.lpstrFilter = "Direct X Files (*.x)\0*.x\0All Files (*.*)\0*.*\0\0";
			g_ofn.lpstrDefExt = "x";

			// Ask for filename
			if(!GetOpenFileName(&g_ofn))
				return FALSE;

			if ( g_hThread)
			{
				g_bThreadEnd = true;

				if ( WAIT_FAILED == WaitForSingleObject( g_hThread, INFINITE ) )
				{
					exit( -1 );
				}

				g_bThreadRunning = false;

				g_hThread = NULL;
			}

			if ( g_Charac.GetMesh()->Load(g_ofn.lpstrFile) == false)
			{
				char buf[512];
				sprintf_s( buf, 512, "Impossible d'ouvrir %s", g_ofn.lpstrFile );
				MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], buf, "Erreur", MB_OK );
			}
			else
			{
				g_Charac.SetMeshFileName( g_ofn.lpstrFile );

				CharacterEditor::InitCharacterGadget();

				g_cameraModelViewer.SetRadius( g_Charac.GetMesh()->GetBoundingSphere().GetRadius() * 2.0f );

				g_bThreadEnd = false;

				g_hThread = CreateThread( 
				NULL,                   // default security attributes
				0,                      // use default stack size  
				GraphicThreadFunction,       // thread function name
				NULL,          // argument to thread function 
				0,                      // use default creation flags 
				NULL);   // returns the thread identifier 

				g_bThreadRunning = true;

				if (g_hThread == NULL) 
				{
					exit(-1);
				}
			}
			break;

		case IDC_BUTTON_ADD_STATEDEF:
			{
				if ( g_hThread && g_bThreadRunning)
				{
					g_bThreadRunning = false;
					SuspendThread( g_hThread );
				}

				if ( DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_STATEDEF), g_aChildTab[CHILD_TAB_STATEDEF], AddStateDefDialogProc) <= 0)
				{
					MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur de creation de la fenete fille IDD_DIALOG_ADD_STATEDEF", "Erreur", MB_OK);
				}
				else
				{
					int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_STATE_DEF ) );

					if ( sel != CB_ERR )
					{
						CharacterEditor::SelectStateDef( sel );
					}
					else
					{
						MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur selection nouveau statedef", "Erreur", MB_OK);
					}
				}

				if ( g_hThread && !g_bThreadRunning)
				{
					g_bThreadRunning = true;
					ResumeThread( g_hThread );
				}
			}
			break;

		case IDC_BUTTON_EDIT_STATEDEF:
			{
				int sel = ComboBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ) );

				if ( sel != CB_ERR )
				{
					if ( g_hThread && g_bThreadRunning)
					{
						g_bThreadRunning = false;
						SuspendThread( g_hThread );
					}

					if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_STATEDEF), g_aChildTab[CHILD_TAB_STATEDEF], AddStateDefDialogProc, (LPARAM) g_pStateDef ) <= 0)
					{
						MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur de creation de la fenete fille IDD_DIALOG_ADD_STATEDEF", "Erreur", MB_OK);
					}

					if ( g_hThread && !g_bThreadRunning )
					{
						g_bThreadRunning = true;
						ResumeThread( g_hThread );
					}
				}
			}
			break;

		case IDC_BUTTON_DELETE_STATEDEF:
			{
				//TODO : obtenir le texte
				//le comparer
				//si state commun => interdiction
				int sel = SendMessage(GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_GETCURSEL, 0, 0);
				if ( sel != CB_ERR )
				{
					SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_DELETESTRING, sel, 0);
					SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_SETCURSEL, 0, 0); 

					if ( g_Charac.GetStateDef().size() > 0 )
					{
						g_StateDefId = STATE_DEF_IDLE;
						g_pStateDef = &g_Charac.GetStateDef()[STATE_DEF_IDLE];

						//TODO : comboBox selection
					}
					else
					{
						g_StateDefId = -2;
						g_pStateDef = NULL;
					}
				}
			}
			break;

		case IDC_COMBO_STATE_DEF:
			if  ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				int sel = SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_GETCURSEL, 0, 0);

				if ( sel != CB_ERR )
				{
					CharacterEditor::SelectStateDef( sel );
				}
			}
			break;

		case IDC_COMBO_STATE_TYPE:
			if  ( HIWORD(wParam) == CBN_SELCHANGE && g_pStateDef )
			{
				int sel = ComboBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE ) );

				if ( sel != CB_ERR )
				{
					g_pStateDef->SetStateType( (eStateType) sel );
				}
			}
			break;

		case IDC_COMBO_MOVE_TYPE:
			if  ( HIWORD(wParam) == CBN_SELCHANGE && g_pStateDef )
			{
				int sel = ComboBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE ) );

				if ( sel != CB_ERR )
				{
					g_pStateDef->SetMoveType( (eMoveType) sel );
				}
			}
			break;

		case IDC_COMBO_PHYSIC:
			if  ( HIWORD(wParam) == CBN_SELCHANGE && g_pStateDef )
			{
				int sel = ComboBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC ) );

				if ( sel != CB_ERR )
				{
					g_pStateDef->SetPhysics( (ePhysics) sel );
				}
			}
			break;

		case IDC_COMBO_ANIM_NAME:
			if ( HIWORD(wParam) == CBN_SELCHANGE )
			{
				int sel = SendMessage(GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME), CB_GETCURSEL, 0, 0);
				if ( sel != CB_ERR )
				{
					if ( g_Charac.GetMesh()->IsLoaded() )
					{
						g_Charac.GetMesh()->SetAnimationSet(sel);
						g_Charac.GetMesh()->SetCurrentAnimationTime( 0.0f, &g_MatWorld);

						SetWindowText( GetDlgItem(hWnd, IDC_TEXT_SCROLLBAR_POSITION), "0");

						SCROLLINFO si;
						si.cbSize = sizeof(si);
						si.fMask = SIF_POS;
						si.nPos = 0;
						SetScrollInfo( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_SCROLLBAR1), SB_CTL, &si, TRUE);

						g_pStateDef->SetAnimId(sel);
					}
				}
			}
			break;

		case IDC_EDIT_SPEED_ANIM:
			if ( g_pStateDef != NULL && HIWORD(wParam) == EN_CHANGE )
			{
				char buf[64];
				GetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_SPEED_ANIM), buf, 63);
				g_pStateDef->SetAnimSpeed( (float) atof(buf) );
			}
			break;

		case IDC_BUTTON_ADD_STATE_CONTROLLER:

			if ( g_hThread && g_bThreadRunning)
			{
				g_bThreadRunning = false;
				SuspendThread( g_hThread );
			}

			if ( DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_STATE_CONTROLLER), g_aChildTab[CHILD_TAB_STATEDEF], AddStateControllerDialogProc) <= 0)
			{
				MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur de creation de la fenete fille IDD_DIALOG_TRIGGER", "Erreur", MB_OK);
			}
			else if ( g_bStateControllerAdded && g_pStateDef )
			{
				SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER),
					LB_ADDSTRING,
					0,
					reinterpret_cast<LPARAM>( g_pStateDef->GetStateController().back().ToString().c_str() ) );

				g_bStateControllerAdded = false;
			}

			if ( g_hThread && !g_bThreadRunning)
			{
				g_bThreadRunning = true;
				ResumeThread( g_hThread );
			}
			break;

		case IDC_BUTTON_EDIT_STATE_CONTROLLER:
			{
				if ( g_hThread && g_bThreadRunning )
				{
					g_bThreadRunning = false;
					SuspendThread( g_hThread );
				}

				int sel = ListBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER) );

				if ( sel != LB_ERR )
				{
					StateController *stateController = &( *( g_pStateDef->GetStateController().begin() + sel) );

					if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_STATE_CONTROLLER), g_aChildTab[CHILD_TAB_STATEDEF], AddStateControllerDialogProc, (LPARAM) stateController ) <= 0)
					{
						MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur de creation de la fenete fille IDD_DIALOG_TRIGGER", "Erreur", MB_OK);
					}
					else if ( g_bStateControllerEdited )
					{
						ListBox_ResetContent( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ) );

						CharacterEditor::FillListBoxStateControllerName( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ), g_pStateDef->GetStateController(), sel );

						g_bStateControllerEdited = false;
					}
				}

				if ( g_hThread && !g_bThreadRunning )
				{
					g_bThreadRunning = true;
					ResumeThread( g_hThread );
				}
			}
			break;

		case IDC_BUTTON_DELETE_STATE_CONTROLLER:
			{
				int sel = ListBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER) );

				if ( sel != LB_ERR )
				{
					g_pStateDef->GetStateController().erase( g_pStateDef->GetStateController().begin() + sel );
					ListBox_DeleteString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER), sel );
				}
			}
			break;

		case IDC_COMBO_PLAYER_CONTROL:
			{
				if ( HIWORD(wParam) == CBN_SELCHANGE )
				{
					int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_PLAYER_CONTROL ) );

					if ( sel != CB_ERR && g_pStateDef )
					{
						g_pStateDef->SetPlayerCtrl( sel == 0 ? true : false );
					}
				}
			}
			break;

		case IDC_EDIT_POWER_ADD:
			{
				if ( HIWORD(wParam) == EN_CHANGE && g_pStateDef )
				{
					char buf[128];
					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_POWER_ADD ), buf, 127 );
					g_pStateDef->SetPowerAdd( atoi(buf) );
				}
			}
			break;

		case IDC_EDIT_JUGGLE:
			{
				if ( HIWORD(wParam) == EN_CHANGE && g_pStateDef )
				{
					char buf[128];
					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_JUGGLE ), buf, 127 );
					g_pStateDef->SetJuggle( atoi(buf) );
				}
			}
			break;

		case IDC_COMBO_FACE_P2:
			{
				if ( HIWORD(wParam) == CBN_SELCHANGE )
				{
					int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_FACE_P2 ) );

					if ( sel != CB_ERR && g_pStateDef )
					{
						g_pStateDef->SetFaceP2( sel == 0 ? true : false );
					}
				}
			}
			break;

		case IDC_COMBO_HIT_DEF_PERSIST:
			{
				if ( HIWORD(wParam) == CBN_SELCHANGE )
				{
					int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_HIT_DEF_PERSIST ) );

					if ( sel != CB_ERR && g_pStateDef )
					{
						g_pStateDef->SetHitDefPersist( sel == 0 ? true : false );
					}
				}
			}
			break;

		case IDC_COMBO_MOVE_HIT_PERSIST:
			{
				if ( HIWORD(wParam) == CBN_SELCHANGE )
				{ 
					int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_MOVE_HIT_PERSIST ) );

					if ( sel != CB_ERR && g_pStateDef )
					{
						g_pStateDef->SetMoveHitPersist( sel == 0 ? true : false );
					}
				}
			}
			break;

		case IDC_COMBO_HIT_COUNT_PERSIST:
			{
				if ( HIWORD(wParam) == CBN_SELCHANGE )
				{
					int sel = ComboBox_GetCurSel( GetDlgItem( hWnd, IDC_COMBO_HIT_COUNT_PERSIST ) );

					if ( sel != CB_ERR && g_pStateDef )
					{
						g_pStateDef->SetHitCountPersist( sel == 0 ? true : false );
					}
				}
			}
			break;

		case IDC_BUTTON_UPDATE_COLLISION:
			{
				HWND hWndDlg = GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY);
				int nbSel = ListBox_GetSelCount( hWndDlg );
				int *pSel = NULL;
				//int len;
				char buf[512];

				if ( g_pStateDef == NULL )
				{
					break;
				}
				
				if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_ATT ) == BST_CHECKED )
				{
					g_pStateDef->ClearCollisionAtt();
				}
				else if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_DEF ) == BST_CHECKED )
				{
					g_pStateDef->ClearCollisionDef();
				}

				if ( nbSel > 0 )
				{
					pSel = new int[nbSel];

					ListBox_GetSelItems( hWndDlg, nbSel, pSel );

					for ( int i = 0; i < nbSel; i++ )
					{
						ListBox_GetText( hWndDlg, pSel[i], buf );

						LPD3DXFRAME pFrame = D3DXFrameFind( g_Charac.GetMesh()->GetFrameRoot(), buf );

						if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_ATT ) == BST_CHECKED )
						{
							g_pStateDef->AddCollisionAtt( pFrame );
						}
						else if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_DEF ) == BST_CHECKED )
						{
							g_pStateDef->AddCollisionDef( pFrame );
						}
					}
					
					delete[] pSel;
				}				
			}
			break;

		case IDC_BUTTON_UPDATE_COLLISION_FOR_ALL:
			{
				HWND hWndDlg = GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY);
				int nbSel = ListBox_GetSelCount( hWndDlg );
				int *pSel = NULL;
				//int len;
				char buf[512];
				std::map< long, StateDef >::iterator it;

				/*if ( g_pStateDef == NULL )
				{
					break;
				}*/
				
				for ( it = g_Charac.GetStateDef().begin(); it != g_Charac.GetStateDef().end(); it++ )
				{
					if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_ATT ) == BST_CHECKED )
					{
						it->second.ClearCollisionAtt();
					}
					else if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_DEF ) == BST_CHECKED )
					{
						it->second.ClearCollisionDef();
					}
				}

				if ( nbSel > 0 )
				{
					pSel = new int[nbSel];

					ListBox_GetSelItems( hWndDlg, nbSel, pSel );

					for ( int i = 0; i < nbSel; i++ )
					{
						ListBox_GetText( hWndDlg, pSel[i], buf );

						LPD3DXFRAME pFrame = D3DXFrameFind( g_Charac.GetMesh()->GetFrameRoot(), buf );

						for ( it = g_Charac.GetStateDef().begin(); it != g_Charac.GetStateDef().end(); it++ )
						{
							
							if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_ATT ) == BST_CHECKED )
							{
								it->second.AddCollisionAtt( pFrame );
							}
							else if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_DEF ) == BST_CHECKED )
							{
								it->second.AddCollisionDef( pFrame );
							}
						}
					}
					
					delete[] pSel;
				}				
			}
			break;

		case IDC_RADIO_COLLISION_ATT:
			{
				//deselectionne
				for ( int i = 0; i < ListBox_GetCount( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY) ); i++ )
				{
					ListBox_SetSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), FALSE, i );
				}
				
				//on selectionne dans la liste les bones de collision
				if ( g_pStateDef )
				{
					int k;

					for ( unsigned int i = 0; i < g_pStateDef->GetCollisionAtt().size(); i++ )
					{
						k = ListBox_FindString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), 0, g_pStateDef->GetCollisionAtt().at(i).GetBoneName().c_str() );
						ListBox_SetSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), TRUE, k );
					}
				}
			}
			break;

		case IDC_RADIO_COLLISION_DEF:
			{
				//on selectionne dans la liste les bones de collision
				//deselectionne
				for ( int i = 0; i < ListBox_GetCount( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY) ); i++ )
				{
					ListBox_SetSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), FALSE, i );
				}
				
				//on selectionne dans la liste les bones de collision
				if ( g_pStateDef )
				{
					int k;

					for ( unsigned int i = 0; i < g_pStateDef->GetCollisionDef().size(); i++ )
					{
						k = ListBox_FindString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), 0, g_pStateDef->GetCollisionDef().at(i).GetBoneName().c_str() );
						ListBox_SetSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), TRUE, k );
					}
				}
			}
			break;

		case IDC_CHECK_SHOW_OBB_ATTACK:
			if ( IsDlgButtonChecked( hWnd, IDC_CHECK_SHOW_OBB_ATTACK ) == BST_CHECKED )
			{
				g_bShowOBBAttack = true;
			}
			else
			{
				g_bShowOBBAttack = false;
			}
			break;

		case IDC_CHECK_SHOW_OBB_DEFENSE:
			if ( IsDlgButtonChecked( hWnd, IDC_CHECK_SHOW_OBB_DEFENSE ) == BST_CHECKED )
			{
				g_bShowOBBDefense = true;
			}
			else
			{
				g_bShowOBBDefense = false;
			}
			break;

		case IDC_BUTTON_PLAY:
			if ( g_bPlayingAnimation == false && g_Charac.GetMesh()->IsLoaded() && g_pStateDef )
			{
				g_Charac.GetMesh()->GetAnimationController()->SetTrackSpeed( g_Charac.GetMesh()->GetCurrentTrack(), g_pStateDef->GetAnimSpeed() );
				g_Charac.GetMesh()->SetCurrentAnimationTime( 0.0f, &g_MatWorld );
				g_bPlayingAnimation = true;
			}
			break;

		case IDC_BUTTON_STATE_CONTROLLER_UP:
			{
				int sel = ListBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ) );

				if ( sel != LB_ERR && sel > 0)
				{
					std::swap( g_pStateDef->GetStateController().at(sel), g_pStateDef->GetStateController().at(sel - 1) );
					CharacterEditor::FillListBoxStateControllerName( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ), g_pStateDef->GetStateController(), sel - 1 );
				}				
			}
			break;

		case IDC_BUTTON_STATE_CONTROLLER_DOWN:
			{
				int sel = ListBox_GetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ) );

				if ( sel != LB_ERR && sel < (int) g_pStateDef->GetStateController().size() - 1 )
				{
					std::swap( g_pStateDef->GetStateController().at(sel), g_pStateDef->GetStateController().at(sel + 1) );
					CharacterEditor::FillListBoxStateControllerName( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ), g_pStateDef->GetStateController(), sel + 1 );
				}
			}
			break;

		} //  WM_COMMAND switch( LOWORD(wParam) )
		break;

	case WM_HSCROLL:
	{
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		if ( g_bPlayingAnimation )
		{
			return FALSE;
		}

		GetScrollInfo( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_SCROLLBAR1), SB_CTL, &si);
		int xPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT: // user clicked left arrow
			si.nPos -= 1;
			break;
		
		case SB_LINERIGHT: // user clicked right arrow
			si.nPos += 1;
			break;

		case SB_PAGELEFT: // user clicked the scroll bar shaft left of the scroll box
			si.nPos -= si.nPage;
			break;

		case SB_PAGERIGHT: // user clicked the scroll bar shaft right of the scroll box
			si.nPos += si.nPage;
			break;

		case SB_THUMBTRACK: // user dragged the scroll box
			si.nPos = si.nTrackPos;
			break;

		default :
			break;
		}
		// Set the position and then retrieve it.  Due to adjustments
		//   by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_SCROLLBAR1), SB_CTL, &si, TRUE);

		if ( xPos != si.nPos )
		{
			float animTime = 0.0f;

			if ( g_Charac.GetMesh()->IsLoaded() )
			{
				g_Charac.GetMesh()->SetCurrentAnimationTime( (float) si.nPos, &g_MatWorld);
				animTime = (float) g_Charac.GetMesh()->GetCurrentAnimationTime();
			}

			char buf[64];
			sprintf_s(buf, 64, "%d%%  (%f)", si.nPos, animTime);
			SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_TEXT_SCROLLBAR_POSITION), buf);			
		}
	}
	break;

	case WM_CLOSE:
		EndDialog( g_aChildTab[CHILD_TAB_STATEDEF], TRUE);
		break;

	case WM_DESTROY:
		break;
	}

	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL CALLBACK CommandDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{
    case WM_INITDIALOG:
		g_aChildTab[CHILD_TAB_COMMAND] = hWnd;			
		return TRUE;
		break;

	case WM_SHOWWINDOW:
		{
			//on remplit le listBox
			Command *pCommand = g_Charac.GetCommand();
			HWND hWndListCommand = GetDlgItem( hWnd, IDC_LIST_COMMAND_NAME);

			ListBox_ResetContent( hWndListCommand );

			if ( pCommand )
			{
				CharacterEditor::FillListBoxCommandName( hWndListCommand, *pCommand->GetCommands() );
			}
		}
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_COMMAND_ADD:
			if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_COMMAND), g_aChildTab[CHILD_TAB_COMMAND], CommandAddDialogProc, NULL ) <= 0)
			{
				MessageBox( g_aChildTab[CHILD_TAB_COMMAND], "Erreur de creation de la fenete fille IDD_DIALOG_ADD_STATEDEF", "Erreur", MB_OK);
			}
			else if ( g_bCombinationAdded )
			{
				g_bCombinationAdded = false;

				ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_NAME ), g_pCombination->GetName() );

				g_Charac.GetCommand()->Add( *g_pCombination );

				delete g_pCombination;
				g_pCombination = NULL;
			}
			break;

		case IDC_BUTTON_COMMAND_EDIT:
			{
				HWND hWndListCommand = GetDlgItem( hWnd, IDC_LIST_COMMAND_NAME);

				int sel = ListBox_GetCurSel( hWndListCommand );

				if ( sel != -1 )
				{
					ButtonCombination *pButtonCombination = &g_Charac.GetCommand()->GetCommands()->at(sel);

					if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_COMMAND), g_aChildTab[CHILD_TAB_COMMAND], CommandAddDialogProc, (LPARAM) pButtonCombination ) <= 0)
					{
						MessageBox( hWndListCommand, "Erreur de creation de la fenete fille IDD_DIALOG_ADD_STATEDEF", "Erreur", MB_OK);
					}
					else if ( g_bCombinationEdited )
					{
						g_bCombinationEdited = false;

						ListBox_ResetContent( hWndListCommand );

						CharacterEditor::FillListBoxCommandName( hWndListCommand, *g_Charac.GetCommand()->GetCommands() );
					}
				}
			}
			break;

		case IDC_BUTTON_COMMAND_DELETE:
			{
				HWND hWndListCommand = GetDlgItem( hWnd, IDC_LIST_COMMAND_NAME);

				int sel = ListBox_GetCurSel( hWndListCommand );

				if ( sel != LB_ERR )
				{
					ListBox_DeleteString( hWndListCommand, sel );
					CharacterEditor::DeleteCommandCombination( sel );
				}

			}
			break;

		} //  WM_COMMAND switch( LOWORD(wParam) )
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}

	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL CALLBACK CommandAddDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static  ButtonCombination *s_pCombination = NULL;

	switch(uMsg) 
	{
    case WM_INITDIALOG:		
		if ( lParam )
		{
			s_pCombination = (ButtonCombination *) lParam;

			char buf[64];

			sprintf_s( buf, sizeof(buf), "%s", s_pCombination->GetName() );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_NAME ), buf );

			sprintf_s( buf, sizeof(buf), "%d", s_pCombination->GetTimeMax() );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_TIME_MAX ), buf );

			CheckDlgButton( hWnd, IDC_CHECK_SUB_COMBINATION, s_pCombination->IsSubCombination() ? BST_CHECKED : BST_UNCHECKED );

			CharacterEditor::FillListBoxButtonName( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), *s_pCombination->GetCombination() );
		}
		else
		{
			s_pCombination = NULL;

			//s_pCombination = new cButtonCombination;
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_NAME ), "name" );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_TIME_MAX ), "-1" );

			CheckDlgButton( hWnd, IDC_CHECK_SUB_COMBINATION, BST_UNCHECKED );
		}
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_COMMAND_ADD:
			if ( DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_COMMAND_BUTTON), hWnd, CommandAddButtonDialogProc, NULL ) <= 0)
			{
				MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur de creation de la fenete fille IDD_DIALOG_ADD_COMMAND_BUTTON", "Erreur", MB_OK);
			}
			else if ( g_bCombinationButtonAdded )
			{
				g_bCombinationButtonAdded = false;

				if ( s_pCombination )
				{
					s_pCombination->Add( *g_pCombinationButton );
				}
				else
				{
					if ( g_pCombination == NULL )
					{
						g_pCombination = new ButtonCombination;
					}

					g_pCombination->Add( *g_pCombinationButton );
				}

				delete g_pCombinationButton;
				g_pCombinationButton = NULL;

				ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), g_szButtonString.c_str() );

				g_szButtonString.clear();				
			}
			break;

		case IDC_BUTTON_COMMAND_EDIT:
			{
				int sel = ListBox_GetCurSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ) );

				if ( sel != -1 /*&& g_pCombination->GetCombination()->size() > sel*/ )
				{
					ButtonCombination::sCombination *pCombi = NULL;

					if ( s_pCombination )
					{
						pCombi = &s_pCombination->GetCombination()->at(sel);
					}
					else if ( g_pCombination )
					{
						pCombi = &g_pCombination->GetCombination()->at(sel);
					}
					else
					{
						MessageBox( hWnd, "erreur cButtonCombination NULL", "Erreur", MB_OK );
						break;
					}
				
					if ( DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD_COMMAND_BUTTON), hWnd, CommandAddButtonDialogProc, (LPARAM) pCombi ) <= 0)
					{
						MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur de creation de la fenete fille IDD_DIALOG_ADD_COMMAND_BUTTON", "Erreur", MB_OK);
					}
					else if ( g_bCombinationButtonEdited )
					{
						g_bCombinationButtonEdited = false;

						ListBox_ResetContent( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ) );
						
						if ( s_pCombination )
						{
							CharacterEditor::FillListBoxButtonName( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), *s_pCombination->GetCombination() );
						}
						else if ( g_pCombination )
						{
							CharacterEditor::FillListBoxButtonName( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), *g_pCombination->GetCombination() );
						}
						
						g_szButtonString.clear();
					}
				}
			}
			break;

		case IDC_BUTTON_COMMAND_DELETE:
			{
				int sel;
				sel = ListBox_GetCurSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ) );

				if ( sel != -1 )
				{
					ListBox_DeleteString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), sel );

					if ( s_pCombination )
					{
						CharacterEditor::DeleteCommandButton( sel, s_pCombination );
						//s_pCombination = NULL;
					}
					else if ( g_pCombination )
					{
						CharacterEditor::DeleteCommandButton( sel, g_pCombination );
						//g_pCombination = NULL;
					}
				}
			}
			break;

		case IDOK:
			{
				bool checked = IsDlgButtonChecked( hWnd, IDC_CHECK_SUB_COMBINATION) == BST_CHECKED ? true : false;

				if ( g_pCombination && s_pCombination == NULL )
				{
					char buf[128];

					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_NAME ), buf, 127 );
					g_pCombination->SetName( buf );

					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_TIME_MAX ), buf, 127 );
					g_pCombination->SetTimeMax( atoi(buf) );

					g_pCombination->SetSubCombination( checked );

					g_bCombinationAdded = true;
				}
				else if ( s_pCombination )
				{
					char buf[128];

					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_NAME ), buf, 127 );
					s_pCombination->SetName( buf );

					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_COMMAND_TIME_MAX ), buf, 127 );
					s_pCombination->SetTimeMax( atoi(buf) );

					s_pCombination->SetSubCombination( checked );

					g_bCombinationEdited = true;
				}
				else
				{
					MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "Erreur lors de l'ajout de la combination", "Erreur", MB_OK);
				}
			}
			EndDialog(hWnd, TRUE);
			break;

		case IDCANCEL:
			if ( g_pCombination )
			{
				delete g_pCombination;
				g_pCombination = NULL;
			}
			EndDialog(hWnd, TRUE);
			break;

		} //  WM_COMMAND switch( LOWORD(wParam) )
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}

	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL CALLBACK CommandAddButtonDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static ButtonCombination::sCombination *s_pCombinationButton = NULL;

	switch(uMsg) 
	{
    case WM_INITDIALOG:
		//remplir combobox button
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "UP" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "FORWARD" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "DOWN" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "BACKWARD" );		
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "PUNCH L" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "PUNCH M" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "PUNCH H" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "KICK L" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "KICK M" );
		ListBox_AddString( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), "KICK H" );
		
		if ( lParam == NULL )
		{
			s_pCombinationButton = NULL;

			//ListBox_SetCurSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), 0 );
			CheckDlgButton( hWnd, IDC_RADIO_BUTTON_PRESSED, BST_CHECKED );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_BUTTON_TIME ), "0" );
		}
		else
		{
			s_pCombinationButton = (ButtonCombination::sCombination *) lParam;

			switch ( s_pCombinationButton->type )
			{
			case ButtonCombination::BUTTON_PRESSED:
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_PRESSED, BST_CHECKED );
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_HELD, BST_UNCHECKED );
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_RELEASED, BST_UNCHECKED );
				break;

			case ButtonCombination::BUTTON_HELD:
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_PRESSED, BST_UNCHECKED );
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_HELD, BST_CHECKED );
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_RELEASED, BST_UNCHECKED );
				break;

			case ButtonCombination::BUTTON_RELEASED:
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_PRESSED, BST_UNCHECKED );
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_HELD, BST_UNCHECKED );
				CheckDlgButton( hWnd, IDC_RADIO_BUTTON_RELEASED, BST_CHECKED );
				break;
			}

			char buf[64];
			sprintf_s( buf, sizeof(buf), "%d", s_pCombinationButton->heldTime );
			SetWindowText( GetDlgItem( hWnd, IDC_EDIT_BUTTON_TIME ), buf );

			if ( ( s_pCombinationButton->button & MASK_UP ) == MASK_UP )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_UP );
			}

			if ( ( s_pCombinationButton->button & MASK_RIGHT ) == MASK_RIGHT )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_RIGHT );
			}

			if ( ( s_pCombinationButton->button & MASK_LEFT ) == MASK_LEFT )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_LEFT );
			}

			if ( ( s_pCombinationButton->button & MASK_DOWN ) == MASK_DOWN )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_DOWN );
			}

			if ( ( s_pCombinationButton->button & MASK_BUTTON_1 ) == MASK_BUTTON_1 )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_BUTTON_1 );
			}

			if ( ( s_pCombinationButton->button & MASK_BUTTON_2 ) == MASK_BUTTON_2 )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_BUTTON_2 );
			}

			if ( ( s_pCombinationButton->button & MASK_BUTTON_3 ) == MASK_BUTTON_3 )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_BUTTON_3 );
			}

			if ( ( s_pCombinationButton->button & MASK_BUTTON_4 ) == MASK_BUTTON_4 )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_BUTTON_4 );
			}

			if ( ( s_pCombinationButton->button & MASK_BUTTON_5 ) == MASK_BUTTON_5 )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_BUTTON_5 );
			}

			if ( ( s_pCombinationButton->button & MASK_BUTTON_6 ) == MASK_BUTTON_6 )
			{
				ListBox_SetSel( GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON ), true, PAD_BUTTON_6 );
			}
		}
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
			{
				HWND hWndList = GetDlgItem( hWnd, IDC_LIST_COMMAND_BUTTON );

				int nbSel = ListBox_GetSelCount( hWndList );

				if ( nbSel > 0 )
				{
					int *pSel = new int[nbSel];
					unsigned long button = 0;

					ListBox_GetSelItems( hWndList, nbSel, pSel );

					for ( int i = 0; i < nbSel; i++ )
					{
						if ( i > 0 )
						{
							g_szButtonString.append( "+ ");
						}

						switch ( pSel[i] )
						{
						case PAD_UP:
							button |= MASK_UP;
							g_szButtonString.append( "UP ");
							break;

						case PAD_DOWN:
							button |= MASK_DOWN;
							g_szButtonString.append( "DOWN ");
							break;

						case PAD_RIGHT:
							button |= MASK_RIGHT;
							g_szButtonString.append( "FORWARD ");
							break;

						case PAD_LEFT:
							button |= MASK_LEFT;
							g_szButtonString.append( "BACKWARD ");
							break;

						case PAD_BUTTON_1:
							button |= MASK_BUTTON_1;
							g_szButtonString.append( "PUNCH L ");
							break;

						case PAD_BUTTON_2:
							button |= MASK_BUTTON_2;
							g_szButtonString.append( "PUNCH M ");
							break;

						case PAD_BUTTON_3:
							button |= MASK_BUTTON_3;
							g_szButtonString.append( "PUNCH H ");
							break;

						case PAD_BUTTON_4:
							button |= MASK_BUTTON_4;
							g_szButtonString.append( "KICK L ");
							break;

						case PAD_BUTTON_5:
							button |= MASK_BUTTON_5;
							g_szButtonString.append( "KICK M ");
							break;

						case PAD_BUTTON_6:
							button |= MASK_BUTTON_6;
							g_szButtonString.append( "KICK H ");
							break;
						}
					}
					
					char buf[64];
					GetWindowText( GetDlgItem( hWnd, IDC_EDIT_BUTTON_TIME ), buf, sizeof(buf) );

					if ( s_pCombinationButton )
					{
						s_pCombinationButton->button = button;
						s_pCombinationButton->heldTime = atoi(buf);

						if ( IsDlgButtonChecked( hWnd, IDC_RADIO_BUTTON_PRESSED ) )
						{
							s_pCombinationButton->type = ButtonCombination::BUTTON_PRESSED; 
						}
						else if ( IsDlgButtonChecked( hWnd, IDC_RADIO_BUTTON_HELD ) )
						{
							s_pCombinationButton->type = ButtonCombination::BUTTON_HELD; 
						}
						else if ( IsDlgButtonChecked( hWnd, IDC_RADIO_BUTTON_RELEASED ) )
						{
							s_pCombinationButton->type = ButtonCombination::BUTTON_RELEASED; 
						}

						g_bCombinationButtonEdited = true;
					}
					else
					{
						g_pCombinationButton = new ButtonCombination::sCombination;
						//cButtonCombination::sCombination combination;
						g_pCombinationButton->button = button;
						g_pCombinationButton->heldTime = atoi(buf);

						if ( IsDlgButtonChecked( hWnd, IDC_RADIO_BUTTON_PRESSED ) )
						{
							g_pCombinationButton->type = ButtonCombination::BUTTON_PRESSED; 
						}
						else if ( IsDlgButtonChecked( hWnd, IDC_RADIO_BUTTON_HELD ) )
						{
							g_pCombinationButton->type = ButtonCombination::BUTTON_HELD; 
						}
						else if ( IsDlgButtonChecked( hWnd, IDC_RADIO_BUTTON_RELEASED ) )
						{
							g_pCombinationButton->type = ButtonCombination::BUTTON_RELEASED; 
						}

						g_bCombinationButtonAdded = true;
					}
					
					EndDialog(hWnd, TRUE);
				}// end if nbSel > 0
				else
				{
					MessageBox( hWnd, "Selectionner au moins un bouton", "Erreur", MB_OK );
				}
			}			
			break;

		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;
		} //  WM_COMMAND switch( LOWORD(wParam) )
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}

	return FALSE;
}

//===================================================================================================
//===================================================================================================
BOOL CALLBACK ParameterDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{
    case WM_INITDIALOG:
		{
			g_aChildTab[CHILD_TAB_PARAMETER] = hWnd;
		}
		return TRUE;
		break;

	case WM_SHOWWINDOW:
		{
			char buf[128];

			sprintf_s( buf, sizeof(buf), "%f %f %f", g_Charac.GetMeshDirection().x, g_Charac.GetMeshDirection().y, g_Charac.GetMeshDirection().z );
			SetWindowText( GetDlgItem( hWnd, IDC_TEXT_MESH_DIRECTION_VALUE ), buf );

			sprintf_s( buf, sizeof(buf), "%f %f %f", g_Charac.GetCharacterBox().GetMin().x, g_Charac.GetCharacterBox().GetMin().y, g_Charac.GetCharacterBox().GetMin().z );
			SetWindowText( GetDlgItem( hWnd, IDC_TEXT_BOX_MIN_VALUE ), buf );

			sprintf_s( buf, sizeof(buf), "%f %f %f", g_Charac.GetCharacterBox().GetMax().x, g_Charac.GetCharacterBox().GetMax().y, g_Charac.GetCharacterBox().GetMax().z );
			SetWindowText( GetDlgItem( hWnd, IDC_TEXT_BOX_MAX_VALUE ), buf );
		}
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_MESH_DIRECTION:
			{
				D3DXVECTOR3 vec = g_Charac.GetMeshDirection();

				if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDIT_VECTOR), hWnd, EditVectorDialogProc, (LPARAM) &vec ) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_EDIT_VECTOR", "Erreur", MB_OK);
				}
				else if ( g_bVectorEdited )
				{
					g_Charac.SetMeshDirection( vec );
					g_bVectorEdited = false;

					char buf[128];
					sprintf_s( buf, sizeof(buf), "%f %f %f", vec.x, vec.y, vec.z );
					SetWindowText( GetDlgItem( hWnd, IDC_TEXT_MESH_DIRECTION_VALUE ), buf );
				}
			}
			break;

		case IDC_BUTTON_BOX_MIN:
			{
				D3DXVECTOR3 vec = g_Charac.GetCharacterBox().GetMin();

				if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDIT_VECTOR), hWnd, EditVectorDialogProc, (LPARAM) &vec ) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_EDIT_VECTOR", "Erreur", MB_OK);
				}
				else if ( g_bVectorEdited )
				{
					g_bVectorEdited = false;
					g_Charac.GetCharacterBox().SetMin( vec );

					char buf[128];
					sprintf_s( buf, sizeof(buf), "%f %f %f", vec.x, vec.y, vec.z );
					SetWindowText( GetDlgItem( hWnd, IDC_TEXT_BOX_MIN_VALUE ), buf );
				}
			}
			break;

		case IDC_BUTTON_BOX_MAX:
			{
				D3DXVECTOR3 vec = g_Charac.GetCharacterBox().GetMax();

				if ( DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDIT_VECTOR), hWnd, EditVectorDialogProc, (LPARAM) &vec ) <= 0)
				{
					MessageBox(hWnd, "Erreur de creation de la fenete fille IDD_DIALOG_EDIT_VECTOR", "Erreur", MB_OK);
				}
				else if ( g_bVectorEdited )
				{
					g_bVectorEdited = false;
					g_Charac.GetCharacterBox().SetMax( vec );

					char buf[128];
					sprintf_s( buf, sizeof(buf), "%f %f %f", vec.x, vec.y, vec.z );
					SetWindowText( GetDlgItem( hWnd, IDC_TEXT_BOX_MAX_VALUE ), buf );
				}
			}
			break;
		} //  WM_COMMAND switch( LOWORD(wParam) )
		break;

	case WM_CLOSE:
		EndDialog(hWnd, TRUE);
		break;

	case WM_DESTROY:
		break;
	}

	return FALSE;
}
