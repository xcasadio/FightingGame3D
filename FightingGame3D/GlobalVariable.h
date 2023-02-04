#pragma once

#include "Resource.h"
#include "FG_Export.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

#define GLOBAL_VAR_P1_CHOOSEN "p1choosen"
#define GLOBAL_VAR_P2_CHOOSEN "p2choosen"
#define GLOBAL_VAR_STAGE_CHOOSEN "stagechoosen"
#define GLOBAL_VAR_PLAY_MODE "playmode"

class FIGHTING_GAME_EXPORT GlobalVariable :
	public IResource
{
public:

	/**
	 *
	 */
	long GetVariable() const;

	/**
	 *
	 */
	void SetVariable(const long var_);

protected:
	long m_Var;
};

#include "DebugEngine/DebugNewOff.h"
