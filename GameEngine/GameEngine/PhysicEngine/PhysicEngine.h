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
	class GAME_ENGINE_EXPORT PhysicParameters
		: public CoordinateSpace
	{
	public:
		PhysicParameters();
		~PhysicParameters();

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
	class GAME_ENGINE_EXPORT PhysicEngine :
		public ISingleton<PhysicEngine>
	{
		MAKE_SINGLETON(PhysicEngine)

			friend class PhysicParameters;

	public:
		/**
		 *
		 */
		~PhysicEngine();

		/**
		 *
		 */
		PhysicSpaceWarp* AddSpaceWarp(eSpaceWarpType type_);

	protected:
		std::deque<PhysicSpaceWarp> m_SpaceWarp;
		std::set<PhysicParameters*> m_ParametersObjects;

	private:
		/**
		 *
		 */
		PhysicEngine(void);

		/**
		 *
		 */
		void AddObject(PhysicParameters* param_);

		/**
		 *
		 */
		void DeleteObject(PhysicParameters* param_);
	};

} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"
