#include "Editor.h"

#include "resource.h"

#include <string>
#include <map>

#include "DebugEngine/DebugNew.h"

extern BOOL CALLBACK CommandDialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
extern BOOL CALLBACK ParameterDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK  StateDefDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

POINT					g_MousePosition;
HWND					g_hWnd;
HWND					g_aChildTab[MAX_CHILD_TAB];
HINSTANCE				g_hInst;
HANDLE					g_hThread;
OPENFILENAME			g_ofn;
std::string				g_FileName/*[MAX_PATH]*/;
char					g_FileNameBuf[MAX_PATH];
cCameraModelViewer		g_cameraModelViewer;
Pad					g_Pad;
DWORD					g_BackgroundColor = 0xffb4b4b4;
D3DXMATRIX				g_MatWorld;
CharacterLocal			g_Charac(NULL);
StateDef				*g_pStateDef = NULL;
StateController			*g_pController = NULL;
Trigger					*g_pTrigger = NULL;
long					g_StateDefId = 0;
bool					g_bTriggerAdded = false;
bool					g_bStateControllerAdded = false;
bool					g_bTriggerEdited = false;
bool					g_bStateControllerEdited = false;
bool					g_bThreadEnd = false;
sStateControllerValue	g_value = { STATE_CONTROLLER_VALUE_TYPE_FLOAT, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) };
bool					g_bValueChanged = false;
ButtonCombination		*g_pCombination = NULL;
bool					g_bCombinationAdded = false;
bool					g_bCombinationEdited = false;
bool					g_bCombinationButtonAdded = false;
bool					g_bCombinationButtonEdited = false;
bool					g_bVectorEdited = false;
std::string				g_szButtonString;
ButtonCombination::sCombination *g_pCombinationButton = NULL;
bool					g_bThreadRunning = false;
bool					g_bShowOBBAttack = true;
bool					g_bShowOBBDefense = true;
bool					g_bPlayingAnimation = false;

std::map< std::string, std::string> g_MaptriggerKeyWordHelper;

void InitMaptriggerKeyWordHelper( std::map< std::string, std::string> &map_ );

/**
 * On s'en sert juste pour verifier si les states existent et non sont pas dubliqués
 */
char pStateDefName[][32] = 
{
	"AIR",
	"IDLE",
	"CROUCH",
	"WALKFORWARD",
	"WALKBACKWARD",
};

int aStateDefId[] = 
{
	STATE_DEF_IDLE,
	STATE_DEF_WALK_FORWARD,
	STATE_DEF_WALK_BACKWARD,
	STATE_DEF_IDLE_TO_CROUCH,
	STATE_DEF_CROUCH,
	STATE_DEF_CROUCH_TO_IDLE,
	STATE_DEF_IDLE_TO_AIR,
	STATE_DEF_IDLE_TO_AIR_FORWARD,
	STATE_DEF_IDLE_TO_AIR_BACKWARD,
	STATE_DEF_AIR_BACKWARD,
	STATE_DEF_AIR_IDLE,
	STATE_DEF_AIR_FORWARD,
	STATE_DEF_AIR_TO_IDLE,

	STATE_DEF_GUARD_STAND,
	STATE_DEF_GUARD_CROUCH,
	STATE_DEF_GUARD_AIR,

	STATE_DEF_HIT_STAND,
	STATE_DEF_HIT_CROUCH,
	STATE_DEF_HIT_AIR,
};

char pStateControllerTypeName[][32] = 
{
	"CHANGE ANIM",
	"CHANGE STATE",
	"CTRL SET",
	"HIT DEF",
	"MOVE PLAYER",
	"MOVE PLAYER BY TIME",
};

int aControllerTypeValue[] = 
{
	STATE_CONTROLLER_TYPE_CHANGE_ANIM,
	STATE_CONTROLLER_TYPE_CHANGE_STATE,
	STATE_CONTROLLER_TYPE_CTRL_SET,
	STATE_CONTROLLER_TYPE_HIT_DEF,
	STATE_CONTROLLER_TYPE_MOVE_PLAYER,
	STATE_CONTROLLER_TYPE_MOVE_PLAYER_BY_TIME,
};

