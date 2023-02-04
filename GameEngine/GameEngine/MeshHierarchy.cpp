#include "MeshHierarchy.h"

#include "GameCore.h"
#include "Graphic.h"
#include "Utilities/Macros.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	bool cAllocateHierarchy::m_bUseSoftwareVP = false;

	/**
	 *
	 */
	cAllocateHierarchy::cAllocateHierarchy()
	{
		m_SkinningMethod = D3DNONINDEXED;
		m_pBoneMatrices = nullptr;
		m_NumBoneMatricesMax = 0;
	}

	/**
	 *
	 */
	cAllocateHierarchy::~cAllocateHierarchy()
	{
		if (m_pBoneMatrices)
		{
			delete[] m_pBoneMatrices;
			m_pBoneMatrices = nullptr;
		}
	}

	/**
	 *
	 */
	void cAllocateHierarchy::SetSkinningMethod(METHOD method_)
	{
		m_SkinningMethod = method_;
	}

	/**
	 *
	 */
	METHOD cAllocateHierarchy::GetMethod()
	{
		return m_SkinningMethod;
	}

	/**
	 *
	 */
	HRESULT cAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)
	{
		HRESULT hr = S_OK;

		*ppNewFrame = nullptr;

		auto pFrame = new D3DXFRAME_DERIVED();
		if (pFrame == nullptr)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = AllocateName(Name, &pFrame->Name);
		if (FAILED(hr))
		{
			goto e_Exit;
		}

		// initialize other data members of the frame
		D3DXMatrixIdentity(&pFrame->TransformationMatrix);
		D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

		pFrame->pMeshContainer = nullptr;
		pFrame->pFrameSibling = nullptr;
		pFrame->pFrameFirstChild = nullptr;

		*ppNewFrame = pFrame;
		pFrame = nullptr;

	e_Exit:
		delete pFrame;
		return hr;
	}

	/**
	 *
	 */
	HRESULT cAllocateHierarchy::CreateMeshContainer(
		LPCSTR Name,
		CONST D3DXMESHDATA* pMeshData,
		CONST D3DXMATERIAL* pMaterials,
		CONST D3DXEFFECTINSTANCE* pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD* pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER* ppNewMeshContainer)
	{
		HRESULT hr;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = nullptr;
		LPDIRECT3DDEVICE9 pd3dDevice = nullptr;

		*ppNewMeshContainer = nullptr;

		// this sample does not handle patch meshes, so fail when one is found
		if (pMeshData->Type != D3DXMESHTYPE_MESH)
		{
			hr = E_FAIL;
			goto e_Exit;
		}

		// get the pMesh interface pointer out of the mesh data structure
		LPD3DXMESH pMesh = pMeshData->pMesh;

		// this sample does not FVF compatible meshes, so fail when one is found
		if (pMesh->GetFVF() == 0)
		{
			hr = E_FAIL;
			goto e_Exit;
		}

		// allocate the overloaded structure to return as a D3DXMESHCONTAINER
		pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
		if (pMeshContainer == nullptr)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
		memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

		// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
		hr = AllocateName(Name, &pMeshContainer->Name);
		if (FAILED(hr))
		{
			goto e_Exit;
		}

		pMesh->GetDevice(&pd3dDevice);
		UINT NumFaces = pMesh->GetNumFaces();

		// if no normals are in the mesh, add them
		if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
		{
			pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

			// clone the mesh to make room for the normals
			hr = pMesh->CloneMeshFVF(pMesh->GetOptions(),
				pMesh->GetFVF() | D3DFVF_NORMAL,
				pd3dDevice, &pMeshContainer->MeshData.pMesh);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// get the new pMesh pointer back out of the mesh container to use
			// NOTE: we do not release pMesh because we do not have a reference to it yet
			pMesh = pMeshContainer->MeshData.pMesh;

			// now generate the normals for the pmesh
			D3DXComputeNormals(pMesh, nullptr);
		}
		else  // if no normals, just add a reference to the mesh for the mesh container
		{
			pMeshContainer->MeshData.pMesh = pMesh;
			pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

			pMesh->AddRef();
		}

		// allocate memory to contain the material information.  This sample uses
		//   the D3D9 materials and texture names instead of the EffectInstance style materials
		pMeshContainer->NumMaterials = max(1, NumMaterials);
		pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
		pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
		pMeshContainer->pAdjacency = new DWORD[NumFaces * 3];
		if ((pMeshContainer->pAdjacency == nullptr) || (pMeshContainer->pMaterials == nullptr))
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces * 3);
		memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

		// if materials provided, copy them
		if (NumMaterials > 0)
		{
			memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

			for (UINT iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
			{
				if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != nullptr)
				{
					/*WCHAR strTexturePath[MAX_PATH];
					WCHAR wszBuf[MAX_PATH];
					MultiByteToWideChar( CP_ACP, 0, pMeshContainer->pMaterials[iMaterial].pTextureFilename, -1, wszBuf, MAX_PATH );
					wszBuf[MAX_PATH - 1] = L'\0';
					DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, wszBuf );*/
					if (FAILED(D3DXCreateTextureFromFile(pd3dDevice, /*strTexturePath,*/
						pMeshContainer->pMaterials[iMaterial].pTextureFilename,
						&pMeshContainer->ppTextures[iMaterial])))
					{
						pMeshContainer->ppTextures[iMaterial] = nullptr;
					}

					// don't remember a pointer into the dynamic memory, just forget the name after loading
					pMeshContainer->pMaterials[iMaterial].pTextureFilename = nullptr;
				}
			}
		}
		else // if no materials provided, use a default one
		{
			pMeshContainer->pMaterials[0].pTextureFilename = nullptr;
			memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
			pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
			pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
			pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
			pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
		}

		// if there is skinning information, save off the required data and then setup for HW skinning
		if (pSkinInfo != nullptr)
		{
			// first save off the SkinInfo and original mesh data
			pMeshContainer->pSkinInfo = pSkinInfo;
			pSkinInfo->AddRef();

			pMeshContainer->pOrigMesh = pMesh;
			pMesh->AddRef();

			// Will need an array of offset matrices to move the vertices from the figure space to the bone's space
			UINT cBones = pSkinInfo->GetNumBones();
			pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
			if (pMeshContainer->pBoneOffsetMatrices == nullptr)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			// get each of the bone offset matrices so that we don't need to get them later
			for (UINT iBone = 0; iBone < cBones; iBone++)
			{
				pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
			}

			// GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
			hr = GenerateSkinnedMesh(this, pd3dDevice, pMeshContainer, m_SkinningMethod);
			if (FAILED(hr))
			{
				goto e_Exit;
			}
		}

		*ppNewMeshContainer = pMeshContainer;
		pMeshContainer = nullptr;

	e_Exit:
		SAFE_RELEASE(pd3dDevice);

		// call Destroy function to properly clean up the memory allocated 
		if (pMeshContainer != nullptr)
		{
			DestroyMeshContainer(pMeshContainer);
		}

		return hr;
	}

	/**
	 *
	 */
	HRESULT cAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
	{
		if (pFrameToFree)
		{
			SAFE_DELETE_ARRAY(pFrameToFree->Name);
			SAFE_DELETE(pFrameToFree);
		}

		return S_OK;
	}

	/**
	 *
	 */
	HRESULT cAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
	{
		auto pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

		SAFE_DELETE_ARRAY(pMeshContainer->Name);
		SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
		SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
		SAFE_DELETE_ARRAY(pMeshContainer->pBoneOffsetMatrices);

		// release all the allocated textures
		if (pMeshContainer->ppTextures != nullptr)
		{
			for (UINT iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
			{
				SAFE_RELEASE(pMeshContainer->ppTextures[iMaterial]);
			}
		}

		SAFE_DELETE_ARRAY(pMeshContainer->ppTextures);
		SAFE_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs);
		SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);
		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
		SAFE_RELEASE(pMeshContainer->pSkinInfo);
		SAFE_RELEASE(pMeshContainer->pOrigMesh);
		SAFE_DELETE(pMeshContainer);
		return S_OK;
	}

	/**
	 * Desc: Allocates memory for a string to hold the name of a frame or mesh
	 */
	HRESULT cAllocateHierarchy::AllocateName(LPCSTR pSrc_, LPSTR* ppDest_)
	{
		if (pSrc_ != nullptr)
		{
			UINT cbLength = (UINT)strlen(pSrc_) + 1;
			*ppDest_ = new char[cbLength];
			if (*ppDest_ == nullptr)
			{
				return E_OUTOFMEMORY;
			}
			memcpy(*ppDest_, pSrc_, cbLength * sizeof(char));
		}
		else
		{
			*ppDest_ = nullptr;
		}

		return S_OK;
	}


	/**
	 * Called either by CreateMeshContainer when loading a skin mesh, or when
	 * changing methods.  This function uses the pSkinInfo of the mesh
	 * container to generate the desired drawable mesh and bone combination
	 * table.
	 */
	HRESULT cAllocateHierarchy::GenerateSkinnedMesh(cAllocateHierarchy* pAlloc, IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer, METHOD method_)
	{
		HRESULT hr = S_OK;
		D3DCAPS9 d3dCaps;
		pd3dDevice->GetDeviceCaps(&d3dCaps);

		if (pMeshContainer->pSkinInfo == nullptr)
		{
			return hr;
		}

		m_bUseSoftwareVP = false;

		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
		SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);

		// if non-indexed skinning mode selected, use ConvertToBlendedMesh to generate drawable mesh
		if (method_ == D3DNONINDEXED)
		{

			hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh
			(
				pMeshContainer->pOrigMesh,
				D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
				pMeshContainer->pAdjacency,
				nullptr, nullptr, nullptr,
				&pMeshContainer->NumInfl,
				&pMeshContainer->NumAttributeGroups,
				&pMeshContainer->pBoneCombinationBuf,
				&pMeshContainer->MeshData.pMesh
			);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
			// Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
			// drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
			auto rgBoneCombinations = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

			// look for any set of bone combinations that do not fit the caps
			for (pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; pMeshContainer->iAttributeSW++)
			{
				DWORD cInfl = 0;

				for (DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++)
				{
					if (rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX)
					{
						++cInfl;
					}
				}

				if (cInfl > d3dCaps.MaxVertexBlendMatrices)
				{
					break;
				}
			}

			// if there is both HW and SW, add the Software Processing flag
			if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
			{
				LPD3DXMESH pMeshTmp;

				hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING | pMeshContainer->MeshData.pMesh->GetOptions(),
					pMeshContainer->MeshData.pMesh->GetFVF(),
					pd3dDevice, &pMeshTmp);
				if (FAILED(hr))
				{
					goto e_Exit;
				}

				pMeshContainer->MeshData.pMesh->Release();
				pMeshContainer->MeshData.pMesh = pMeshTmp;
				pMeshTmp = nullptr;
			}
		}
		// if indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
		else if (method_ == D3DINDEXED)
		{
			DWORD NumMaxFaceInfl;
			DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

			LPDIRECT3DINDEXBUFFER9 pIB;
			hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl);
			pIB->Release();
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
			// can be handled
			NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

			if (d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl)
			{
				// HW does not support indexed vertex blending. Use SW instead
				pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
				pMeshContainer->UseSoftwareVP = true;
				m_bUseSoftwareVP = true;
				Flags |= D3DXMESH_SYSTEMMEM;
			}
			else
			{
				// using hardware - determine palette size from caps and number of bones
				// If normals are present in the vertex data that needs to be blended for lighting, then 
				// the number of matrices is half the number specified by MaxVertexBlendMatrixIndex.
				pMeshContainer->NumPaletteEntries = min((d3dCaps.MaxVertexBlendMatrixIndex + 1) / 2,
					pMeshContainer->pSkinInfo->GetNumBones());
				pMeshContainer->UseSoftwareVP = false;
				Flags |= D3DXMESH_MANAGED;
			}

			hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
			(
				pMeshContainer->pOrigMesh,
				Flags,
				pMeshContainer->NumPaletteEntries,
				pMeshContainer->pAdjacency,
				nullptr, nullptr, nullptr,
				&pMeshContainer->NumInfl,
				&pMeshContainer->NumAttributeGroups,
				&pMeshContainer->pBoneCombinationBuf,
				&pMeshContainer->MeshData.pMesh);
			if (FAILED(hr))
			{
				goto e_Exit;
			}
		}
		// if vertex shader indexed skinning mode selected, use ConvertToIndexedsBlendedMesh to generate drawable mesh
		else if ((method_ == D3DINDEXEDVS) || (method_ == D3DINDEXEDHLSLVS))
		{
			// Get palette size
			// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
			// (96 - 9) /3 i.e. Maximum constant count - used constants 
			UINT MaxMatrices = 26;
			pMeshContainer->NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());

			DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
			if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
			{
				pMeshContainer->UseSoftwareVP = false;
				Flags |= D3DXMESH_MANAGED;
			}
			else
			{
				pMeshContainer->UseSoftwareVP = true;
				m_bUseSoftwareVP = true;
				Flags |= D3DXMESH_SYSTEMMEM;
			}

			SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

			hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
			(
				pMeshContainer->pOrigMesh,
				Flags,
				pMeshContainer->NumPaletteEntries,
				pMeshContainer->pAdjacency,
				nullptr, nullptr, nullptr,
				&pMeshContainer->NumInfl,
				&pMeshContainer->NumAttributeGroups,
				&pMeshContainer->pBoneCombinationBuf,
				&pMeshContainer->MeshData.pMesh);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
			DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
			if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
			{
				LPD3DXMESH pMesh;
				hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
				if (!FAILED(hr))
				{
					pMeshContainer->MeshData.pMesh->Release();
					pMeshContainer->MeshData.pMesh = pMesh;
					pMesh = nullptr;
				}
			}

			D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
			LPD3DVERTEXELEMENT9 pDeclCur;
			hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while (pDeclCur->Stream != 0xff)
			{
				if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
				{
					pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
				}
				pDeclCur++;
			}

			hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
			if (pAlloc->m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
			{
				pAlloc->m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

				// Allocate space for blend matrices
				delete[] pAlloc->m_pBoneMatrices;
				//TODO : new deja surcharger pour D3DXMATRIXA16
				//#include "DebugEngine/DebugNewOff.h"
				pAlloc->m_pBoneMatrices = new D3DXMATRIX[pAlloc->m_NumBoneMatricesMax];//D3DXMATRIXA16[ pAlloc->m_NumBoneMatricesMax ];
				//#include "DebugEngine/DebugNew.h"
				if (pAlloc->m_pBoneMatrices == nullptr)
				{
					hr = E_OUTOFMEMORY;
					goto e_Exit;
				}
			}

		}
		// if software skinning selected, use GenerateSkinnedMesh to create a mesh that can be used with UpdateSkinnedMesh
		else if (method_ == SOFTWARE)
		{
			hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
				pd3dDevice, &pMeshContainer->MeshData.pMesh);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(nullptr, &pMeshContainer->NumAttributeGroups);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			delete[] pMeshContainer->pAttributeTable;
			pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
			if (pMeshContainer->pAttributeTable == nullptr)
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, nullptr);
			if (FAILED(hr))
			{
				goto e_Exit;
			}

			// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
			if (pAlloc->m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
			{
				pAlloc->m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

				// Allocate space for blend matrices
				delete[] pAlloc->m_pBoneMatrices;
				//TODO : pourquoi erreur ????????
#include "DebugEngine/DebugNewOff.h"
				pAlloc->m_pBoneMatrices = new D3DXMATRIXA16[pAlloc->m_NumBoneMatricesMax];
#include "DebugEngine/DebugNew.h"
				if (pAlloc->m_pBoneMatrices == nullptr)
				{
					hr = E_OUTOFMEMORY;
					goto e_Exit;
				}
			}
		}
		else  // invalid m_SkinningMethod value
		{
			// return failure due to invalid skinning method value
			hr = E_INVALIDARG;
			goto e_Exit;
		}

	e_Exit:
		return hr;
	}

	/**
	 * update the skinning method
	 */
	void cAllocateHierarchy::UpdateSkinningMethod(cAllocateHierarchy* pAlloc, LPD3DXFRAME pFrameBase, METHOD method_)
	{
		auto pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

		auto pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		while (pMeshContainer != nullptr)
		{
			GenerateSkinnedMesh(pAlloc, GameCore::Instance().GetGraphic().GetDeviceCOM(), pMeshContainer, method_);

			pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer;
		}

		if (pFrame->pFrameSibling != nullptr)
		{
			UpdateSkinningMethod(pAlloc, pFrame->pFrameSibling, method_);
		}

		if (pFrame->pFrameFirstChild != nullptr)
		{
			UpdateSkinningMethod(pAlloc, pFrame->pFrameFirstChild, method_);
		}
	}

}; // namespace GameEngine