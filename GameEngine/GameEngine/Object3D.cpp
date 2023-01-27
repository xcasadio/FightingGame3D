#include "Object3D.h"

#include "MeshX.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/*
	IVisualObject::IVisualObject(void)
	{

	}

	IVisualObject::~IVisualObject(void)
	{

	}
	*/

	
	Object3D::Object3D(void)
	{
		m_pMesh = NULL;
	}

	Object3D::~Object3D(void)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	bool Object3D::Load( const char fileName_ )
	{
		return false;
	}
	
	void Object3D::Render()
	{
		
	}
/*
	WorldPosition &cObject3D::GetWorldPosition()
	{
		return m_PosWorld;
	}
*/
} // fin namespace GameEngine