//===================================================================================================
//===================================================================================================
void CharacterEditor::CreateCharacter()
{
	/*
	if ( g_pCharac )
	{
		delete g_pCharac;
	}

	g_pCharac = new cCharacterLocal;

	//fill gadget
	*/
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::InitMainDialogGadget()
{
	// STATE TYPE
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "STAND" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE),
						CB_ADDSTRING,
						0,
						reinterpret_cast<LPARAM>( "CROUCH" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE),
						CB_ADDSTRING,
						0,
						reinterpret_cast<LPARAM>( "AIR" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE),
						CB_ADDSTRING,
						0,
						reinterpret_cast<LPARAM>( "LIE DOWN" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE),
						CB_ADDSTRING,
						0,
						reinterpret_cast<LPARAM>( "UNCHANGED" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE), CB_SETCURSEL, 0, 0);

	//MOVE TYPE
	SendMessage( GetDlgItem(g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "ATTACK" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "IDLE" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "HIT" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "UNCHANGED" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE), CB_SETCURSEL, 0, 0);

	//PHYSIC
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "STAND" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "CROUCH" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "AIR" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "NONE" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "UNCHANGED" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC), CB_SETCURSEL, 0, 0);

	//COMBO TRUE-FALSE
	//player control
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PLAYER_CONTROL),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "True" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PLAYER_CONTROL),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "False" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PLAYER_CONTROL), CB_SETCURSEL, 1, 0);

	//face P2
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_FACE_P2),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "True" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_FACE_P2),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "False" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_FACE_P2), CB_SETCURSEL, 1, 0);

	//hit def persist
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_DEF_PERSIST),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "True" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_DEF_PERSIST),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "False" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_DEF_PERSIST), CB_SETCURSEL, 1, 0);

	//move hit persist
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_HIT_PERSIST),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "True" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_HIT_PERSIST),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "False" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_HIT_PERSIST), CB_SETCURSEL, 1, 0);

	//hit count persist
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_COUNT_PERSIST),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "True" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_COUNT_PERSIST),
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>( "False" ) );

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_COUNT_PERSIST), CB_SETCURSEL, 1, 0);

	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_JUGGLE), "0");

	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_POWER_ADD), "0");

	//type collision
	CheckDlgButton( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_ATT, BST_CHECKED );
	CheckDlgButton( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_DEF, BST_UNCHECKED );

	//show OBB collision
	CheckDlgButton( g_aChildTab[CHILD_TAB_STATEDEF], IDC_CHECK_SHOW_OBB_ATTACK, BST_CHECKED );
	CheckDlgButton( g_aChildTab[CHILD_TAB_STATEDEF], IDC_CHECK_SHOW_OBB_DEFENSE, BST_CHECKED );
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::InitStateControllerDialogGadget( HWND hWnd, StateController *pStateController_ )
{
	int i;

	//type
	for ( i = 0; i < sizeof(pStateControllerTypeName) / (sizeof(char) * 32); i++ )
	{
		SendMessage( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_TYPE),
			CB_ADDSTRING,
			0,
			reinterpret_cast<LPARAM> (pStateControllerTypeName[i]) );
	}

	ComboBox_AddString( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_HIT_HEIGHT), "LOW" );
	ComboBox_AddString( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_HIT_HEIGHT), "MIDDLE" );
	ComboBox_AddString( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_HIT_HEIGHT), "HIGH" );

	if ( pStateController_ == NULL )
	{
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_STATE_CONTROLLER_NAME), "name");

		//type
		SendMessage( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_TYPE), CB_SETCURSEL, 0, 0 );

		//value
		SetWindowText( GetDlgItem(hWnd, IDC_TEXT_VALUE), "value");

		//attribut 1

		//attribut 2

		ComboBox_SetCurSel( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_HIT_HEIGHT), 0 );

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_DAMAGE), "0");
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_GUARD_DAMAGE), "0");

		//Guard Flag

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PAUSE_TIME_P1), "0");
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PAUSE_TIME_P2), "0");

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPARK_NO), "0");

		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_HIT_SOUND), "0");
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_GUARD_SOUND), "0");

		SetWindowText( GetDlgItem(hWnd, IDC_TEXT_SPARK_POSITION), "value");
		SetWindowText( GetDlgItem(hWnd, IDC_TEXT_SPARK_DIRECTION), "value");

		g_pController = new StateController;
	}
	else // parametre donc edit
	{
		char buf[64];

		//name
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_STATE_CONTROLLER_NAME), (LPSTR) g_pController->GetName().c_str() );

		//type
		for ( i = 0; i < sizeof(aControllerTypeValue) / sizeof(int); i++ )
		{
			if ( (eStateControllerType) aControllerTypeValue[i] == g_pController->GetType() )
			{
				ComboBox_SetCurSel( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_TYPE), i );
				break;
			}
		}

		//hitsound
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetHitSound() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_HIT_SOUND), buf );

		//guarsound
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetGuardSound() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_GUARD_SOUND), buf );

		//value
		switch ( g_pController->GetValue().type )
		{
		case STATE_CONTROLLER_VALUE_TYPE_FLOAT:
			sprintf_s( buf, sizeof(buf), "%f", g_pController->GetValue().real );
			break;

		case STATE_CONTROLLER_VALUE_TYPE_VEC3:
			sprintf_s( buf, sizeof(buf), "%f %f %f", g_pController->GetValue().vec3.x, g_pController->GetValue().vec3.y, g_pController->GetValue().vec3.z );
			break;
		}
		SetWindowText( GetDlgItem(hWnd, IDC_TEXT_VALUE), buf);

		//attr1

		//attr2

		//damage
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetDamage() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_DAMAGE), buf );

		//guard damage
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetGuardDamage() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_GUARD_DAMAGE), buf );

		//hitheight
		ComboBox_SetCurSel( GetDlgItem(hWnd, IDC_COMBO_STATE_CONTROLLER_HIT_HEIGHT), g_pController->GetHitHeight() );

		//guard flag

		//pause time P1
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetPauseTimeP1() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PAUSE_TIME_P1), buf );

		//pause time P2
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetPauseTimeP2() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_PAUSE_TIME_P2), buf );

		//spark no
		sprintf_s( buf, sizeof(buf), "%d", g_pController->GetSparkNo() );
		SetWindowText( GetDlgItem(hWnd, IDC_EDIT_SPARK_NO), buf );

		//spark position
		sprintf_s( buf, sizeof(buf), "%f %f %f", g_pController->GetSparkPosition().x, g_pController->GetSparkPosition().y, g_pController->GetSparkPosition().z );
		SetWindowText( GetDlgItem(hWnd, IDC_TEXT_SPARK_POSITION), buf);

		//spark direction
		sprintf_s( buf, sizeof(buf), "%f %f %f", g_pController->GetSparkDirection().x, g_pController->GetSparkDirection().y, g_pController->GetSparkDirection().z );
		SetWindowText( GetDlgItem(hWnd, IDC_TEXT_SPARK_DIRECTION), buf);

		//triggers
		ListBox_ResetContent( GetDlgItem( hWnd, IDC_LIST_TRIGGER) );
		FillListBoxTriggerName( GetDlgItem( hWnd, IDC_LIST_TRIGGER), g_pController->GetTriggers() );

		ListBox_SetCurSel( GetDlgItem( hWnd, IDC_LIST_TRIGGER ), 0 );
	}
}

