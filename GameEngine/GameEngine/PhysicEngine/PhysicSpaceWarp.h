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
	class GAME_ENGINE_EXPORT cPhysicSpaceWarp :
		public CoordinateSpace
	{
	public:
		/*
		cPhysicSpaceWarp(void);
		virtual ~cPhysicSpaceWarp(void);
		*/

	protected:
		eSpaceWarpType m_Type;
	};

	//=====================================================================================
	//=====================================================================================

	/**
	 * Les objets statiques de collisions
	 */
	class GAME_ENGINE_EXPORT cPhysicDeflector :
		public cPhysicSpaceWarp
	{
	public:
		cPhysicDeflector(void);
		//virtual ~cPhysicDeflector(void);

	protected:
		float m_Bounce;
		float m_Friction;
	};

	//=====================================================================================
	//=====================================================================================

	/**
	 * Les forces appliquées aux mondes (ex: vent, gravité)
	 */
	class GAME_ENGINE_EXPORT cPhysicForce :
		public cPhysicSpaceWarp
	{
	public:
		cPhysicForce(void);
		//virtual ~cPhysicForce(void);

	protected:
		float m_fValue;
	};

} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"
