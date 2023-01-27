#ifndef _MESHX_H_
#define _MESHX_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Config/Export.h"
#include "Utilities/Serializable.h"
#include "Resource.h"

//#include "Object3D.h"
#include "ParticleEngine/ParticleObject.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * La classe MeshX permet de gerer les structures 
	 * ID3DXMESH de Direct3D
	 */
	class GAME_ENGINE_EXPORT MeshX : 
		public IResource,
		/*virtual public IVisualObject,*/
		public IParticleVisualObject,
		virtual public ISerializable
	{
	public:
		/**
		 *
		 */
		MeshX(void);

		/**
		 *
		 */
		virtual ~MeshX(void);

		/**
		 * @is flux d'entrée
		 * @return le nombre d'octet lu
		 */
		virtual int Read( std::istream &is );

		/**
		 * @os flux de sortie
		 * @return le meme flux modifié
		 */
		std::ostream &operator >> ( std::ostream &os );

		/**
		 * Charge un fichier mesh au format .X
		 * Pris en compte : material et texture
		 * @param fileName le nom du fichier à télécharger
		 * @return true si reussi sinon false
		 */
		bool Load(const char *fileName);

		/** 
		 * Libere les ressources allouées
		 */
		void Free();

		/**
		 * Affiche le mesh
		 */
		void Render();

		/**
		 * Affiche le mesh
		 */
		void Render( D3DMATERIAL9 *pMat_, bool bDrawTexture_);

		/**
		 * @return Retourne le mesh
		 */
		LPD3DXMESH GetMesh();

		/**
		 * Il peut y avoir plusieurs materiaux. Pour connaitre le nombre il faute appeler
		 * la fonction GetMaterialNumber()
		 * @return Retourne un D3DMATERIAL9* sur LES materiaux
		 */
		D3DMATERIAL9 *GetMaterials();

		/**
		 * Il peut y avoir plusieurs textures. Pour connaitre le nombre il faute appeler
		 * la fonction GetMaterialNumber()
		 * @return Retourne un LPDIRECT3DTEXTURE9* sur LES textures
		 */
		LPDIRECT3DTEXTURE9 *GetTextures();

		/**
		 * @return le nombre de materiaux (= nombre de textures)
		 */
		DWORD GetMaterialNumber() const;

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
		 * @return true si le mesh à été correctement chargé
		 */
		bool IsLoaded();

	protected:
		LPD3DXMESH              m_pMesh; // Our mesh object in sysmem
		D3DMATERIAL9           *m_pMeshMaterials; // Materials for our mesh
		LPDIRECT3DTEXTURE9     *m_pMeshTextures; // Textures for our mesh
		DWORD                   m_dwNumMaterials;   // Number of mesh materials

		bool m_bIsLoaded;

	private:
		
	};

	/**
	 * La classe cBoundingForm permet est la classe de base
	 * de tous les autres types de Bounding
	 */
	class GAME_ENGINE_EXPORT BoundingForm
	{
	public:
		typedef enum eBoundingForms
		{
			BOUNDING_UNDEFINED = 0,
			BOUNDING_BOX,
			BOUNDING_SPHERE,
		} eBoundingForms;

		BoundingForm();

		virtual void Compute(IDirect3DVertexBuffer9 *pVB_, DWORD NumVertices, DWORD FVF) = 0;
		virtual void Compute(LPD3DXMESH mesh_);
		virtual void Compute(MeshX &mesh);

		eBoundingForms GetType() const;
		void SetType(eBoundingForms type_);

	protected:
		eBoundingForms m_Type;
		//TODO scale - rotation
	};

	/**
	 * La classe BoundingBox definit un bounding box
	 * Le bounding box est definit par le point minimum
	 * et par le point maximum
	 */
	class GAME_ENGINE_EXPORT BoundingBox
		: public BoundingForm
	{
	public:
		BoundingBox();

		virtual void Compute(IDirect3DVertexBuffer9 *pVB_, DWORD NumVertices, DWORD FVF);

		void SetMin(D3DXVECTOR3 &min_);
		D3DXVECTOR3 GetMin() const;
		void SetMax(D3DXVECTOR3 &max_);
		D3DXVECTOR3 GetMax() const;

	private:
		D3DXVECTOR3 m_Min, m_Max;	
	};

	/**
	 * La classe BoundingBox definit une bounding sphere
	 */
	class GAME_ENGINE_EXPORT BoundingSphere
		: public BoundingForm
	{
	public:
		BoundingSphere();

		virtual void Compute(IDirect3DVertexBuffer9 *pVB_, DWORD NumVertices, DWORD FVF);

		void SetCenter(D3DXVECTOR3 &center_);
		D3DXVECTOR3 GetCenter() const;
		void SetRadius(float radius_);
		float GetRadius() const;
	private:
		D3DXVECTOR3 m_Center;
		float m_Radius;
	};

}// namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif //_MESHX_H_
