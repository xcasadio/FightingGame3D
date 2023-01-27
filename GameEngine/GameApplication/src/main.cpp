#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#include "GameCore.h"
#include "Window.h"
#include "Pad.h"
#include "MediaPathManager.h"
#include "Logger/Logger.h"
#include "Logger/LoggerFile.h"
#include "ResourceManager.h"
#include "Plugin.h"
#include "json11/json11.hpp"

#include <windows.h>
#include <windowsx.h>

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;
using namespace json11;

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	ILogger::SetLogger(new cLoggerFile());

	std::ifstream configFile("gameengine.json");
	std::stringstream buffer;

	if (configFile)
	{		
		buffer << configFile.rdbuf();
		configFile.close();
	}

	std::string err;
	Json configFileJson = Json::parse(buffer.str(), err);

	GameCore::s_window_param param;
	
	Plugin game = Plugin(configFileJson["gamedll"].string_value()); //from config

	Json windowParam = configFileJson["window"];
	param.depth = windowParam["depth"].int_value();
	param.width = windowParam["width"].int_value();
	param.height = windowParam["height"].int_value();
	param.windowed = windowParam["windowed"].bool_value();
	param.title = windowParam["title"].string_value().c_str();

	Window win;
	if (!win.Create(param.width, param.height, param.title, param.windowed))
	{
		return -1;
	}
	
	if (GameCore::Instance().Init(nCmdShow, szCmdLine, win, param) == false)
	{
		return -1;
	}

	if (!GameCore::Instance().CreateInputDevice(win.GethWnd(), win.GethInst(), param.windowed))
	{
		return -1;
	}		

	Pad::sPad configButtons[MAX_PAD_BUTTON];
	int keys[MAX_PAD_BUTTON] = {17, 32, 31, 30, 28, 1, 25, 38, 24, 23, 22, 21};

	for (int i=0; i< MAX_PAD_BUTTON; i++)
	{
		configButtons[i].primaryKey.input = INPUT_DEVICE_KEYBOARD;
		configButtons[i].primaryKey.key = keys[i];
	}

	GameCore::Instance().GetPad(0).Load(&configButtons[0]);

	/*
	if ( !cGameCore::Instance().GetPad(0).Load("Data\\pad.xml") )
	{
		cWindow::Error( true, "Erreur pad Load(Data\\pad.xml)" );
	}

	if ( !cGameCore::Instance().GetPad(1).Load("Data\\pad2.xml") )
	{
		cWindow::Error( true, "Erreur pad Load(Data\\pad2.xml)" );
	}

	
	cMediaPathManager::Instance().AddPath( "Data\\character\\" );
	cMediaPathManager::Instance().AddPath( "Data\\mesh\\stage\\" );
	cMediaPathManager::Instance().AddPath( "Data\\mesh\\character\\" );
	cMediaPathManager::Instance().AddPath( "Data\\img\\Item\\" );
	cMediaPathManager::Instance().AddPath( "Data\\img\\menu\\");

	cResourceManager::Instance().Add( GLOBAL_VAR_P1_CHOOSEN, new GlobalVariable );
	cResourceManager::Instance().Add( GLOBAL_VAR_P2_CHOOSEN, new GlobalVariable );
	cResourceManager::Instance().Add( GLOBAL_VAR_STAGE_CHOOSEN, new GlobalVariable );
	cResourceManager::Instance().Add( GLOBAL_VAR_PLAY_MODE, new GlobalVariable );
	*/
	//do in plugin game
	//cGameCore::Instance().AddGameState(new GameRunStage); //MainMenu;

	GameCore::Instance().Run();

	/*
	cResourceManager::Instance().Remove( GLOBAL_VAR_P1_CHOOSEN );
	cResourceManager::Instance().Remove( GLOBAL_VAR_P2_CHOOSEN );
	cResourceManager::Instance().Remove( GLOBAL_VAR_STAGE_CHOOSEN );
	cResourceManager::Instance().Remove( GLOBAL_VAR_PLAY_MODE );
	*/
	GameCore::Destroy();
	MediaPathManager::Destroy();

	return 0;
}