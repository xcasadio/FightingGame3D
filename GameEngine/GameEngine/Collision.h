/**
 * Les fonctions de collisions
 *
 * créé le 01/09
 * @author      Xavier Casadio
 * @version     0.1 (01/09)
 */
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "Config/Export.h"

#include "MeshX.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

 //a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Classe qui definit que des fonctions static.
	 */
	class GAME_ENGINE_EXPORT Collision
	{
	public:
		/**
		 * Collision entre 2 bounding box alignées
		 * @param box1 une reference sur un objet BoundingBox
		 * @param box1Pos une reference sur la position de box1
		 * @param box2 une reference sur un objet BoundingBox
		 * @param box2Pos une reference sur la position de box2
		 * @return true si il y a collision sinon retourne false
		 */
		static bool AxisAlignedBoundingBox(BoundingBox& box1, D3DXVECTOR3& box1Pos, BoundingBox& box2, D3DXVECTOR3& box2Pos);

		/**
		 * Collision entre 2 bounding box orientées
		 * @param box1 une reference sur un objet BoundingBox
		 * @param box1Mat une reference sur la matrice de transformation de box1
		 * @param box2 une reference sur un objet BoundingBox
		 * @param box2Mat une reference sur la matrice de transformation de box2
		 * @return true si il y a collision sinon retourne false
		 */
		static bool OrientedBoundingBox(BoundingBox& box1, D3DXMATRIX& box1Mat, BoundingBox& box2, D3DXMATRIX& box2Mat);

		/**
		 * Collision entre 2 bounding box alignées
		 * @param sphere1 une reference sur un objet BoundingBox
		 * @param sphere1Pos une reference sur la position de box1
		 * @param sphere2 une reference sur un objet BoundingBox
		 * @param sphere2Pos une reference sur la position de box2
		 * @return true si il y a collision sinon retourne false
		 */
		static bool BetweenSpheres(BoundingSphere& sphere1, D3DXVECTOR3& sphere1Pos, BoundingSphere& sphere2, D3DXVECTOR3& sphere2Pos);
	};
} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _COLLISION_H_
