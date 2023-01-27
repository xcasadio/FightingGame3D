#include "MeshXAnimated.h"
#include "MeshHierarchy.h"

#include "GameCore.h"
#include "Graphic.h"
#include "Utilities/Macros.h"

//#include <dxerr.h>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

#define MOVE_TRANSITION_TIME D3DX_16F_MIN

namespace GameEngine
{
	/**
	 *
	 */
	MeshXAnimated::MeshXAnimated()
	{
		m_pFrameRoot = NULL;
		m_pAnimController = NULL;
		m_SkinningMethod = D3DNONINDEXED;

		m_CurrentAnimationSet = 0;	
		m_NumAnimationSets = 0;
		m_CurrentTrack = 0;
		m_CurrentTime = 0.0f;
		m_CurrentAnimationTime = 0.0f;

		m_IsLoaded = false;
	}

	/**
	 *
	 */
	MeshXAnimated::~MeshXAnimated()
	{
		Free();
	}

	/**
	 *
	 */
	void MeshXAnimated::Free()
	{
		SAFE_RELEASE(m_pAnimController);
		cAllocateHierarchy alloc;
		D3DXFrameDestroy( m_pFrameRoot, &alloc);
		m_pFrameRoot = NULL;

		m_IsLoaded = false;
	}

	/**
	 *
	 */
	bool MeshXAnimated::Load( const char *fileName_)
	{
		Free();

		HRESULT hr;
		cAllocateHierarchy Alloc;

		if ( FAILED(D3DXLoadMeshHierarchyFromX( fileName_, D3DXMESH_MANAGED, GameCore::Instance().GetGraphic().GetDeviceCOM(),
                                          &Alloc, NULL, &m_pFrameRoot, &m_pAnimController ) ) )
		{
			Window::Error( false, "D3DXLoadMeshHierarchyFromX() : impossible de charger %s", fileName_);
			return false;
		}

		m_NumAnimationSets = m_pAnimController->GetMaxNumAnimationSets();

		V( SetupBoneMatrixPointers( m_pFrameRoot ) );

		float radius;
		D3DXVECTOR3 center;

		if ( FAILED (D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &center, &radius ) ) )
		{
			Window::Error( false, "D3DXLoadMeshHierarchyFromX() : erreur D3DXFrameCalculateBoundingSphere()");
			return false;
		}
		
		m_BoundingSphere.SetCenter( center );
		m_BoundingSphere.SetRadius( radius );

		m_IsLoaded = true;