//===================================================================================================
//===================================================================================================
eStateControllerType CharacterEditor::GetControllerTypeFromCBSelected( int t_ )
{
	return (eStateControllerType) aControllerTypeValue[t_];
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::FillGadgetFromCharacterValue()
{
	std::map< long, StateDef >::iterator it;
	char buf[256];

	for ( it = g_Charac.GetStateDef().begin(); it != g_Charac.GetStateDef().end(); it++ )
	{
		sprintf_s( buf, 256, "%d %s", it->first, it->second.GetDescription() );

		SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF),
					CB_ADDSTRING,
					0,
					reinterpret_cast<LPARAM>( buf ) );
	}

	ComboBox_SetCurSel( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ), 0 );
	SelectStateDef( 0 );

	FillGadgetStateDef();
}


//===================================================================================================
//===================================================================================================
/** 
 * Remplit les gadgets avec les donnees du stateDef courant, le parametre specifie
 * si le stateDef est commun ou non. Si commun on ne peut le modifié sauf l'anim
 * @param stateCommun_ si true desactive les gadgets sauf l'animation
 */
void CharacterEditor::FillGadgetStateDef()
{
	char buf[64];
	bool communState = false;
	
	if ( g_Charac.GetMesh()->IsLoaded() )
	{
		g_Charac.GetMesh()->SetAnimationSet( g_pStateDef->GetAnimId() );
		g_Charac.GetMesh()->SetCurrentAnimationTime( 0.0f, &g_MatWorld );

		SetWindowText( GetDlgItem( g_hWnd, IDC_TEXT_SCROLLBAR_POSITION ), "0%%  (0.00000)");

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = 0;
		SetScrollInfo( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_SCROLLBAR1), SB_CTL, &si, TRUE);
	}

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE), CB_SETCURSEL, g_pStateDef->GetStateType(), 0 );
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE), CB_SETCURSEL, g_pStateDef->GetMoveType(), 0 );
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC), CB_SETCURSEL, g_pStateDef->GetPhysics(), 0 );
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME ), CB_SETCURSEL, g_pStateDef->GetAnimId(), 0 );

	//speed
	sprintf_s(buf, 64, "%f", g_pStateDef->GetAnimSpeed() );
	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_SPEED_ANIM), buf);

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PLAYER_CONTROL), CB_SETCURSEL, g_pStateDef->GetPlayerCtrl() ? 0 : 1, 0 );
	
	sprintf_s( buf, 64, "%f", g_pStateDef->GetAnimSpeed());
	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_SPEED_ANIM), buf);

	sprintf_s( buf, 64, "%d", g_pStateDef->GetPowerAdd());
	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_POWER_ADD), buf);

	sprintf_s( buf, 64, "%d", g_pStateDef->GetJuggle());
	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_JUGGLE), buf);

	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_FACE_P2), CB_SETCURSEL, g_pStateDef->GetFaceP2() ? 0 : 1, 0 ); 
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_COUNT_PERSIST), CB_SETCURSEL, g_pStateDef->GetHitCountPersist() ? 0 : 1, 0 ); 
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_DEF_PERSIST), CB_SETCURSEL, g_pStateDef->GetHitDefPersist() ? 0 : 1, 0 ); 
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_HIT_PERSIST), CB_SETCURSEL, g_pStateDef->GetMoveHitPersist() ? 0 : 1, 0 );

	//Si state predefini on desactive certain gadget
	for ( int i = 0; i < sizeof(aStateDefId) / sizeof(int); i++ )
	{
		if ( g_StateDefId == aStateDefId[i] )
		{
			//communState = true;
			break;
		}
	}

	if ( communState )
	{
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_FACE_P2), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_COUNT_PERSIST), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_DEF_PERSIST), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_HIT_PERSIST), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PLAYER_CONTROL), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE), false);

		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_JUGGLE), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_POWER_ADD), false);

		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER), false);

		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_BUTTON_ADD_STATE_CONTROLLER), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_BUTTON_EDIT_STATE_CONTROLLER), false);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_BUTTON_DELETE_STATE_CONTROLLER), false);
	}
	else
	{
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_FACE_P2), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_COUNT_PERSIST), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_HIT_DEF_PERSIST), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_HIT_PERSIST), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_MOVE_TYPE), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PHYSIC), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_PLAYER_CONTROL), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_TYPE), true);

		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_JUGGLE), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_POWER_ADD), true);

		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER), true);

		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_BUTTON_ADD_STATE_CONTROLLER), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_BUTTON_EDIT_STATE_CONTROLLER), true);
		EnableWindow( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_BUTTON_DELETE_STATE_CONTROLLER), true);
	}

	//collision
	if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_ATT ) == BST_CHECKED )
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
	else if ( IsDlgButtonChecked( g_aChildTab[CHILD_TAB_STATEDEF], IDC_RADIO_COLLISION_DEF ) == BST_CHECKED )
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

	ListBox_ResetContent( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ) );

	FillListBoxStateControllerName( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER ), g_pStateDef->GetStateController(), 0 );
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::FillListBoxButtonName( HWND hWnd_, std::vector<ButtonCombination::sCombination> &v_ )
{
	std::string str;
	std::vector<ButtonCombination::sCombination>::iterator it;
	
	for ( it = v_.begin(); it != v_.end(); it++ )
	{
		int nb = 0;

		for ( int i = 0; i < 16; i++ )
		{
			long n = 1 << i;

			if ( (it->button & n) == n )
			{
				if ( str.size() > 0 )
				{
					str.append( "+ ");
				}

				switch ( i )
				{
				case PAD_UP:
					str.append( "UP ");
					break;

				case PAD_DOWN:
					str.append( "DOWN ");
					break;

				case PAD_RIGHT:
					str.append( "FORWARD ");
					break;

				case PAD_LEFT:
					str.append( "BACKWARD ");
					break;

				case PAD_BUTTON_1:
					str.append( "PUNCH L ");
					break;

				case PAD_BUTTON_2:
					str.append( "PUNCH M ");
					break;

				case PAD_BUTTON_3:
					str.append( "PUNCH H ");
					break;

				case PAD_BUTTON_4:
					str.append( "KICK L ");
					break;

				case PAD_BUTTON_5:
					str.append( "KICK M ");
					break;

				case PAD_BUTTON_6:
					str.append( "KICK H ");
					break;
				}
			}
		}

		ListBox_AddString( hWnd_, str.c_str() );

		str.clear();
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::FillListBoxCommandName( HWND hWnd_, std::vector<ButtonCombination> &v_ )
{
	std::vector<ButtonCombination>::iterator itCommand;

	for ( itCommand = v_.begin(); itCommand != v_.end(); itCommand++ )
	{
		ListBox_AddString( hWnd_, itCommand->GetName() );
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::DeleteCommandCombination( int index_ )
{
	std::vector<ButtonCombination>::iterator it;
	int i = 0;

	for ( it = g_Charac.GetCommand()->GetCommands()->begin(); it != g_Charac.GetCommand()->GetCommands()->end(); it++ )
	{
		if ( i == index_ )
		{
			g_Charac.GetCommand()->GetCommands()->erase( it );
			break;
		}
		i++;
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::DeleteCommandButton( int index_, ButtonCombination *pCombi_ )
{
	std::vector<ButtonCombination::sCombination>::iterator it;

	int i = 0;

	for ( it = pCombi_->GetCombination()->begin(); it != pCombi_->GetCombination()->end(); it++ )
	{
		if ( i == index_ )
		{
			pCombi_->GetCombination()->erase( it );
			break;
		}
		i++;
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::FillListBoxStateControllerName( HWND hWnd_, std::vector<StateController> &v_, unsigned int sel_ )
{
	ListBox_ResetContent(hWnd_);

	if ( g_pStateDef )
	{
		std::vector<StateController>::iterator it;

		for ( it = v_.begin(); it != v_.end(); it++ )
		{
			ListBox_AddString( hWnd_, it->ToString().c_str() );
		}
	}

	ListBox_SetCurSel( hWnd_, sel_ );
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::FillListBoxTriggerName( HWND hWnd_, std::vector<Trigger> &v_ )
{
	for ( int i = 0; i < (int) v_.size(); i++ )
	{
		ListBox_AddString( hWnd_, v_.at(i).ToString().c_str() );
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::DeleteStateDef( long id_ )
{
	std::map<long, StateDef>::iterator it = g_Charac.GetStateDef().find(id_);

	if ( it == g_Charac.GetStateDef().end() )
	{
		Window::Error( false, "Le stateDef %d n'existe pas!", id_);
		return;
	}

	g_Charac.GetStateDef().erase( it );
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::InitCharacterGadget()
{
	D3DXMatrixIdentity(&g_MatWorld);

	if ( g_Charac.GetCommand() == NULL )
	{
		//g_Charac.Free();
		g_Charac.SetCommandType( COMMAND_TYPE_PLAYER );
		g_Charac.InitCommunParameters();
	}

	if ( g_Charac.GetMesh()->IsLoaded() )
	{
		g_Charac.GetMesh()->SetCurrentAnimationTime( 0.0f, &g_MatWorld);
	}
	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_TEXT_SCROLLBAR_POSITION), "0");

	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_EDIT_SPEED_ANIM), "1.0");

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_POS;
	si.nMin = 0;
	si.nMax = 99;
	si.nPos = 0;

	SetScrollInfo( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_SCROLLBAR1), SB_CTL, &si, TRUE);

	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_TEXT_MESH_FILENAME), g_ofn.lpstrFile);

	//clear COMBOX BOX
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME), CB_RESETCONTENT, 0, 0);
	SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF), CB_RESETCONTENT, 0, 0);

	//rajouter stateDef
	FillGadgetFromCharacterValue();

	//on cherche les anim
	if ( g_Charac.GetMesh()->IsLoaded() )
	{
		for (UINT i = 0; i < g_Charac.GetMesh()->GetNumberOfAnimationSet(); i++)
		{
			SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME),
						CB_ADDSTRING,
						0,
						reinterpret_cast<LPARAM>( (LPCSTR) g_Charac.GetMesh()->GetAnimationSetName(i).c_str() ) );
		}

		SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME), CB_SETCURSEL, 0, 0);

		//hierarchie
		InitHierarchy( g_Charac.GetMesh()->GetFrameRoot() );
	}

	//TODO : collision
	g_Charac.InitOBB();

	//anim id
	if ( g_pStateDef )
	{
		SendMessage( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME), CB_SETCURSEL, g_pStateDef->GetAnimId(), 0 );
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::InitHierarchy( LPD3DXFRAME pFrame_ )
{
	if ( strlen(pFrame_->Name) > 0 && pFrame_->pFrameFirstChild != NULL )
	{
		if ( pFrame_->pFrameFirstChild->pMeshContainer != NULL )
		{
			ListBox_AddString( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY), pFrame_->Name );
		}
	}

	if (pFrame_->pFrameSibling != NULL)
	{
		InitHierarchy( pFrame_->pFrameSibling);
	}

	if ( pFrame_->pFrameFirstChild != NULL)
	{
		InitHierarchy( pFrame_->pFrameFirstChild );
	}
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::SelectStateDef( int sel_ )
{
	char buf[512];

	//on recuperer l'Id
	if ( ComboBox_GetLBTextLen( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ), sel_ ) < 512 )
	{
		if ( ComboBox_GetLBText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF ), sel_, buf ) == CB_ERR )
		{
			MessageBox( g_aChildTab[CHILD_TAB_STATEDEF], "SelectStateDef", "Erreur", MB_OK | MB_ICONERROR );
			return;
		}
		
		g_StateDefId = atoi(buf);
		g_pStateDef = g_Charac.GetStateDef( g_StateDefId );

		if ( g_pStateDef != NULL )
		{
			FillGadgetStateDef();
		}
	}
}

