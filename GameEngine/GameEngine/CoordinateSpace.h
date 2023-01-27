#pragma once

#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

namespace GameEngine
{

	/**
	 *
	 */
	 //TODO : struct en public ou class en protected ????????
	class GAME_ENGINE_EXPORT CoordinateSpace
	{
	public:

	protected:
		D3DXVECTOR3 m_vPos, m_vScale;
		D3DXQUATERNION m_qRot;
	};

} // namespace GameEngine
