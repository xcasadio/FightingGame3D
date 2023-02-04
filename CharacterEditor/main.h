#pragma once

#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <d3dx9anim.h>

#include "GameCore.h"
#include "Graphic.h"
#include "Light.h"
#include "Camera.h"
#include "Font.h"
#include "Input.h"
#include "MeshXAnimated.h"
#include "Pad.h"

#include "Character.h"
#include "CharacterLocal.h"
#include "State/stateDefTypes.h"
#include "State/stateDef.h"
#include "Command.h"

#include "Types.h"

#include <string>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

extern POINT					g_MousePosition;
extern HWND						g_hWnd;
extern HWND						g_aChildTab[MAX_CHILD_TAB];
extern HINSTANCE				g_hInst;
extern HANDLE					g_hThread;
extern OPENFILENAME				g_ofn;
extern std::string				g_FileName/*[MAX_PATH]*/;
extern char						g_FileNameBuf[MAX_PATH];
extern CameraModelViewer		g_cameraModelViewer;
extern Pad						g_Pad;
extern DWORD					g_BackgroundColor;
extern D3DXMATRIX				g_MatWorld;
extern CharacterLocal			g_Charac;
extern StateDef* g_pStateDef;
extern StateController* g_pController;
extern Trigger* g_pTrigger;
extern long						g_StateDefId;
extern bool						g_bTriggerAdded;
extern bool						g_bStateControllerAdded;
extern bool						g_bTriggerEdited;
extern bool						g_bStateControllerEdited;
extern bool						g_bThreadEnd;
extern sStateControllerValue	g_value;
extern bool						g_bValueChanged;
extern ButtonCombination* g_pCombination;
extern bool						g_bCombinationAdded;
extern bool						g_bCombinationEdited;
extern bool						g_bCombinationButtonAdded;
extern bool						g_bCombinationButtonEdited;
extern bool						g_bVectorEdited;
extern std::string				g_szButtonString;
extern ButtonCombination::sCombination* g_pCombinationButton;
extern bool						g_bThreadRunning;
extern bool						g_bShowOBBAttack;
extern bool						g_bShowOBBDefense;
extern bool						g_bPlayingAnimation;

BOOL CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//utiliser pour statedef
BOOL CALLBACK  StateDefDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY  AddStateDefDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY  AddStateControllerDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY  TriggerDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY  EditValueStateControllerDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY  EditVectorDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//utiliser pour command
BOOL CALLBACK CommandDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CommandAddDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CommandAddButtonDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//utiliser pour parameter
BOOL CALLBACK ParameterDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


DWORD WINAPI GraphicThreadFunction(LPVOID lpParam);

#include "DebugEngine/DebugNewOff.h"
