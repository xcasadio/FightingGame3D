#include <windows.h>
#include <windowsx.h>

#include "GameCore.h"
#include "Window.h"
#include "Pad.h"
#include "MainMenu.h"
#include "GameRunStage.h" // a enlever
#include "MediaPathManager.h"
#include "ResourceManager.h"
#include "Command.h"
#include "GlobalVariable.h"
#include "Logger/Logger.h"
#include "Logger/LoggerFile.h"

using namespace GameEngine;

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	ILogger::SetLogger(new LoggerFile());

	GameCore::s_window_param param;

#ifdef _DEBUG
	param.depth = 32;
	param.width = 1024;
	param.height = 768;
	param.windowed = true;
#else
	param.depth = 32;
	param.width = 1024;
	param.height = 768;
	//TODO : savoir pourquoi en fullscreen ca bug
	param.windowed = true;
#endif

	Window win;
	if (!win.Create(param.width, param.height, "Fighting Game", param.windowed))
		return -1;

	if (GameCore::Instance().Init(nCmdShow, szCmdLine, win, param) == false)
		return -1;

	if (!GameCore::Instance().CreateInputDevice(win.GethWnd(), win.GethInst(), param.windowed))
		return -1;

	if (!GameCore::Instance().GetPad(0).Load("pad.xml"))
	{
		Window::Error(true, "Erreur pad Load(pad.xml)");
	}

	if (!GameCore::Instance().GetPad(1).Load("pad2.xml"))
	{
		Window::Error(true, "Erreur pad Load(pad2.xml)");
	}

	MediaPathManager::Instance().AddPath("Data\\character\\");
	MediaPathManager::Instance().AddPath("Data\\mesh\\stage\\");
	MediaPathManager::Instance().AddPath("Data\\mesh\\character\\");
	MediaPathManager::Instance().AddPath("Data\\img\\Item\\");
	MediaPathManager::Instance().AddPath("Data\\img\\menu\\");

	ResourceManager::Instance().Add(GLOBAL_VAR_P1_CHOOSEN, new GlobalVariable);
	ResourceManager::Instance().Add(GLOBAL_VAR_P2_CHOOSEN, new GlobalVariable);
	ResourceManager::Instance().Add(GLOBAL_VAR_STAGE_CHOOSEN, new GlobalVariable);
	ResourceManager::Instance().Add(GLOBAL_VAR_PLAY_MODE, new GlobalVariable);

	GameCore::Instance().AddGameState(new MainMenu);

	GameCore::Instance().Run();

	ResourceManager::Instance().Remove(GLOBAL_VAR_P1_CHOOSEN);
	ResourceManager::Instance().Remove(GLOBAL_VAR_P2_CHOOSEN);
	ResourceManager::Instance().Remove(GLOBAL_VAR_STAGE_CHOOSEN);
	ResourceManager::Instance().Remove(GLOBAL_VAR_PLAY_MODE);

	GameCore::Destroy();
	MediaPathManager::Destroy();

	return 0;
}