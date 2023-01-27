//#ifndef _OBJECT3D_H_
//#define _OBJECT3D_H_

#pragma once

#include <windows.h>
#include <windowsx.h>

#include "Utilities/Serializable.h"
#include "Config/Export.h"
#include "Resource.h"

#include <d3d9.h>
#include <d3dx9.h>

//#include "WorldPosition.h"
//#include "MeshX.h"

namespace GameEngine
{
	class MeshX;

	/**
	 *
	 */
	class GAME_ENGINE_EXPORT IVisualObject
	{
	public:
		//IVisualObject(void);
		//virtual ~IVisualObject(void);

		virtual void Render() = 0;
	};
	
	/**
	 *
	 */
	class GAME_ENGINE_EXPORT Object3D
		: public IResource, 
		public IVisualObject,
		public ISerializable
	{
	public:
		Object3D(void);
		virtual ~Object3D(void);

		bool Load( const char fileName_ );

		void Render();

		//WorldPosition &GetWorldPosition();

	private:
		MeshX *m_pMesh;
		//WorldPosition m_PosWorld;
	};

}// fin namespace GameEngine

//#endif // _OBJECT3D_H_
