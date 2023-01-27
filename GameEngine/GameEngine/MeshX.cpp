#include "MeshX.h"
#include "Graphic.h"
#include "GameCore.h"
#include "Utilities/Serializable.h"
#include "Utilities/StringUtilities.h"
#include "MediaPathManager.h"

#include <string>

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>
//#include <d3dx9math.h>

#include <fstream>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{	
	/**
	 *
	 */
	MeshX::MeshX(void)
	{
		m_pMesh          = NULL; // Our mesh object in sysmem
		m_pMeshMaterials = NULL; // Materials for our mesh
		m_pMeshTextures  = NULL; // Textures for our mesh
		m_dwNumMaterials = 0L;   // Number of mesh materials
		m_bIsLoaded = false;

		IParticleVisualObject::SetAlphaEnabled(true);
		IParticleVisualObject::SetDestBlend(D3DBLEND_ONE);
		IParticleVisualObject::SetSrcBlend(D3DBLEND_ONE);
		IParticleVisualObject::SetLighting(true);
		IParticleVisualObject::SetZWrite(true);
	}

	/**
	 *
	 */
	MeshX::~MeshX(void)
	{
		Free();
	}

	/**
	 * @is flux d'entrée
	 * @return le nombre d'octet lu
	 */
	int MeshX::Read( std::istream &is_ )
	{
		IParticleVisualObject::Read(is_);

		int octetRead = is_.tellg();
		SIZE_T n = 0;

		is_.read( (char *) &n, sizeof(n) );

		char *buf = new char[n];

		is_.read( buf, sizeof(char) * n );

		std::string str = MediaPathManager::Instance().FindMedia( buf );

		if ( Load( buf ) == false )
		{
			Window::Error( false, "MeshX::Read() : Load( buf )" );
		}

		delete[] buf;
		buf = NULL;

		octetRead = (int) is_.tellg() - octetRead;
		return octetRead;
	}

	/**
	 * @os flux de sortie
	 * @return le meme flux modifié
	 */
	std::ostream &MeshX::operator >> ( std::ostream &os_ )
	{
		IParticleVisualObject::operator >> (os_);

		SIZE_T n = GetName().size() + 1;
		os_.write( (char *) &n, sizeof(n) );

		std::string str = cStringUtilities::GetFileName( GetName() );

		os_.write( (char *) str.c_str(), sizeof(char) * (n - 1) );

		char c = '\0';
		os_.write( (char *) &c, sizeof(char) );

		return os_;
	}

	/**
	 *
	 */
	bool MeshX::Load(const char *fileName)
	{
		LPD3DXBUFFER pD3DXMtrlBuffer;
		Graphic *graphic = &GameCore::Instance().GetGraphic();

		Free();

		// Load the mesh from the specified file
		if( FAILED( D3DXLoadMeshFromX( fileName, D3DXMESH_SYSTEMMEM, 
									   graphic->GetDeviceCOM(), NULL, 
									   &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, 
									   &m_pMesh ) ) )
		{
			Free();
			Window::Error( false, "Could not find file for MeshX %s", fileName);
			return false;
		}

		// We need to extract the material properties and texture names from the 
		// pD3DXMtrlBuffer
		D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
		m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];

		if( m_pMeshMaterials == NULL )
		{
			pD3DXMtrlBuffer->Release();
			Free();
			Window::Error( false, "MeshX::Load() : Out of memory, materials");
			return false/*E_OUTOFMEMORY*/;
		}

		m_pMeshTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

		if( m_pMeshTextures == NULL )
		{
			pD3DXMtrlBuffer->Release();
			Free();
			Window::Error( false, "MeshX::Load() : Out of memory, textures");
			return false/*E_OUTOFMEMORY*/;
		}

		for( DWORD i=0; i<m_dwNumMaterials; i++ )
		{
			// Copy the material
			m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

			// Set the ambient color for the material (D3DX does not do this)
			m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

			m_pMeshTextures[i] = NULL;

			if( d3dxMaterials[i].pTextureFilename != NULL && 
				lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
			{
				// On ajoute le chemin complet au nom des textures
				std::string str = cStringUtilities::GetFileName( d3dxMaterials[i].pTextureFilename );
				str = MediaPathManager::Instance().FindMedia( str.c_str() );

				// Create the texture
				if( FAILED( D3DXCreateTextureFromFileA( graphic->GetDeviceCOM(), 
													/*d3dxMaterials[i].pTextureFilename*/str.c_str(), 
													&m_pMeshTextures[i] ) ) )
				{
					pD3DXMtrlBuffer->Release();
					Free();
					Window::Error( false, "MeshX::Load() :c ould not find texture map %s", str.c_str());
					return false;
				}
			}
		}

		// Done with the material buffer
		pD3DXMtrlBuffer->Release();

		m_bIsLoaded = true;

		return true;
	}

	/**
	 *
	 */
	bool MeshX::IsLoaded()
	{
		return m_bIsLoaded;
	}

	/**
	 *
	 */
	void MeshX::Render()
	{
		Render( NULL, true);
	}

	/**
	 *
	 */
	void MeshX::Render( D3DMATERIAL9 *pMat_, bool bDrawTexture_)
	{
		Graphic *graphic = &GameCore::Instance().GetGraphic();

		if ( pMat_ != NULL )
		{
			graphic->GetDeviceCOM()->SetMaterial( pMat_ );
		}

		for( DWORD i=0; i<m_dwNumMaterials; i++ )
		{
			if ( pMat_ == NULL )
			{
				graphic->GetDeviceCOM()->SetMaterial( &m_pMeshMaterials[i] );
			}
			
			if ( bDrawTexture_ )
			{
				graphic->GetDeviceCOM()->SetTexture( 0, m_pMeshTextures[i] );
			}
	    
			m_pMesh->DrawSubset( i );
		}

		graphic->GetDeviceCOM()->SetTexture( 0, NULL );
	}

	/**
	 *
	 */
	LPD3DXMESH MeshX::GetMesh()
	{
		return m_pMesh;
	}

	/**
	 *
	 */
	D3DMATERIAL9 *MeshX::GetMaterials()
	{
		return m_pMeshMaterials;
	}

	/**
	 *
	 */
	LPDIRECT3DTEXTURE9 *MeshX::GetTextures()
	{
		return m_pMeshTextures;
	}

	/**
	 * Definit le mode d'affichage utilisé pour les particules
	 * exemple : texture, alpha, bend... 
	 */
	void MeshX::SetRenderMode()
	{
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, m_AlphaBlendEnable);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_SRCBLEND, m_SrcBlend);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_DESTBLEND, m_DestBlend);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_LIGHTING, m_Lighting);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ZWRITEENABLE, m_ZWriteEnable);
	}

	/**
	 * Permet de "deconfigurer" le mode d'affichage des particules
	 * exemple : on sauvegarde les precedents parametres puis on les restorent
	 */
	void MeshX::UnsetRenderMode()
	{
		//GameCore::Instance().GetGraphic().SetAmbientLight(m_AmbientLightR, m_AmbientLightG, m_AmbientLightB);

		//TODO - enregistrer les parametres precedents
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_LIGHTING, true);
		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetRenderState(D3DRS_ZWRITEENABLE, true);

		GameCore::Instance().GetGraphic().GetDeviceCOM()->SetTexture(0, NULL);
	}

	/**
	 *
	 */
	DWORD MeshX::GetMaterialNumber() const
	{
		return m_dwNumMaterials;
	}

	/**
	 *
	 */
	void MeshX::Free()
	{
		if( m_pMeshMaterials != NULL ) 
			delete[] m_pMeshMaterials;

		if( m_pMeshTextures )
		{
			for( DWORD i = 0; i < m_dwNumMaterials; i++ )
			{
				if( m_pMeshTextures[i] )
				{
					m_pMeshTextures[i]->Release();
				}
			}
			delete[] m_pMeshTextures;
		}

		if( m_pMesh != NULL )
			m_pMesh->Release();

		m_bIsLoaded = false;
	}

	//////////////////////////////////////////////////////
	/// cBoundingForm
	//////////////////////////////////////////////////////
	
	/**
	 *
	 */
	BoundingForm::BoundingForm()
	{
		m_Type = BOUNDING_UNDEFINED;
	}

	/**
	 *
	 */
	void BoundingForm::Compute(LPD3DXMESH mesh_)
	{
		IDirect3DVertexBuffer9 *pVertex = NULL;
		mesh_->GetVertexBuffer(&pVertex);
		Compute( pVertex, mesh_->GetNumVertices(), D3DXGetFVFVertexSize(mesh_->GetFVF()));
	}

	/**
	 *
	 */
	void BoundingForm::Compute(MeshX &mesh)
	{
		Compute(mesh.GetMesh());
	}

	/**
	 *
	 */
	BoundingForm::eBoundingForms BoundingForm::GetType() const
	{
		return m_Type;
	}

	/**
	 *
	 */
	void BoundingForm::SetType(BoundingForm::eBoundingForms type_)
	{
		m_Type = type_;
	}

	//////////////////////////////////////////////////////
	/// BoundingBox
	//////////////////////////////////////////////////////
	
	/**
	 *
	 */
	BoundingBox::BoundingBox()
	{
		m_Type = BOUNDING_BOX;
	}

	/**
	 *
	 */
	void BoundingBox::Compute(IDirect3DVertexBuffer9 *pVB_, DWORD NumVertices, DWORD FVF )
	{
		void *pData = NULL;

		if ( SUCCEEDED( pVB_->Lock(0, 0, &pData, D3DLOCK_READONLY) ) )
		{
			if ( FAILED( D3DXComputeBoundingBox( (D3DXVECTOR3*) pData, NumVertices, FVF, &m_Min, &m_Max ) ) )
			{
				Window::Error( false, "Erreur D3DXComputeBoundingBox");
			}
			pVB_->Unlock();
		}		
	}

	/**
	 *
	 */
	void BoundingBox::SetMin(D3DXVECTOR3 &min_)
	{
		m_Min = min_;
	}

	/**
	 *
	 */
	D3DXVECTOR3 BoundingBox::GetMin() const
	{
		return m_Min;
	}

	/**
	 *
	 */
	void BoundingBox::SetMax(D3DXVECTOR3 &max_)
	{
		m_Max = max_;
	}

	/**
	 *
	 */
	D3DXVECTOR3 BoundingBox::GetMax() const
	{
		return m_Max;
	}

	//////////////////////////////////////////////////////
	/// BoundingSphere
	//////////////////////////////////////////////////////
	
	/**
	 *
	 */
	BoundingSphere::BoundingSphere()
	{
		m_Type = BOUNDING_SPHERE;
		m_Radius = 0.0f;
	}

	/**
	 *
	 */
	void BoundingSphere::Compute(IDirect3DVertexBuffer9 *pVB_, DWORD NumVertices, DWORD FVF )
	{
		void *pData = NULL;

		if ( SUCCEEDED( pVB_->Lock(0, 0, &pData, D3DLOCK_READONLY) ) )
		{
			if ( FAILED( D3DXComputeBoundingSphere( (D3DXVECTOR3*) pData, NumVertices, FVF, &m_Center, &m_Radius ) ) )
			{
				Window::Error( false, "Erreur D3DXComputeBoundingSphere");
			}
			pVB_->Unlock();
		}		
	}

	/**
	 *
	 */
	void BoundingSphere::SetCenter(D3DXVECTOR3 &center_)
	{
		m_Center = center_;
	}

	/**
	 *
	 */
	D3DXVECTOR3 BoundingSphere::GetCenter() const
	{
		return m_Center;
	}
	
	/**
	 *
	 */
	void BoundingSphere::SetRadius(float radius_)
	{
		m_Radius = radius_;
	}

	/**
	 *
	 */
	float BoundingSphere::GetRadius() const
	{
		return m_Radius;
	}

} // namespace GameEngine
