#pragma once

#include "../Config/Export.h"
#include "../CoordinateSpace.h"
#include "../Utilities/Singleton.h"

#include "PhysicSpaceWarp.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <deque>
#include <set>

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{

	/**
	 * Les variables necessaires pour les calculs de la physique.
	 */
	class GAME_ENGINE_EXPORT cPhysicParameters
		: public CoordinateSpace
	{
	public:
		cPhysicParameters();
		~cPhysicParameters();
		
	protected:
		D3DXVECTOR3 m_vDir, m_vAcc;
		D3DXVECTOR3 m_vPrevDir, m_vPrevAcc;
		CoordinateSpace m_PrevCoord;
		float m_fWeight;
	};

	//=====================================================================================
	//=====================================================================================

	/**
	 * Le moteur de la physique en singleton
	 */
	class GAME_ENGINE_EXPORT cPhysicEngine :
		public ISingleton<cPhysicEngine>
	{
		MAKE_SINGLETON(cPhysicEngine)

		friend class cPhysicParameters;

	public:
		/**
		 *
		 */
		~cPhysicEngine(void);

		/**
		 *
		 */
		cPhysicSpaceWarp *AddSpaceWarp( eSpaceWarpType type_ );

	protected:
		std::deque<cPhysicSpaceWarp> m_SpaceWarp;
		std::set<cPhysicParameters *> m_ParametersObjects;
		
	private:
		/**
		 *
		 */
		cPhysicEngine(void);

		/**
		 *
		 */
		void AddObject( cPhysicParameters *param_ );

		/**
		 *
		 */
		void DeleteObject( cPhysicParameters *param_ );
	};

} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"
