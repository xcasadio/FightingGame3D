#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <d3d9.h>
#include <d3dx9core.h>

#include "Camera.h"
#include "Texture.h"
#include "WorldPosition.h"
#include "MeshX.h"

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{

	//TODO : Utiliser les buffer<T>
	//TODO : supprimer les WorldPosition car juste calcule position avec camera

	class GAME_ENGINE_EXPORT cSkyBox
	{
	public:
		typedef enum eSkyBoxSide
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT,
			FRONT,
			BACK
		} eSkyBoxSide;

		/**
		 *
		 */
		cSkyBox(void);

		/**
		 *
		 */
		~cSkyBox(void);

		/**
		 *
		 */
		bool Create();

		/**
		 *
		 */
		bool Free();

		/**
		 * Charge une texture pour une face du cube. Prise en charge de
		 * la transparence et des different format de DirectX
		 */
		bool LoadTexture(eSkyBoxSide side, const char* fileName, D3DCOLOR transparent = 0, D3DFORMAT format = D3DFMT_UNKNOWN);

		/**
		 * Affiche le sky box (alpha autorisé) centré sur la camera
		 */
		bool Render(ICamera& camera, bool alpha = false);

		/**
		 *
		 */
		void SetScale(float scale_);

	private:
		typedef struct sSkyBoxVertex
		{
			float x, y, z;
			float u, v;
		} sSkyBoxVertex;

		const DWORD SKYBOX_FVF;

		Texture m_Textures[6];
		IDirect3DVertexBuffer9* m_pVB;
		float m_fScale;
	};


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	//TODO : pour skyDome et skySphere il faut prevoir plusieur niveau
	// de modelisation ( LOW, MIDDLE, HIGH )

	//TODO : skydome
	class GAME_ENGINE_EXPORT cSkyDome
	{
	public:

		/**
		 *
		 */
		cSkyDome(void);

		/**
		 *
		 */
		~cSkyDome(void);

		/**
		 *
		 */
		bool Create();

		/**
		 *
		 */
		bool Free();

		/**
		 * Charge une texture pour une face du cube. Prise en charge de
		 * la transparence et des different format de DirectX
		 */
		bool LoadTexture(const char* fileName, D3DCOLOR transparent = 0, D3DFORMAT format = D3DFMT_UNKNOWN);

		/**
		 * Affiche le sky box (alpha autorisé) centré sur la camera
		 */
		bool Render(ICamera& camera, bool alpha = false);

		/**
		 *
		 */
		void SetScale(float scale_);

	private:
		typedef struct sSkyDomeVertex
		{
			float x, y, z;
			float u, v;
		} sSkyDomeVertex;

		const DWORD SKYDOME_FVF;

		Texture m_Texture;
		IDirect3DVertexBuffer9* m_pVB;
		float m_fScale;
	};

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	/**
	 *
	 */
	class GAME_ENGINE_EXPORT cSkySphere
	{
	public:

		/**
		 *
		 */
		cSkySphere(void);

		/**
		 *
		 */
		~cSkySphere(void);

		/**
		 *
		 */
		bool LoadTexture(const char* fileName, D3DCOLOR transparent = 0, D3DFORMAT format = D3DFMT_UNKNOWN);

		/**
		 * Affiche le sky box (alpha autorisé) centré sur la camera
		 */
		void Render(ICamera& camera, bool alpha = false);

		/**
		 *
		 */
		void SetScale(float scale_);

	private:
		Texture m_Texture;
		MeshX m_MeshX;
		float m_fScale;
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _SKYBOX_H_
