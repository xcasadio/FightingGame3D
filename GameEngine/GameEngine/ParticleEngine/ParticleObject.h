#ifndef _PARTICLEOBJECT_H_
#define _PARTICLEOBJECT_H_

#include "../Object3D.h"
#include "../Utilities/Serializable.h"
#include "../Utilities/Singleton.h"
#include "../Config/Export.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <string>

#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
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
		virtual D3DMATERIAL9* GetMaterials() = 0;

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
		void SetAlphaEnabled(bool value_);

		/**
		 *
		 */
		bool GetAlphaEnabled();

		/**
		 *
		 */
		void SetSrcBlend(DWORD value_);

		/**
		 *
		 */
		DWORD GetSrcBlend();

		/**
		 *
		 */
		void SetDestBlend(DWORD value_);

		/**
		 *
		 */
		DWORD GetDestBlend();

		/**
		 *
		 */
		void SetLighting(bool value_);

		/**
		 *
		 */
		bool GetLighting();

		/**
		 *
		 */
		void SetZWrite(bool value_);

		/**
		 *
		 */
		bool GetZWrite();

	protected:
		DWORD m_AlphaBlendEnable, m_SrcBlend, m_DestBlend, m_Lighting, m_ZWriteEnable;
		unsigned char m_AmbientLightR, m_AmbientLightB, m_AmbientLightG;
	};

	//=====================================================================================
	//=====================================================================================

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
		virtual int Read(std::istream& is);

		/**
		 * @os flux de sortie
		 * @return le meme flux modifié
		 */
		std::ostream& operator >> (std::ostream& os);

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
		bool Load(const char* fileName_);

		/**
		 *
		 */
		void Render();

		/**
		 *
		 */
		D3DMATERIAL9* GetMaterials();

		/**
		 *
		 */
		DWORD GetMaterialNumber() const;


		/**
		 *
		 */
		void Configuration(eParticleBillBoardParameters param_);

		/**
		 *
		 */
		void Configuration(DWORD alphaBlendEnable_, DWORD srcBlend_, DWORD destBlend_, bool lighting_, bool zWriteEnable_);

	private:
		const DWORD PARTICLE_FVF;

		IDirect3DVertexBuffer9* m_pParticleVB;//vertex buffer
		IDirect3DTexture9* m_pParticleTexture;
		D3DMATERIAL9 m_Mat;

		std::string m_FileName;

		void Free();
	};

} // namespace GameEngine

#include "../DebugEngine/DebugNewOff.h"

#endif // _PARTICLEOBJECT_H_
