#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Permet de calculer si certain type d'objet sont dans
	 * le champ de vision de la camera courante
	 */
	class  GAME_ENGINE_EXPORT Frustum
	{
	public:
		/**
		 * Construct the six planes from current view and
		 * projection. Can override the default depth value.
		 */
		bool Construct(float ZDistance = 0.0f);

		/**
		 * Teste si un point est visible dans le frustum
		 * @param XPos
		 * @param YPos
		 * @param ZPos
		 * @return true si l'objet est visible, sinon false
		 */
		bool CheckPoint(float XPos, float YPos, float ZPos);

		/**
		 * Teste si un cube est visible dans le frustum
		 * @param XCenter
		 * @param YCenter
		 * @param ZCenter
		 * @param Size la moitié de la longueur d'un coté du cube
		 * @param CompletelyContained optionnel, valeur à true si le cube est completement visible sinon false
		 * @return true si l'objet est visible ou partiellement visible, sinon false
		 */
		bool CheckCube(float XCenter, float YCenter, \
			float ZCenter, float Size, \
			bool* CompletelyContained = nullptr);

		/**
		 * Teste si un rectangle est visible dans le frustum
		 * @param XCenter
		 * @param YCenter
		 * @param ZCenter
		 * @param XSize la moitié de la longueur des cotés de l'axe x
		 * @param YSize la moitié de la longueur des cotés de l'axe y
		 * @param ZSize la moitié de la longueur des cotés de l'axe z
		 * @return true si l'objet est visible ou partiellement visible, sinon false
		 */
		bool CheckRectangle(float XCenter, float YCenter, \
			float ZCenter, float XSize, \
			float YSize, float ZSize, \
			bool* CompletelyContained = nullptr);

		/**
		 * Teste si une sphere est visible dans le frustum
		 * @param XCenter
		 * @param YCenter
		 * @param ZCenter
		 * @param Radius
		 * @return true si l'objet est visible, sinon false
		 */
		bool CheckSphere(float XCenter, float YCenter, \
			float ZCenter, float Radius);

	private:
		D3DXPLANE m_Planes[6];
	};

}//namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif
