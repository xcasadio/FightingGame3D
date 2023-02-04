#include "PhysicEngine.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	SINGLETON_IMPL(PhysicEngine)

		PhysicParameters::PhysicParameters()
	{
		PhysicEngine::Instance().AddObject(this);
	}

	PhysicParameters::~PhysicParameters()
	{
		PhysicEngine::Instance().DeleteObject(this);
	}

	//=====================================================================================
	//=====================================================================================
	PhysicEngine::PhysicEngine()
	{
	}

	PhysicEngine::~PhysicEngine()
	{
	}

	/**
	 *
	 */
	PhysicSpaceWarp* PhysicEngine::AddSpaceWarp(eSpaceWarpType type_)
	{
		switch (type_)
		{
		case SPACEWARP_TYPE_DEFLECTOR:
		{
			auto pDeflector = new PhysicDeflector();
			//m_SpaceWarp( 
		}
		break;

		case SPACEWARP_TYPE_FORCE:
		{

		}
		break;
		}

		return nullptr;
	}

	/**
	 *
	 */
	void PhysicEngine::AddObject(PhysicParameters* param_)
	{
		m_ParametersObjects.insert(param_);
	}

	/**
	 *
	 */
	void PhysicEngine::DeleteObject(PhysicParameters* param_)
	{
		auto it = m_ParametersObjects.find(param_);

		if (it == m_ParametersObjects.end())
		{
			return;
		}

		m_ParametersObjects.erase(it);
	}

} // namespace gameEngine