void CharacterEditor::FillCBTriggerHelper( HWND hWnd_, HWND CBhWnd_ )
{
	static bool bInit = false;

	std::map< std::string, std::string>::iterator it;

	if ( bInit == false )
	{
		InitMaptriggerKeyWordHelper( g_MaptriggerKeyWordHelper );
		bInit = true;
	}

	for ( it = g_MaptriggerKeyWordHelper.begin(); it != g_MaptriggerKeyWordHelper.end(); it++ )
	{
		ComboBox_AddString( CBhWnd_, it->first.c_str() );
	}
}

void CharacterEditor::FillStaticTextTriggerHelper( HWND LabelhWnd_, /*std::string str*/ int sel_ )
{
	std::map< std::string, std::string>::iterator it;// = g_MaptriggerKeyWordHelper.find( str );
/*
	if ( it != g_MaptriggerKeyWordHelper.end() )
	{
		SetWindowText( LabelhWnd_, it->second.c_str() );
	}
	else
	{

	}*/

	for ( it = g_MaptriggerKeyWordHelper.begin(); it != g_MaptriggerKeyWordHelper.end(); it++ )
	{
		if ( sel_ == 0 )
		{
			SetWindowText( LabelhWnd_, it->second.c_str() );
			break;
		}

		sel_--;
	}
}