		return true;
	}

	/**
	 *
	 */
	void MeshXAnimated::SetSkinningMethod( METHOD method_)
	{
		if ( method_ != m_SkinningMethod)
		{
			m_SkinningMethod = method_;
			cAllocateHierarchy alloc;
			alloc.UpdateSkinningMethod( &alloc, m_pFrameRoot, m_SkinningMethod);
		}
	}

	/**
	 *
	 */
	unsigned int MeshXAnimated::GetCurrentAnimationSet() const
	{
		return m_CurrentAnimationSet;
	}

	/**
	 *
	 */
	unsigned int MeshXAnimated::GetNumberOfAnimationSet() const
	{
		return m_NumAnimationSets;
	}

	/**
	 *
	 */
	METHOD MeshXAnimated::GetMethod()
	{
		return m_SkinningMethod;
	}

	/**
	 * Called to setup the pointers for a given bone to its transformation matrix
	 */
	HRESULT MeshXAnimated::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
	{
		HRESULT hr;

		if (pFrame->pMeshContainer != NULL)
		{
			hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
			if (FAILED(hr))
				return hr;
		}

		if (pFrame->pFrameSibling != NULL)
		{
			hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
			if (FAILED(hr))
				return hr;
		}

		if (pFrame->pFrameFirstChild != NULL)
		{
			hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
			if (FAILED(hr))
				return hr;
		}

		return S_OK;
	}
		
	/**
	 * Called to setup the pointers for a given bone to its transformation matrix
	 */
	HRESULT MeshXAnimated::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase )
	{
		UINT iBone, cBones;
		D3DXFRAME_DERIVED *pFrame;

		D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

		// if there is a skinmesh, then setup the bone matrices
		if (pMeshContainer->pSkinInfo != NULL)
		{
			cBones = pMeshContainer->pSkinInfo->GetNumBones();

			pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
			if (pMeshContainer->ppBoneMatrixPtrs == NULL)
				return E_OUTOFMEMORY;

			for (iBone = 0; iBone < cBones; iBone++)
			{
				pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone) );
				if (pFrame == NULL)
					return E_FAIL;

				pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
			}
		}

		return S_OK;
	}

	/**
	 * 
	 */
	bool MeshXAnimated::Update( const float time_, D3DXMATRIX *pMatWorld_ )
	{
		bool res = false;

		if( m_pAnimController != NULL )
		{
			m_pAnimController->AdvanceTime( (double) time_, NULL );
		}

		if( m_pFrameRoot != NULL )
		{
			UpdateFrameMatrices( m_pFrameRoot, pMatWorld_ );
		}
		
		// TODO : verifier si c'est la fin de l'animation
		double animTime = GetCurrentAnimationTime();

		//l'animation a bouclé
		if ( animTime < m_CurrentAnimationTime )
		{
			res = true;
		}

		m_CurrentAnimationTime = animTime;

		return res;
	}

	/**
	 * Get the name of the animation
	 * @param index - the animation set index
	 * @return the name
	 */
	std::string MeshXAnimated::GetAnimationSetName(unsigned int index_)
	{
		if (index_ >= m_NumAnimationSets)
			return "Error: No set exists";

		// Get the animation set
		LPD3DXANIMATIONSET set;
		m_pAnimController->GetAnimationSet(index_, &set );

		std::string nameString(set->GetName());

		set->Release();

		return nameString;
	}

	/**
	 * update the frame matrices
	 */
	void MeshXAnimated::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
	{
		D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

		if (pParentMatrix != NULL)
			D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
		else
			pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

		if (pFrame->pFrameSibling != NULL)
		{
			UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
		}

		if (pFrame->pFrameFirstChild != NULL)
		{
			UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
		}
	}

	/**
	 *
	 */
	void MeshXAnimated::ReleaseAttributeTable( LPD3DXFRAME pFrameBase )
	{
		D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
		D3DXMESHCONTAINER_DERIVED *pMeshContainer;

		pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pFrame->pMeshContainer;

		while( pMeshContainer != NULL )
		{
			delete[] pMeshContainer->pAttributeTable;

			pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pMeshContainer->pNextMeshContainer;
		}

		if (pFrame->pFrameSibling != NULL)
		{
			ReleaseAttributeTable(pFrame->pFrameSibling);
		}

		if (pFrame->pFrameFirstChild != NULL)
		{
			ReleaseAttributeTable(pFrame->pFrameFirstChild);
		}
	}

	/**
	 *
	 */
	void MeshXAnimated::Draw()
	{
		DrawFrame( m_pFrameRoot );
	}

	/**
	 * Called to render a frame in the hierarchy
	 */
	void MeshXAnimated::DrawFrame( LPD3DXFRAME pFrame )
	{
		LPD3DXMESHCONTAINER pMeshContainer;

		pMeshContainer = pFrame->pMeshContainer;
		while (pMeshContainer != NULL)
		{
			DrawMeshContainer( pMeshContainer, pFrame );

			pMeshContainer = pMeshContainer->pNextMeshContainer;
		}

		if (pFrame->pFrameSibling != NULL)
		{
			DrawFrame( pFrame->pFrameSibling);
		}

		if (pFrame->pFrameFirstChild != NULL)
		{
			DrawFrame( pFrame->pFrameFirstChild );
		}
	}
	
	/**
	 * Called to render a mesh in the hierarchy
	 */
	void MeshXAnimated::DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
	{
		IDirect3DDevice9 *pd3dDevice = GameCore::Instance().GetGraphic().GetDeviceCOM();

		HRESULT hr;
		D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
		D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
		UINT iMaterial;
		UINT NumBlend;
		UINT iAttrib;
		DWORD AttribIdPrev;
		LPD3DXBONECOMBINATION pBoneComb;

		UINT iMatrixIndex;
		//UINT iPaletteEntry;
		D3DXMATRIXA16 matTemp;
		D3DCAPS9 d3dCaps;
		V( pd3dDevice->GetDeviceCaps( &d3dCaps ) );

		// first check for skinning
		if (pMeshContainer->pSkinInfo != NULL)
		{
			if( m_SkinningMethod == D3DNONINDEXED )
			{
				AttribIdPrev = UNUSED32; 
				pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

				// Draw using default vtx processing of the device (typically HW)
				for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
				{
					NumBlend = 0;
					for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
					{
						if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
						{
							NumBlend = i;
						}
					}

					if( d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1 )
					{
						// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
						for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
						{
							iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
							if (iMatrixIndex != UINT_MAX)
							{
								D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
								V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
							}
						}

						V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );

						// lookup the material used for this subset of faces
						if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
						{
							V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
							V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );
							AttribIdPrev = pBoneComb[iAttrib].AttribId;
						}

						// draw the subset now that the correct material and matrices are loaded
						pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
					}
				}

				// If necessary, draw parts that HW could not handle using SW
				if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
				{
					AttribIdPrev = UNUSED32; 
					pd3dDevice->SetSoftwareVertexProcessing(TRUE);
					for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
					{
						NumBlend = 0;
						for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
						{
							if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
							{
								NumBlend = i;
							}
						}

						if (d3dCaps.MaxVertexBlendMatrices < NumBlend + 1)
						{
							// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
							for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
							{
								iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
								if (iMatrixIndex != UINT_MAX)
								{
									D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
									V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
								}
							}

							V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );

							// lookup the material used for this subset of faces
							if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
							{
								V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
								V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );
								AttribIdPrev = pBoneComb[iAttrib].AttribId;
							}

							// draw the subset now that the correct material and matrices are loaded
							pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
						}
					}
					V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
				}

				V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0) );
			}
			/*
			else if (m_SkinningMethod == D3DINDEXED)
			{
				// if hw doesn't support indexed vertex processing, switch to software vertex processing
				if (pMeshContainer->UseSoftwareVP)
				{
					// If hw or pure hw vertex processing is forced, we can't render the
					// mesh, so just exit out.  Typical applications should create
					// a device with appropriate vertex processing capability for this
					// skinning method.
					if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
						return;

					V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
				}

				// set the number of vertex blend indices to be blended
				if (pMeshContainer->NumInfl == 1)
				{
					V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS) );
				}
				else
				{
					V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1) );
				}

				if (pMeshContainer->NumInfl)
					V( pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE) );

				// for each attribute group in the mesh, calculate the set of matrices in the palette and then draw the mesh subset
				pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
				for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
				{
					// first calculate all the world matrices
					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
							V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp ) );
						}
					}
	                
					// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
					V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D ) );
					V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );

					// finally draw the subset with the current world matrix palette and material state
					pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
				}

				// reset blending state
				V( pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE) );
				V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0) );

				// remember to reset back to hw vertex processing if software was required
				if (pMeshContainer->UseSoftwareVP)
				{
					V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
				}
			}
			else if (m_SkinningMethod == D3DINDEXEDVS) 
			{
				// Use COLOR instead of UBYTE4 since Geforce3 does not support it
				// vConst.w should be 3, but due to COLOR/UBYTE4 issue, mul by 255 and add epsilon
				D3DXVECTOR4 vConst( 1.0f, 0.0f, 0.0f, 765.01f );

				if (pMeshContainer->UseSoftwareVP)
				{
					// If hw or pure hw vertex processing is forced, we can't render the
					// mesh, so just exit out.  Typical applications should create
					// a device with appropriate vertex processing capability for this
					// skinning method.
					if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
						return;

					V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
				}

				V( pd3dDevice->SetVertexShader( g_pIndexedVertexShader[pMeshContainer->NumInfl - 1] ) );

				pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
				for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
				{
					// first calculate all the world matrices
					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
							D3DXMatrixMultiplyTranspose(&matTemp, &matTemp, &g_matView);
							V( pd3dDevice->SetVertexShaderConstantF(iPaletteEntry*3 + 9, (float*)&matTemp, 3) );
						}
					}

					// Sum of all ambient and emissive contribution
					D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
					D3DXCOLOR color2(.25, .25, .25, 1.0);
					D3DXCOLOR ambEmm;
					D3DXColorModulate(&ambEmm, &color1, &color2);
					ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

					// set material color properties 
					V( pd3dDevice->SetVertexShaderConstantF(8, (float*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse), 1) );
					V( pd3dDevice->SetVertexShaderConstantF(7, (float*)&ambEmm, 1) );
					vConst.y = pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Power;
					V( pd3dDevice->SetVertexShaderConstantF(0, (float*)&vConst, 1) );

					V( pd3dDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]) );

					// finally draw the subset with the current world matrix palette and material state
					pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
				}

				// remember to reset back to hw vertex processing if software was required
				if (pMeshContainer->UseSoftwareVP)
				{
					V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
				}
				V( pd3dDevice->SetVertexShader( NULL ) );
			}
			else if (m_SkinningMethod == D3DINDEXEDHLSLVS) 
			{
				if (pMeshContainer->UseSoftwareVP)
				{
					// If hw or pure hw vertex processing is forced, we can't render the
					// mesh, so just exit out.  Typical applications should create
					// a device with appropriate vertex processing capability for this
					// skinning method.
					if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
						return;

					V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
				}

				pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
				for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
				{ 
					// first calculate all the world matrices
					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
							D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &g_matView);
						}
					}
					g_pEffect->SetMatrixArray( "mWorldMatrixArray", m_pBoneMatrices, pMeshContainer->NumPaletteEntries);

					// Sum of all ambient and emissive contribution
					D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
					D3DXCOLOR color2(.25, .25, .25, 1.0);
					D3DXCOLOR ambEmm;
					D3DXColorModulate(&ambEmm, &color1, &color2);
					ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

					// set material color properties 
					g_pEffect->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
					g_pEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

					// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
					V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] ) );

					// Set CurNumBones to select the correct vertex shader for the number of bones
					g_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1);

					// Start the effect now all parameters have been updated
					UINT numPasses;
					g_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) );
					for( UINT iPass = 0; iPass < numPasses; iPass++ )
					{
						g_pEffect->BeginPass( iPass );

						// draw the subset with the current world matrix palette and material state
						pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );

						g_pEffect->EndPass();
					}

					g_pEffect->End();

					V( pd3dDevice->SetVertexShader(NULL) );
				}

				// remember to reset back to hw vertex processing if software was required
				if (pMeshContainer->UseSoftwareVP)
				{
					V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
				}
			}
			else if (m_SkinningMethod == SOFTWARE)
			{
				D3DXMATRIX  Identity;
				DWORD       cBones  = pMeshContainer->pSkinInfo->GetNumBones();
				DWORD       iBone;
				PBYTE       pbVerticesSrc;
				PBYTE       pbVerticesDest;

				// set up bone transforms
				for (iBone = 0; iBone < cBones; ++iBone)
				{
					D3DXMatrixMultiply
					(
						&m_pBoneMatrices[iBone],                 // output
						&pMeshContainer->pBoneOffsetMatrices[iBone], 
						pMeshContainer->ppBoneMatrixPtrs[iBone]
					);
				}

				// set world transform
				D3DXMatrixIdentity(&Identity);
				V( pd3dDevice->SetTransform(D3DTS_WORLD, &Identity) );

				pMeshContainer->pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);
				pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);

				// generate skinned mesh
				pMeshContainer->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest);

				pMeshContainer->pOrigMesh->UnlockVertexBuffer();
				pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

				for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
				{
					V( pd3dDevice->SetMaterial(&(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D)) );
					V( pd3dDevice->SetTexture(0, pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId]) );
					pMeshContainer->MeshData.pMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId);
				}
			}*/
			else // bug out as unsupported mode
			{
				Window::Error( true, "DrawMeshContainer() : METHOD non supporté" );
			}
		}
		else  // standard mesh, just draw it after setting material properties
		{
			V( pd3dDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix) );

			for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
			{
				V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
				V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] ) );
				pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
			}
		}
	}
	
	/**
	 * Change to a different animation set
	 * Handles transitions between animations to make it smooth and not a sudden jerk to a new position
	 * @param index - new animation set index
	*/
	void MeshXAnimated::SetAnimationSet(unsigned int index_)
	{
		if (index_ == m_CurrentAnimationSet)
			return;

		if (index_ >= m_NumAnimationSets)
			index_ = 0;

		// Remember current animation
		m_CurrentAnimationSet = index_;

		// Get the animation set from the controller
		LPD3DXANIMATIONSET set;
		m_pAnimController->GetAnimationSet(m_CurrentAnimationSet, &set );

		// Assign to our track
		m_pAnimController->SetTrackAnimationSet( m_CurrentTrack, set );
		set->Release();	

		m_CurrentAnimationTime = 0.0f;

		// Note: for a smooth transition between animation sets we can use two tracks and assign the new set to the track
		// not currently playing then insert Keys into the KeyTrack to do the transition between the tracks
		// tracks can be mixed together so we can gradually change into the new animation
/*
		// Alternate tracks
		DWORD newTrack = ( m_CurrentTrack == 0 ? 1 : 0 );

		// Assign to our track
		m_pAnimController->SetTrackAnimationSet( newTrack, set );
		set->Release();	

		// Clear any track events currently assigned to our two tracks
		m_pAnimController->UnkeyAllTrackEvents( m_CurrentTrack );
		m_pAnimController->UnkeyAllTrackEvents( newTrack );

		// Add an event key to disable the currently playing track kMoveTransitionTime seconds in the future
		m_pAnimController->KeyTrackEnable( m_CurrentTrack, FALSE, m_CurrentTime + MOVE_TRANSITION_TIME );
		// Add an event key to change the speed right away so the animation completes in kMoveTransitionTime seconds
		m_pAnimController->KeyTrackSpeed( m_CurrentTrack, 0.0f, m_CurrentTime, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
		// Add an event to change the weighting of the current track (the effect it has blended with the secon track)
		m_pAnimController->KeyTrackWeight( m_CurrentTrack, 0.0f, m_CurrentTime, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

		// Enable the new track
		m_pAnimController->SetTrackEnable( newTrack, TRUE );
		// Add an event key to set the speed of the track
		m_pAnimController->KeyTrackSpeed( newTrack, 1.0f, m_CurrentTime, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
		// Add an event to change the weighting of the current track (the effect it has blended with the first track)
		// As you can see this will go from 0 effect to total effect(1.0f) in kMoveTransitionTime seconds and the first track goes from 
		// total to 0.0f in the same time.
		m_pAnimController->KeyTrackWeight( newTrack, 1.0f, m_CurrentTime, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

		// Remember current track
		m_CurrentTrack = newTrack;*/
	}

	//////////////////////////////////////////////////

	/**
	 *
	 */
	ID3DXAnimationController *MeshXAnimated::GetAnimationController()
	{
		return m_pAnimController;
	}

	/**
	 *
	 */
	DOUBLE MeshXAnimated::GetCurrentAnimationTimeMax()
	{
		LPD3DXANIMATIONSET set;
		m_pAnimController->GetTrackAnimationSet(m_CurrentTrack, &set );

		double time = set->GetPeriod();

		set->Release();

		return time;
	}

	/**
	 *
	 */
	DOUBLE MeshXAnimated::GetCurrentAnimationTime()
	{
		LPD3DXANIMATIONSET set;
		m_pAnimController->GetTrackAnimationSet(m_CurrentTrack, &set );

		D3DXTRACK_DESC trackDesc;
		m_pAnimController->GetTrackDesc( m_CurrentTrack, &trackDesc);

		double time = set->GetPeriodicPosition( trackDesc.Position );
		set->Release();

		return time;
	}

	/**
	 *
	 */
	UINT MeshXAnimated::GetCurrentTrack()
	{
		return m_CurrentTrack;
	}

	/**
	 *
	 */
	UINT MeshXAnimated::GetCurrentAnimationSet()
	{
		return m_CurrentAnimationSet;
	}

	/**
	 *
	 */
	void MeshXAnimated::SetCurrentAnimationTime( float percentage_, D3DXMATRIX *pMatWorld_)
	{
		m_pAnimController->SetTrackPosition(m_CurrentTrack, GetCurrentAnimationTimeMax() * percentage_ / 100.0f);
		m_CurrentAnimationTime = GetCurrentAnimationTimeMax() * percentage_ / 100.0f;

		if ( pMatWorld_ )
		{
			Update(0.0f, pMatWorld_);
		}
	}

	/**
	 *
	 */
	bool MeshXAnimated::IsLoaded()
	{
		return m_IsLoaded;
	}

	/**
	 *
	 *
	D3DXVECTOR3 MeshXAnimated::GetObjectCenter()
	{
		return m_vObjectCenter;
	}
	 
	float MeshXAnimated::GetObjectRadius()
	{
		return m_fObjectRadius; 
	}*/

	/**
	 *
	 */
	BoundingSphere &MeshXAnimated::GetBoundingSphere()
	{
		return m_BoundingSphere;
	}

	/**
	 *
	 */
	LPD3DXFRAME MeshXAnimated::GetFrameRoot()
	{
		return m_pFrameRoot;
	}

} // namespace GameEngine
