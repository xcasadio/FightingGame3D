#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "../WorldPosition.h"
#include "../Object3D.h"
#include "../Utilities/Serializable.h"
#include "../Utilities/Singleton.h"
#include "../Config/Export.h"

#include "ParticleType.h"
#include "ParticleObject.h"
#include "ParticleBehaviour.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <string>

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	class cParticleSystem;

	/**
	 * Gere plusieurs system de particule
	 */
	class GAME_ENGINE_EXPORT cParticleSystemManager :
		public ISingleton<cParticleSystemManager>
	{
		MAKE_SINGLETON(cParticleSystemManager);

	public:
		/**
		 *
		 */
		~cParticleSystemManager();

		/**
		 * Ajout d'un system de particule. Le delete est géré par la classe
		 */
		void Add(cParticleSystem* pParticleSystem_);

		/**
		 *
		 */
		cParticleSystem* GetParticleSystem(DWORD id_);

		/**
		 *
		 */
		void Render();

		/**
		 *
		 */
		void Update(float time_);

		/**
		 *
		 */
		void Clear();

	private:
		std::vector<cParticleSystem*> m_ParticleSystem;
	};

	//=====================================================================================
	//=====================================================================================

	/**
	 * Definit un systeme de particule
	 * On peut definir indépendamment le comportement (TODO)
	 * et la particule elle meme. (class IParticleVisualObject)
	 */
	class GAME_ENGINE_EXPORT cParticleSystem :
		public ISerializable
	{
	public:
		/**
		 *
		 */
		cParticleSystem(void);

		/**
		 *
		 */
		~cParticleSystem(void);

		/**
		 * @is flux d'entrée
		 * @return le nombre d'octet lu
		 */
		virtual int Read(std::istream& is);

		/**
		 * @os flux de sortie
		 * @return le meme flux modifié
		 */
		std::ostream& operator >> (std::ostream& os);

		/**
		 *
		 */
		void Render();

		/**
		 *
		 */
		void Update(float time_);

		/**
		 *
		 */
		void Start();

		/**
		 *
		 */
		size_t GetNumParticle() const;

		/**
		 *
		 */
		void SetEmissionPoint(float x, float y, float z);

		/**
		 *
		 */
		D3DXVECTOR3 GetEmissionPoint() const;

		/**
		 *
		 */
		void SetSpeed(float speedMin, float speedMax);

		/**
		 *
		 */
		void GetSpeed(float* speedMin, float* speedMax) const;

		/**
		 *
		 */
		void SetLife(unsigned int life_);

		/**
		 *
		 */
		unsigned int GetLife() const;

		/**
		 *
		 */
		void SetParticleLife(unsigned int lifeMin, unsigned int lifeMax);

		/**
		 *
		 */
		void GetParticleLife(unsigned int* lifeMin, unsigned int* lifeMax) const;

		/**
		 *
		 */
		void SetSize(float sizeStart, float sizeEnd);

		/**
		 *
		 */
		void GetSize(float* sizeStart, float* sizeEnd) const;

		/**
		 *
		 */
		void SetColorStart(float a, float r, float g, float b);

		/**
		 *
		 */
		D3DCOLORVALUE GetColorStart() const;

		/**
		 *
		 */
		void SetColorEnd(float a, float r, float g, float b);

		/**
		 *
		 */
		D3DCOLORVALUE GetColorEnd() const;

		/**
		 *
		 */
		void SetParticleMax(unsigned int max);

		/**
		 *
		 */
		unsigned int GetParticleMax() const;

		/**
		 *
		 */
		void SetEmissionPerSec(unsigned int emissionPerSec);

		/**
		 *
		 */
		unsigned int GetEmissionPerSec() const;

		/**
		 *
		 */
		void SetContinuous(bool continuous);

		/**
		 *
		 */
		bool GetContinuous() const;

		/**
		 *
		 */
		void SetSpread(D3DXVECTOR3& spread_);

		/**
		 *
		 */
		D3DXVECTOR3 GetSpread() const;

		/**
		 *
		 */
		void SetDirection(D3DXVECTOR3& vect_);

		/**
		 *
		 */
		D3DXVECTOR3 GetDirection() const;

		/**
		 *
		 */
		void SetId(DWORD id_);

		/**
		 *
		 */
		DWORD GetId() const;

		/**
		 *
		 */
		void SetWorldPosition(D3DXVECTOR3& vPos_);

		//TODO
		//void SetTangentialAcceleration(float min, float max);

		/**
		 *
		 */
		void SetVisualObject(IParticleVisualObject* pVisualObject_);

		/**
		 *
		 */
		IParticleVisualObject* GetParticleVisualObject();

	private:
		IParticleVisualObject* m_pVisualObject;

		std::vector<sParticle> m_Particles;

		DWORD m_Id;

		D3DVECTOR m_vEmissionPoint;
		D3DCOLORVALUE m_ColorStart, m_ColorEnd;
		D3DXVECTOR3 m_vSpread;
		D3DXVECTOR3 m_vVectDir;
		float m_fSpeedMin, m_fSpeedMax;
		unsigned int m_ParticleSystemLife;
		unsigned int m_LifeMin, m_LifeMax;
		float m_fSizeStart, m_fSizeEnd;
		unsigned int m_NbParticleMax;
		unsigned int m_EmissionPerSec;//nb particle par seconde
		unsigned long m_Timer;
		bool m_bContinuous; //si on emet en continu
		bool m_bMaxAttained; // si le max a été atteint en cas d'emission non continue

		bool m_bIsBillBoard;

		D3DXVECTOR3 m_vPos;

		//TODO
		float m_fTanAccMin, m_fTanAccMax;//Tangential Acceleration

		float m_fEmissionPerSecError;

		IParticleSystemBehaviour* m_Behaviour;

		/**
		 *
		 */
		void CreateParticle();// créer une nouvelle particule
	};

} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"

#endif // _PARTICLESYSTEM_H_
