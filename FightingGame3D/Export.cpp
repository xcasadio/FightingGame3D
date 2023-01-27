#include "GameCore.h"
#include "MainMenu.h"
#include "GameRunStage.h" // a enlever
#include "MediaPathManager.h"
#include "ResourceManager.h"
#include "GlobalVariable.h"

#include "DebugEngine/DebugNew.h"

extern "C" __declspec(dllexport) void LoadPlugin()
{
	MediaPathManager::Instance().AddPath("Data\\character\\");
	MediaPathManager::Instance().AddPath("Data\\mesh\\stage\\");
	MediaPathManager::Instance().AddPath("Data\\mesh\\character\\");
	MediaPathManager::Instance().AddPath("Data\\img\\Item\\");
	MediaPathManager::Instance().AddPath("Data\\img\\menu\\");

	ResourceManager::Instance().Add(GLOBAL_VAR_P1_CHOOSEN, new GlobalVariable);
	ResourceManager::Instance().Add(GLOBAL_VAR_P2_CHOOSEN, new GlobalVariable);
	ResourceManager::Instance().Add(GLOBAL_VAR_STAGE_CHOOSEN, new GlobalVariable);
	ResourceManager::Instance().Add(GLOBAL_VAR_PLAY_MODE, new GlobalVariable);

	GameCore::Instance().AddGameState(new GameRunStage); //MainMenu;
}

extern "C" __declspec(dllexport) void UnloadPlugin()
{
	ResourceManager::Instance().Remove(GLOBAL_VAR_P1_CHOOSEN);
	ResourceManager::Instance().Remove(GLOBAL_VAR_P2_CHOOSEN);
	ResourceManager::Instance().Remove(GLOBAL_VAR_STAGE_CHOOSEN);
	ResourceManager::Instance().Remove(GLOBAL_VAR_PLAY_MODE);
}

#include "DebugEngine/DebugNewOff.h"
