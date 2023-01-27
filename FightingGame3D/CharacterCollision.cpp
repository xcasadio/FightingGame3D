#include "CharacterCollision.h"

#include "Window.h"
#include "Collision.h"

#include "DebugEngine/DebugNew.h"

using namespace GameEngine;

CharacterCollision::CharacterCollision(void)
{
	m_pFrame = NULL;
}

/*
cCharacterCollision::~cCharacterCollision(void)
{
}
*/

/**
 *
 */
CharacterCollision::CharacterCollision( const CharacterCollision &collision )
{
	Copy( collision );
}

/**
 *
 */
CharacterCollision &CharacterCollision::operator = ( const CharacterCollision &collision )
{
	Copy( collision );
	return *this;
}

/**
 *
 */
void CharacterCollision::Copy( const CharacterCollision &collision )
{
	if ( this != &collision )
	{
		m_BoundingBox.SetMin( collision.m_BoundingBox.GetMin() );
		m_BoundingBox.SetMax( collision.m_BoundingBox.GetMax() );
		m_BoneName = collision.m_BoneName;
		m_pFrame = collision.m_pFrame;
	}
}

/**
 *
 */
void CharacterCollision::SetHierarchy( LPD3DXFRAME pFrame_ )
{
	LPD3DXFRAME pFrameChild = pFrame_->pFrameFirstChild;
	D3DXVECTOR3 vMin, vMax;

	if ( pFrameChild == NULL )
	{
		Window::Error( false, "cCharacterCollision::SetHierarchy() : pFrameChild == NULL" );
		return;
	}

	if ( pFrameChild->pMeshContainer == NULL )
	{
		Window::Error( false, "cCharacterCollision::SetHierarchy() : pFrameChild->pMeshContainer == NULL" );
		return;
	}

	if ( pFrameChild->pMeshContainer->MeshData.Type == D3DXMESHTYPE_MESH )
	{
		IDirect3DVertexBuffer9 *pVB_ = NULL;
		pFrameChild->pMeshContainer->MeshData.pMesh->GetVertexBuffer(&pVB_);

		void *pData = NULL;

		if ( SUCCEEDED( pVB_->Lock(0, 0, &pData, D3DLOCK_READONLY) ) )
		{
			if ( FAILED( D3DXComputeBoundingBox( (D3DXVECTOR3*) pData, 
								pFrameChild->pMeshContainer->MeshData.pMesh->GetNumVertices(),
								D3DXGetFVFVertexSize(pFrameChild->pMeshContainer->MeshData.pMesh->GetFVF() ), 
								&vMin, 
								&vMax ) ) )
			{
				Window::Error( false, "Erreur D3DXComputeBoundingBox");
			}
			else
			{
				m_BoundingBox.SetMin( vMin );
				m_BoundingBox.SetMax( vMax );
			}

			pVB_->Unlock();
		}		
	}

	m_BoneName.append( pFrame_->Name );

	m_pFrame = pFrame_;
}

/**
 *
 */
std::string CharacterCollision::GetBoneName()
{
	return m_BoneName;
}

/** 
 *
 */
bool CharacterCollision::Collision( CharacterCollision *pColl )
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED *) (m_pFrame);
	
	if ( m_pFrame->pFrameFirstChild == NULL )
	{
		Window::Error( true, "cCharacterCollision::GetMatrix() : m_pFrame->pFrameFirstChild == NULL" );
		return false;
	}

	D3DXFRAME_DERIVED *pFrameChild = (D3DXFRAME_DERIVED *) (m_pFrame->pFrameFirstChild);

	return Collision::OrientedBoundingBox( m_BoundingBox, pFrameChild->CombinedTransformationMatrix, pColl->m_BoundingBox, pColl->GetMatrix() );
}

/**
 *
 */
D3DXMATRIX &CharacterCollision::GetMatrix()
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED *) (m_pFrame);
	
	if ( m_pFrame->pFrameFirstChild == NULL )
	{
		Window::Error( true, "cCharacterCollision::GetMatrix() : m_pFrame->pFrameFirstChild == NULL" );		
	}
	
	D3DXFRAME_DERIVED *pFrameChild = (D3DXFRAME_DERIVED *) (m_pFrame->pFrameFirstChild);
	return pFrameChild->CombinedTransformationMatrix;
}

/**
 *
 */
D3DXMATRIX &CharacterCollision::GetMatrixForDisplay()
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED *) (m_pFrame);
	
	if ( m_pFrame->pFrameFirstChild == NULL )
	{
		Window::Error( true, "cCharacterCollision::GetMatrix() : m_pFrame->pFrameFirstChild == NULL" );
	}

	D3DXFRAME_DERIVED *pFrameChild = (D3DXFRAME_DERIVED *) (m_pFrame->pFrameFirstChild);
	
	//return pFrameChild->CombinedTransformationMatrix;

	//pour l'affichage
	D3DXVECTOR3 vScale = m_BoundingBox.GetMax() - m_BoundingBox.GetMin();

	D3DXMATRIX matScale, matTrans;

	D3DXMatrixIdentity( &m_MatWorld );

	D3DXMatrixScaling( &matScale, vScale.x, vScale.y, vScale.z );
	D3DXMatrixTranslation( &matTrans, vScale.x / 2.0f + m_BoundingBox.GetMin().x, vScale.y / 2.0f + m_BoundingBox.GetMin().y, vScale.z / 2.0f + m_BoundingBox.GetMin().z );
	D3DXMatrixMultiply( &m_MatWorld, &matScale, &matTrans);

	D3DXMatrixMultiply( &m_MatWorld, &m_MatWorld, &pFrameChild->CombinedTransformationMatrix);
	//fin affichage

	return m_MatWorld;
}
