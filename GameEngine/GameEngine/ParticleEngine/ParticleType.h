#pragma once

#include <iostream>

namespace GameEngine
{

	/**
	 * L'etat d'une particule
	 */
	typedef struct sParticle
	{
		D3DXVECTOR3 vPos;
		D3DCOLORVALUE color;
		unsigned long life, lifeEnd;//durée de vie
		D3DXVECTOR3 vectDir;
		float tanAcc; // tangential acceleration
		float speed;
		float size;
	} sParticle;

} // namespace GameEngine
