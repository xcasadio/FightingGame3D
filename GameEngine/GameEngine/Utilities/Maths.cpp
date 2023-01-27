#include "Maths.h"

#include <d3dx9math.h>

//a inclure en dernier
#include "../DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Calcul de la matrice de rotation entre 2 vecteurs
	 * @param pOut_ la matrice de rotation resultante
	 * @param pFrom_ vecteur de depart
	 * @param pTo_ vecteur de fin
	 */
	D3DXMATRIX *cMaths::MatrixRotationFrom2Vectors( D3DXMATRIX *pOut_, const D3DXVECTOR3 *pFrom_, const D3DXVECTOR3 *pTo_ )
	{
		D3DXVECTOR3 vAxis;
		float fAngle;
		
		/*
		cas des vecteurs opposés
		angle : 180 degre
		axe : choix abitraire
		*/
		if ( cMaths::IsVectorEqual( *pFrom_, *pTo_ ) )
		{
			return D3DXMatrixIdentity( pOut_ );
		}
		else
		{
			if ( cMaths::IsVectorEqual( *pFrom_, -(*pTo_) ) )
			{
				D3DXVec3Cross( &vAxis, pFrom_, &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) );
				fAngle = D3DX_PI;
			}
			else
			{
				D3DXVec3Cross( &vAxis, pFrom_, pTo_ );

				if ( vAxis == D3DXVECTOR3( 0.0f, 0.0f, 0.0f) )
				{
					vAxis = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
				}

				*pOut_ = D3DXMATRIX( pFrom_->x, pFrom_->y, pFrom_->z, 0.0f, pTo_->x, pTo_->y, pTo_->z, 0.0f, vAxis.x, vAxis.y, vAxis.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );

				if ( D3DXMatrixDeterminant( pOut_ ) < 0 )
				{
					D3DXVec3Cross( &vAxis, pTo_, pFrom_);
					fAngle = acosf( D3DXVec3Dot( pTo_, pFrom_) );
				}
				else
				{
					fAngle = acosf( D3DXVec3Dot( pFrom_, pTo_) );
				}
			}
		}

		D3DXQUATERNION qRot;
		D3DXQuaternionRotationAxis( &qRot, &vAxis, fAngle );

		return D3DXMatrixRotationQuaternion( pOut_, &qRot );

		//return D3DXMatrixRotationAxis( pOut_, &vAxis, fAngle);
	}

	/**
	 * Permet de savoir si 2 vecteurs sont opposés avec un taux  d'erreur
	 * @param v1 un vecteur
	 * @param v2 un vecteur
	 * @param fEpsilon optionnel : la constante d'erreur
	 */
	bool cMaths::IsOppositeVector( const D3DXVECTOR3 &v1_, const D3DXVECTOR3 &v2_, const float fEpsilon_ )
	{
		if ( ( v1_.x > 0.0f && v2_.x > 0.0f ) || ( v1_.x < 0.0f && v2_.x < 0.0f ) ||
			( v1_.y > 0.0f && v2_.y > 0.0f ) || ( v1_.y < 0.0f && v2_.y < 0.0f ) ||
			( v1_.z > 0.0f && v2_.z > 0.0f ) || ( v1_.z < 0.0f && v2_.z < 0.0f ) )
		{
			return false;
		}

		if ( fabsf( v2_.x ) <= fabsf( v1_.x ) + fEpsilon_ && fabsf( v2_.x ) >= fabsf( v1_.x ) - fEpsilon_ &&
			fabsf( v2_.y ) <= fabsf( v1_.y ) + fEpsilon_ && fabsf( v2_.y ) >= fabsf( v1_.y ) - fEpsilon_ &&
			fabsf( v2_.z ) <= fabsf( v1_.z ) + fEpsilon_ && fabsf( v2_.z ) >= fabsf( v1_.z ) - fEpsilon_ )
		{
			return true;
		}
		
		return false;
	}

	/**
	 * Permet de savoir si 2 vecteurs sont opposés avec un taux  d'erreur
	 * @param v1 un vecteur
	 * @param v2 un vecteur
	 * @param fEpsilon optionnel : la constante d'erreur
	 */
	bool cMaths::IsVectorEqual( const D3DXVECTOR3 &v1_, const D3DXVECTOR3 &v2_, const float fEpsilon_ )
	{
		if ( ( v1_.x > 0.0f && v2_.x < 0.0f ) || ( v1_.x < 0.0f && v2_.x > 0.0f ) ||
			( v1_.y > 0.0f && v2_.y < 0.0f ) || ( v1_.y < 0.0f && v2_.y > 0.0f ) ||
			( v1_.z > 0.0f && v2_.z < 0.0f ) || ( v1_.z < 0.0f && v2_.z > 0.0f ) )
		{
			return false;
		}

		if ( fabsf( v2_.x ) <= fabsf( v1_.x ) + fEpsilon_ && fabsf( v2_.x ) >= fabsf( v1_.x ) - fEpsilon_ &&
			fabsf( v2_.y ) <= fabsf( v1_.y ) + fEpsilon_ && fabsf( v2_.y ) >= fabsf( v1_.y ) - fEpsilon_ &&
			fabsf( v2_.z ) <= fabsf( v1_.z ) + fEpsilon_ && fabsf( v2_.z ) >= fabsf( v1_.z ) - fEpsilon_ )
		{
			return true;
		}
		
		return false;
	}

} // namespace GameEngine
