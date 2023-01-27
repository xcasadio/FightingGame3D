#include "PhysicEngine.h"

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	SINGLETON_IMPL(cPhysicEngine)

	cPhysicParameters::cPhysicParameters()
	{
		cPhysicEngine::Instance().AddObject( this );
	}

	cPhysicParameters::~cPhysicParameters()
	{
		cPhysicEngine::Instance().DeleteObject( this );
	}

	//=====================================================================================
	//=====================================================================================
	cPhysicEngine::cPhysicEngine(void)
	{
	}

	cPhysicEngine::~cPhysicEngine(void)
	{
	}

	/**
	 *
	 */
	cPhysicSpaceWarp *cPhysicEngine::AddSpaceWarp( eSpaceWarpType type_ )
	{
		switch ( type_ )
		{
		case SPACEWARP_TYPE_DEFLECTOR:
			{
				cPhysicDeflector *pDeflector = new cPhysicDeflector();
				//m_SpaceWarp( 
			}
			break;

		case SPACEWARP_TYPE_FORCE:
			{

			}
			break;
		}

		return NULL;
	}

	/**
	 *
	 */
	void cPhysicEngine::AddObject( cPhysicParameters *param_ )
	{
		m_ParametersObjects.insert( param_ );
	}

	/**
	 *
	 */
	void cPhysicEngine::DeleteObject( cPhysicParameters *param_ )
	{
		std::set<cPhysicParameters *>::iterator it = m_ParametersObjects.find( param_ );

		if ( it == m_ParametersObjects.end() )
		{
			return;
		}

		m_ParametersObjects.erase( it );
	}

} // namespace gameEngine
