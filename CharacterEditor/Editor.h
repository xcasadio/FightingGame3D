#pragma once

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <d3d9.h>
#include <d3dx9core.h>
#include <d3dx9anim.h>

#include "Camera.h"
#include "Window.h"

#include "Character.h"
#include "CharacterLocal.h"
#include "State/stateDefTypes.h"
#include "State/stateDef.h"
#include "State/StateController.h"
#include "State/Trigger.h"
#include "Command.h"

#include "Types.h"

#include <vector>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

/**
 *
 */
class CharacterEditor
{
public:
	static void CreateCharacter();

	static void InitMainDialogGadget();
	static void InitStateControllerDialogGadget( HWND hWnd, StateController *pStateController_ );
	static eStateControllerType GetControllerTypeFromCBSelected( int t_ );
	static void FillGadgetFromCharacterValue();
	static void FillGadgetStateDef();

	static void FillListBoxStateControllerName( HWND hWnd_, std::vector<StateController> &v_, unsigned int sel_ );

	static void FillListBoxTriggerName( HWND hWnd_, std::vector<Trigger> &v_ );

	static void FillListBoxButtonName( HWND hWnd_, std::vector<ButtonCombination::sCombination> &v_ );
	static void FillListBoxCommandName( HWND hWnd_, std::vector<ButtonCombination> &v_ );

	static void FillCBTriggerHelper( HWND hWnd_, HWND CBhWnd_ );
	static void FillStaticTextTriggerHelper( HWND LabelhWnd_, /*std::string str*/ int sel_ );

	static void FillStateDefListHelper( HWND hWnd_ );

	static void DeleteCommandCombination( int index_ );
	static void DeleteCommandButton( int index_, ButtonCombination *pCombi_ );

	static void DeleteStateDef( long id_ );

	static void InitCharacterGadget();
	static void InitHierarchy( LPD3DXFRAME pFrame_ );

	static void SelectStateDef( int sel_ );

	static int MsgInitTabControlDialog(HWND hDlg);
	static void UpdateChildTab(HWND hDlg);

	static void ClearCharacterGadget();
};

#include "DebugEngine/DebugNewOff.h"
