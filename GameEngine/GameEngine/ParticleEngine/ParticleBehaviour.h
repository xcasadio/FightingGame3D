#ifndef _PARTICLEBEHAVIOUR_H_
#define _PARTICLEBEHAVIOUR_H_

#include "Config/Export.h"
#include "Object3D.h"
#include "ParticleType.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <string>

#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Definit la classe abstraire pour le comportement d'un system de particule
	 */
	class GAME_ENGINE_EXPORT IParticleSystemBehaviour // ou Physic
	{
	public:
		/**
		 *
		 */
		virtual void Update(float time_, std::vector<sParticle>& particles_) = 0;
	};
} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"

#endif // _PARTICLEBEHAVIOUR_H_