void InitMaptriggerKeyWordHelper( std::map< std::string, std::string> &map_ )
{
	std::string str;
	map_["\"\""] = str.append( "Un nom de commande doit être contenu entre les guillemets, cela permet de savoir si la commande à été retourné (renvoie 1)" );
	str.clear();
	map_[TRIGGER_KEY_WORD_A] = str.append( "retourne la valeur AIR" );
	str.clear();
	map_[TRIGGER_KEY_WORD_C] = str.append( "retourne la valeur CROUCH" );
	str.clear();
	map_[TRIGGER_KEY_WORD_S] = str.append( "retourne la valeur STAND" );
	str.clear();
	map_[TRIGGER_KEY_WORD_ANIMID] = str.append( "retourne l'identifiant de l'animation courante" );
	str.clear();
	map_[TRIGGER_KEY_WORD_ANIM_TIME] = str.append( "retourne le temps (la durée) de l'animation courant" );
	str.clear();
	map_[TRIGGER_KEY_WORD_CTRL] = str.append( "retourne 1 si le joueur a le controle du joueur sinon 1" );
	str.clear();
	map_[TRIGGER_KEY_WORD_COMMAND] = str.append( "Inutilisé" );
	str.clear();
	map_[TRIGGER_KEY_WORD_END_ANIM_REACH] = str.append( "retourne 1 si la fin de l'animation est atteinte" );
	str.clear();
	map_[TRIGGER_KEY_WORD_HIT] = str.append( "retourne 1 si le perso a touché l'adversaire" );
	str.clear();
	map_[TRIGGER_KEY_WORD_IS_HIT] = str.append( "retourne 1 si le perso a été touché par l'adversaire" );
	str.clear();
	map_[TRIGGER_KEY_WORD_MOVE_CONTACT] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_POS_X ] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_POS_Y] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_POS_Z] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_STATE_TYPE] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_STATE_NO] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_VEL_X] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_VEL_Y] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_VEL_Z] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_JUMP_VEL_X] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_JUMP_VEL_Y] = str.append( "retourne " );
	str.clear();
	map_[TRIGGER_KEY_WORD_JUMP_VEL_Z] = str.append( "retourne " );
	str.clear();
}

void CharacterEditor::FillStateDefListHelper( HWND hWnd_ )
{
	std::map< long, StateDef >::iterator it;
	char buf[256];

	for ( it = g_Charac.GetStateDef().begin(); it != g_Charac.GetStateDef().end(); it++ )
	{
		sprintf_s( buf, 256, "%d %s", it->first, it->second.GetDescription() );

		ListBox_AddString( hWnd_, buf );
	}

	ListBox_SetCurSel( hWnd_, 0 );
}

//===================================================================================================
//===================================================================================================
void CharacterEditor::UpdateChildTab(HWND hDlg)
{
	// afficher le dialogue correspondant à l'onglet sélectionné et masquer les autres
	HWND hTabCtrl = GetDlgItem(hDlg, IDC_TAB_MAIN);
	int nSelected = SendMessage(hTabCtrl, TCM_GETCURSEL, 0, 0);

	for( int i = 0; i < MAX_CHILD_TAB; i++ )
	{
		ShowWindow( g_aChildTab[i], (i == nSelected)? SW_SHOW : SW_HIDE);
	}
	
	if ( nSelected == CHILD_TAB_STATEDEF && g_hThread && !g_bThreadRunning)
	{
		ResumeThread( g_hThread );
		g_bThreadRunning = true;
	}
	else if ( nSelected != CHILD_TAB_STATEDEF && g_bThreadRunning )
	{
		g_bThreadRunning = false;
		SuspendThread( g_hThread );
	}
}

