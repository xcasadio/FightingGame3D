#pragma once

#include "WorldPosition.h"
#include "Object3D.h"
#include "Utilities/Serializable.h"
#include "Utilities/Singleton.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <string>

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
		void Add( cParticleSystem *pParticleSystem_ );

		/**
		 *
		 */
		cParticleSystem *GetParticleSystem( DWORD id_ );

		/**
		 *
		 */
		void Render();

		/**
		 *
		 */
		void Update( float time_ );

		/**
		 *
		 */
		void Clear();

	private:
		std::vector<cParticleSystem *> m_ParticleSystem;
	};

	//////////////////////////////////////////////////////////////////
	/**
	 *
	 */
	class GAME_ENGINE_EXPORT IParticleVisualObject : 
		public IVisualObject, 
		public ISerializable
	{
	public:
		/**
		 * La fonction d'affichage des particules
		 */
		virtual void Render() = 0;

		/**
		 * Pour recuperer les materiaux. Utiliser pour changer la couleur
		 * des particules
		 * @return un tableau de D3DMATERIAL9
		 */
		virtual D3DMATERIAL9 *GetMaterials() = 0;

		/**
		 * @return le nombre de materiaux, ( utile pour GetMaterials() )
		 */
		virtual DWORD GetMaterialNumber() const = 0;

		/**
		 * Definit le mode d'affichage utilisé pour les particules
		 * exemple : texture, alpha, bend... 
		 */
		virtual void SetRenderMode() = 0;

		/**
		 * Permet de "deconfigurer" le mode d'affichage des particules
		 * exemple : on sauvegarde les precedents parametres puis on les restorent
		 */
		virtual void UnsetRenderMode() = 0;

		/**
		 * Permet de charger le fichier de serialisation
		 * pour initialiser un objet
		 */
		//virtual bool LoadSerialized(const char *fileName_);

		/**
		 * Permet sauvegarder (serialisation) l'etat
		 * de l'objet courant
		 */
		//virtual bool SaveSerialized(const char *fileName_);

		/**
		 * @is flux d'entrée
		 * @return le nombre d'octet lu
		 */
		virtual int Read( std::istream &is );

		/**
		 * @os flux de sortie
		 * @return le meme flux modifié
		 */
		virtual std::ostream &operator << ( std::ostream &os );

		/**
		 *
		 */
		void SetAlphaEnabled(bool value_);

		/**
		 *
		 */
		bool GetAlphaEnabled();

		/**
		 *
		 */
		void SetSrcBlend( DWORD value_);

		/**
		 *
		 */
		DWORD GetSrcBlend();

		/**
		 *
		 */
		void SetDestBlend( DWORD value_);

		/**
		 *
		 */
		DWORD GetDestBlend();

		/**
		 *
		 */
		void SetLighting( bool value_);

		/**
		 *
		 */
		bool GetLighting();

		/**
		 *
		 */
		void SetZWrite( bool value_);

		/**
		 *
		 */
		bool GetZWrite();

	protected:
		DWORD m_AlphaBlendEnable, m_SrcBlend, m_DestBlend, m_Lighting, m_ZWriteEnable;
		unsigned char m_AmbientLightR, m_AmbientLightB, m_AmbientLightG; 
	};
	
	//////////////////////////////////////////////////////////////////
	/**
	 * Definit un systeme de particule
	 * On peut definir indépendamment le comportement (TODO)
	 * et la particule elle meme. (class IParticleVisualObject)
	 */
	class GAME_ENGINE_EXPORT cParticleSystem : 
		public ISerializable
	{
	public:
		//friend class IParticleSystemBehaviour;

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

		/**
		 *
		 */
		cParticleSystem(void);

		/**
		 *
		 */
		~cParticleSystem(void);

		//bool LoadSerialized(const char *fileName_);
		//virtual bool SaveSerialized(const char *fileName_);
		/**
		 * @is flux d'entrée
		 * @return le nombre d'octet lu
		 */
		virtual int Read( std::istream &is );

		/**
		 * @os flux de sortie
		 * @return le meme flux modifié
		 */
		virtual std::ostream &operator << ( std::ostream &os );

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
		void GetSpeed(float *speedMin, float *speedMax) const;

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
		void GetParticleLife(unsigned int *lifeMin, unsigned int *lifeMax) const;

		/**
		 *
		 */
		void SetSize(float sizeStart, float sizeEnd);

		/**
		 *
		 */
		void GetSize(float *sizeStart, float *sizeEnd) const;

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
		void SetEmissionPerSec( unsigned int emissionPerSec);

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
		void SetSpread( D3DXVECTOR3 &spread_);

		/**
		 *
		 */
		D3DXVECTOR3 GetSpread() const;

		/**
		 *
		 */
		void SetDirection(D3DXVECTOR3 &vect_);

		/**
		 *
		 */
		D3DXVECTOR3 GetDirection() const;

		/**
		 *
		 */
		void SetId( DWORD id_ );

		/**
		 *
		 */
		DWORD GetId() const;

		/**
		 *
		 */
		void SetWorldPosition( D3DXVECTOR3 &vPos_ );

		//TODO
		//void SetTangentialAcceleration(float min, float max);

		void SetVisualObject( IParticleVisualObject *pVisualObject_);

		IParticleVisualObject *GetParticleVisualObject();

	private:
		IParticleVisualObject *m_pVisualObject;

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

		IParticleSystemBehaviour *m_Behaviour;

		/**
		 *
		 */
		void CreateParticle();// créer une nouvelle particule
	};

	//////////////////////////////////////////////////////////////////
	/**
	 * Definit une particule de type billboard
	 * TODO : utiliser des sprite avec D3DXSPRITE_BILLBOARD ??
	 */
	class GAME_ENGINE_EXPORT cParticleBillBoard :
		public IParticleVisualObject,
		virtual public ISerializable
	{
	public:
		typedef enum eParticleBillBoardParameters
		{
			PARTICLE_BILLBOARD_PARAM_FIRE = 0,
			PARTICLE_BILLBOARD_PARAM_FOG,
		} eParticleBillBoardParameters;

		typedef struct sParticleVertex
		{
			FLOAT x, y, z;    // 3D coordinates
			D3DCOLOR Diffuse; // Color
			FLOAT u, v;       // Texture coordinates
		} sParticleVertex;
		

		/**
		 *
		 */
		cParticleBillBoard();

		/**
		 *
		 */
		~cParticleBillBoard();

		/**
		 * @is flux d'entrée
		 * @return le nombre d'octet lu
		 */
		virtual int Read( std::istream &is );

		/**
		 * @os flux de sortie
		 * @return le meme flux modifié
		 */
		virtual std::ostream &operator << ( std::ostream &os );

		/**
		 * Definit le mode d'affichage utilisé pour les particules
		 * exemple : texture, alpha, bend... 
		 */
		virtual void SetRenderMode();

		/**
		 * Permet de "deconfigurer" le mode d'affichage des particules
		 * exemple : on sauvegarde les precedents parametres puis on les restorent
		 */
		virtual void UnsetRenderMode();


		/**
		 *
		 */
		bool Load(const char *fileName_);

		/**
		 *
		 */
		void Render();

		/**
		 *
		 */
		D3DMATERIAL9 *GetMaterials();

		/**
		 *
		 */
		DWORD GetMaterialNumber() const;


		/**
		 *
		 */
		void Configuration( eParticleBillBoardParameters param_);

		/**
		 *
		 */
		void Configuration( DWORD alphaBlendEnable_, DWORD srcBlend_, DWORD destBlend_, bool lighting_, bool zWriteEnable_);

	private:
		const DWORD PARTICLE_FVF;

		IDirect3DVertexBuffer9 *m_pParticleVB;//vertex buffer
		IDirect3DTexture9 *m_pParticleTexture;
		D3DMATERIAL9 m_Mat;

		std::string m_FileName;

		void Free();
	};

	//////////////////////////////////////////////////////////////////
	/**
	 * Definit la classe abstraire pour le comportement d'un system de particule
	 */
	class GAME_ENGINE_EXPORT IParticleSystemBehaviour // ou Physic
	{
	public:
		/**
		 *
		 */
		virtual void Update() = 0;
	};

} // namespace GameEngine
