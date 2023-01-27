#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include <string>

#include "MeshX.h"
#include "MeshHierarchy.h"
#include "Config/Export.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

class GAME_ENGINE_EXPORT CharacterCollision
{
public:
	CharacterCollision();
	CharacterCollision(const CharacterCollision& collision);
	CharacterCollision& operator = (const CharacterCollision& collision);

	void SetHierarchy(LPD3DXFRAME pFrame);
	std::string GetBoneName();

	bool Collision(CharacterCollision* pColl);

	D3DXMATRIX& GetMatrix();
	D3DXMATRIX& GetMatrixForDisplay();

private:
	std::string m_BoneName;
	BoundingBox m_BoundingBox;
	LPD3DXFRAME m_pFrame;

	D3DXMATRIX m_MatWorld;

	void Copy(const CharacterCollision& collision);
};

#include "DebugEngine/DebugNewOff.h"