//===================================================================================================
//===================================================================================================
int CharacterEditor::MsgInitTabControlDialog(HWND hDlg)
{
	// création des dialogues enfants
	g_aChildTab[CHILD_TAB_STATEDEF] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_CHILD_DIALOG_STATEDEF),
									hDlg, StateDefDialogProc);
	g_aChildTab[CHILD_TAB_COMMAND] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_CHILD_DIALOG_COMMAND),
									hDlg, CommandDialogProc);
	g_aChildTab[CHILD_TAB_PARAMETER] = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_CHILD_DIALOG_PARAMETERS),
									hDlg, ParameterDialogProc);
	// ajout des onglets

	HWND hTabCtrl = GetDlgItem(hDlg, IDC_TAB_MAIN);
	TCITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = "StateDef";
	SendMessage(hTabCtrl, TCM_INSERTITEM, 0, (LPARAM) &tci);
	tci.pszText = "Command";
	SendMessage(hTabCtrl, TCM_INSERTITEM, 1, (LPARAM) &tci);
	tci.pszText = "Parameter";
	SendMessage(hTabCtrl, TCM_INSERTITEM, 2, (LPARAM) &tci);

	// détermination du rectangle où placer les dialogues enfants à partir du rectangle
	// du tabctrl, conversion dans le systemde coordonnée de la boîte de dialogue parente
	RECT rcTabCtrl;
	GetWindowRect(hTabCtrl, &rcTabCtrl);
	SendMessage(hTabCtrl, TCM_ADJUSTRECT, FALSE, (LPARAM) &rcTabCtrl);
	MapWindowPoints(NULL, hDlg, (LPPOINT) &rcTabCtrl, 2);

	// on place les dialogue enfants au centre de ce rectangle
	for(int i = 0; i < MAX_CHILD_TAB; i++)
	{
		// taille du dialogue enfant
		RECT rcChildTab;
		GetWindowRect( g_aChildTab[i], &rcChildTab );
		int x = ( rcTabCtrl.left + rcTabCtrl.right ) / 2 - ( rcChildTab.right - rcChildTab.left ) / 2;
		int y = ( rcTabCtrl.top + rcTabCtrl.bottom ) / 2 - ( rcChildTab.bottom - rcChildTab.top ) / 2;
		SetWindowPos( g_aChildTab[i], NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW );
	}

	// affichage de l'onglet un et mise à jour
	SendMessage(hTabCtrl, TCM_SETCURSEL, 0, 0);
	UpdateChildTab(hDlg);

	return TRUE;
}

void CharacterEditor::ClearCharacterGadget()
{
	SetWindowText( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_TEXT_MESH_FILENAME), "");

	//clear COMBOX BOX
	ComboBox_ResetContent( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_STATE_DEF) );
	ComboBox_ResetContent( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_COMBO_ANIM_NAME) );

	ListBox_ResetContent( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_STATE_CONTROLLER) );

	ListBox_ResetContent( GetDlgItem( g_aChildTab[CHILD_TAB_STATEDEF], IDC_LIST_HIERARCHY) );
}
