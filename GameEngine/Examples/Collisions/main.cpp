#include <windows.h>
#include <windowsx.h>

#include "GameCore.h"
#include "Window.h"
#include "Pad.h"
#include "GameRun.h"

using namespace GameEngine;

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	cGameCore::s_window_param param;

	param.depth = 32;
	param.height = 768;
	param.width = 1024;
#ifdef _DEBUG
	param.windowed = true;
#else
	param.windowed = false;
#endif

	cWindow win;
	if ( !win.Create( param.width, param.height, "Collision Demo", param.windowed) )
		return -1;
	
	if ( cGameCore::Instance().Init(nCmdShow, szCmdLine, win, param) == false )
		return -1;

	if ( !cGameCore::Instance().CreateInputDevice( win.GethWnd(), win.GethInst(), param.windowed ) )
		return -1;

	for ( int i = 0; i < MAX_PAD; i++ )
	{
		if ( !cGameCore::Instance().GetPad(i).Load("pad.xml") )
			MessageBox(NULL, "Erreur pad load()", "Erreur", MB_OK);
	}

	cGameCore::Instance().AddGameState(new GameRun());

	cGameCore::Instance().Run();

	cGameCore::Destroy();

	return 0;
}