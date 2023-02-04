#pragma once

#include "../Config/Export.h"
#include "../CoordinateSpace.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	//les differents type de space warp
	typedef enum eSpaceWarpType
	{
		SPACEWARP_TYPE_FORCE,
		SPACEWARP_TYPE_DEFLECTOR,
	} eSpaceWarpType;

	//=====================================================================================
	//=====================================================================================

	/**
	 * Les space warp sont des objets invisibles qui créés des forces qui s'appliquent au monde
	 * et pas seulement à un objet
	 */
	class GAME_ENGINE_EXPORT PhysicSpaceWarp :
		public CoordinateSpace
	{
	public:
		/*
		PhysicSpaceWarp(void);
		virtual ~PhysicSpaceWarp(void);
		*/

	protected:
		eSpaceWarpType m_Type;
	};

	//=====================================================================================
	//=====================================================================================

	/**
	 * Les objets statiques de collisions
	 */
	class GAME_ENGINE_EXPORT PhysicDeflector :
		public PhysicSpaceWarp
	{
	public:
		PhysicDeflector();
		//virtual ~PhysicDeflector(void);

	protected:
		float m_Bounce;
		float m_Friction;
	};

	//=====================================================================================
	//=====================================================================================

	/**
	 * Les forces appliquées aux mondes (ex: vent, gravité)
	 */
	class GAME_ENGINE_EXPORT PhysicForce :
		public PhysicSpaceWarp
	{
	public:
		PhysicForce();
		//virtual ~PhysicForce(void);

	protected:
		float m_fValue;
	};

} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"
