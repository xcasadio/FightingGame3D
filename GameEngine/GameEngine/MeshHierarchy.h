#ifndef _MESHHIERARCHY_H_
#define _MESHHIERARCHY_H_

#include <d3d9.h>
#include <d3dx9.h>

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/** enum for various skinning modes possible */
	enum GAME_ENGINE_EXPORT METHOD
	{
		D3DNONINDEXED,
		D3DINDEXED,
		SOFTWARE,
		D3DINDEXEDVS,
		D3DINDEXEDHLSLVS,
		NONE
	};

	/**
	 * Desc: Structure derived from D3DXFRAME so we can add some app-specific
	 *       info that will be stored with each frame
	 */
	struct GAME_ENGINE_EXPORT D3DXFRAME_DERIVED : public D3DXFRAME
	{
		D3DXMATRIXA16        CombinedTransformationMatrix;
	};

	/**
	 * Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
	 *       info that will be stored with each mesh
	 */
	struct GAME_ENGINE_EXPORT D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
	{
		LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    
	                                
		// SkinMesh info             
		LPD3DXMESH           pOrigMesh;
		LPD3DXATTRIBUTERANGE pAttributeTable;
		DWORD                NumAttributeGroups; 
		DWORD                NumInfl;
		LPD3DXBUFFER         pBoneCombinationBuf;
		D3DXMATRIX**         ppBoneMatrixPtrs;
		D3DXMATRIX*          pBoneOffsetMatrices;
		DWORD                NumPaletteEntries;
		bool                 UseSoftwareVP;
		DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
	};


	/** Name: class cAllocateHierarchy
	 * Desc: Custom version of ID3DXAllocateHierarchy with custom methods to create
	 *       frames and meshcontainers.
	 */
	class GAME_ENGINE_EXPORT cAllocateHierarchy: public ID3DXAllocateHierarchy
	{
	public:
		cAllocateHierarchy();
		~cAllocateHierarchy();

		void SetSkinningMethod( METHOD method_);
		METHOD GetMethod();

		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
		STDMETHOD(CreateMeshContainer)(THIS_ 
			LPCSTR Name, 
			CONST D3DXMESHDATA *pMeshData,
			CONST D3DXMATERIAL *pMaterials, 
			CONST D3DXEFFECTINSTANCE *pEffectInstances, 
			DWORD NumMaterials, 
			CONST DWORD *pAdjacency, 
			LPD3DXSKININFO pSkinInfo, 
			LPD3DXMESHCONTAINER *ppNewMeshContainer);
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

		/**
		 *
		 */
		static void UpdateSkinningMethod( cAllocateHierarchy *pAlloc, LPD3DXFRAME pFrameBase, METHOD method_);

	private:
		METHOD					m_SkinningMethod;
		//TODO : utiliser D3DXMATRIXA16 pour optimisation
		D3DXMATRIX*				m_pBoneMatrices;
		UINT                    m_NumBoneMatricesMax;

		static bool m_bUseSoftwareVP;

		/**
		 *
		 */
		HRESULT AllocateName( LPCSTR pSrc_, LPSTR *ppDest_ );

		/**
		 *
		 */
		void ReleaseAttributeTable( LPD3DXFRAME pFrameBase );
		
		/**
		 *
		 */
		static HRESULT GenerateSkinnedMesh( cAllocateHierarchy *pAlloc, IDirect3DDevice9 *pd3dDevice, D3DXMESHCONTAINER_DERIVED *pMeshContainer, METHOD method_ );
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif //MESHHIERARCHY_H_
