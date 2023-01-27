#include "Texture.h"
#include "Graphic.h"
#include "GameCore.h"
#include "Utilities/Serializable.h"
#include "MediaPathManager.h"
#include "Utilities/StringUtilities.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	Texture::Texture()
	{
		m_Texture = NULL;
		m_Width = m_Height = m_Transparent = 0;
	}

	/**
	 *
	 */
	Texture::~Texture()
	{
		Free();
	}

	/**
	 * 
	 *
	bool Texture::SaveSerialized(const char *fileName_)
	{
		std::fstream fs( fileName_, std::ios::binary | std::ios::out );

		*this << fs;

		fs.close();

		return true
	}*/

	/**
	 *
	 */
	int Texture::Read( std::istream &is )
	{
		return -1;
	}

	/**
	 *
	 */
	std::ostream &Texture::operator >> ( std::ostream &os_ )
	{
		D3DSURFACE_DESC d3dsd;

		SIZE_T n = GetName().size() + 1;
		os_.write( (char *) &n, sizeof(n) );

		std::string str = cStringUtilities::GetFileName( GetName() );
		os_.write( (char *) str.c_str(), sizeof(char) * (n - 1) );

		os_.write( (char *) &m_Transparent, sizeof( DWORD ) );

		if( m_Texture != NULL )
		{
			D3DFORMAT format = D3DFMT_UNKNOWN;

			if( SUCCEEDED( m_Texture->GetLevelDesc( 0, &d3dsd ) ) )
			{
				format = d3dsd.Format;
			}

			os_.write( (char *) &format, sizeof(D3DFORMAT) );		
		}

		return os_;
	}

	/**
	 *
	 */
	bool Texture::Load(const char *fileName_, DWORD Transparent_, D3DFORMAT Format_)
	{
		Free();

		Graphic *Graphics = &GameCore::Instance().GetGraphic();

		if( Graphics->GetDeviceCOM() == NULL)
			return false;

		if( fileName_ == NULL)
			return false;

		std::string str = cStringUtilities::GetFileName( fileName_ );
		str = MediaPathManager::Instance().FindMedia( str.c_str() );

		if( FAILED( D3DXCreateTextureFromFileEx( Graphics->GetDeviceCOM(),
						str.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
						0, Format_, D3DPOOL_MANAGED, 
						D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE,
						Transparent_, NULL, NULL, &m_Texture ) ) )
			return false;

		m_Width = GetWidth();
		m_Height = GetHeight();
		m_Transparent = Transparent_;

		return true;
	}

	/**
	 *
	 */
	bool Texture::Create(DWORD Width, DWORD Height, D3DFORMAT Format)
	{
		Free();

		if(FAILED(GameCore::Instance().GetGraphic().GetDeviceCOM()->CreateTexture(Width, Height, 0, 0, Format, D3DPOOL_MANAGED, &m_Texture, NULL)))
			return false;

		return true;
	}

	/**
	 *
	 */
	bool Texture::Create(IDirect3DTexture9 *Texture)
	{
		D3DLOCKED_RECT SrcRect, DestRect;
		D3DSURFACE_DESC Desc;

		if ( m_Texture == Texture)
			return false;

		Free();

		Graphic *Graphics = &GameCore::Instance().GetGraphic();

		if( Graphics == NULL)
			return false;

		if(Texture == NULL)
			return true;

		// Copy texture over
		Texture->GetLevelDesc(0, &Desc);
		m_Width  = Desc.Width;
		m_Height = Desc.Height;
		Graphics->GetDeviceCOM()->CreateTexture(m_Width, m_Height, 0, 0, Desc.Format, D3DPOOL_MANAGED, &m_Texture, NULL);

		Texture->LockRect(0, &SrcRect, NULL, D3DLOCK_READONLY);
		m_Texture->LockRect(0, &DestRect, NULL, 0);

		memcpy(DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height);

		m_Texture->UnlockRect(0);
		Texture->UnlockRect(0);

		return true;
	}

	/**
	 *
	 */
	bool Texture::Free()
	{
		if ( m_Texture )
		{
			m_Texture->Release();
		}

		m_Texture = NULL;
		m_Width = m_Height = 0;

		return true;
	}

	/**
	 *
	 */
	bool Texture::IsLoaded()
	{
		if(m_Texture == NULL)
			return false;

		return true;
	}

	/**
	 *
	 */
	IDirect3DTexture9 *Texture::GetTextureCOM()
	{
		return m_Texture;
	}

	/**
	 *
	 */
	unsigned long Texture::GetWidth()
	{
		D3DSURFACE_DESC d3dsd;

		if(m_Texture == NULL)
			return 0;

		if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
			return 0;

		return d3dsd.Width;
	}

	/**
	 *
	 */
	unsigned long Texture::GetHeight()
	{
		D3DSURFACE_DESC d3dsd;

		if(m_Texture == NULL)
			return 0;

		if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
			return 0;

		return d3dsd.Height;
	}

	/**
	 *
	 */
	D3DFORMAT Texture::GetFormat()
	{
		D3DSURFACE_DESC d3dsd;

		if(m_Texture == NULL)
			return D3DFMT_UNKNOWN;

		if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
			return D3DFMT_UNKNOWN;

		return d3dsd.Format;
	}

	/**
	 *
	 */
	bool Texture::Blit(long DestX, long DestY,                   \
						D3DXMATRIX &matTransform,				  \
						long SrcX, long SrcY,                     \
						long Width, long Height,                  \
						D3DCOLOR Color)
	{
		RECT Rect;
		ID3DXSprite *pSprite;
		D3DXVECTOR3 position;

		if(m_Texture == NULL)
			return false;

		if((pSprite = GameCore::Instance().GetGraphic().GetSpriteCOM()) == NULL)
			return false;

		if(!Width)
			Width = m_Width;

		if(!Height)
			Height = m_Height;

		Rect.left = SrcX;
		Rect.top  = SrcY;
		Rect.right = Rect.left + Width;
		Rect.bottom = Rect.top + Height;

		position.x = (float) DestX;
		position.y = (float) DestY;
		position.z = 0;

		if(FAILED(pSprite->SetTransform( &matTransform )))
			return false;

		if(FAILED(pSprite->Draw(m_Texture, &Rect, NULL, &position, Color)))
			return false;

		return true;
	}

	/**
	 *
	 */
	bool Texture::Blit(long DestX, long DestY,                   \
						long SrcX, long SrcY,                     \
						long Width, long Height,                  \
						float XScale, float YScale,               \
						float ZRot,								  \
						D3DCOLOR Color)
	{
		D3DXMATRIX matScale, matRot;
		RECT Rect;
		ID3DXSprite *pSprite;
		//D3DXVECTOR3 center;
		D3DXVECTOR3 position;

		if(m_Texture == NULL)
			return false;

		if((pSprite = GameCore::Instance().GetGraphic().GetSpriteCOM()) == NULL)
			return false;

		if(!Width)
			Width = m_Width;

		if(!Height)
			Height = m_Height;

		Rect.left = SrcX;
		Rect.top  = SrcY;
		Rect.right = Rect.left + Width;
		Rect.bottom = Rect.top + Height;

		position.x = (float) DestX;
		position.y = (float) DestY;
		position.z = 0;

		D3DXMatrixIdentity(&matScale);
		D3DXMatrixScaling(&matScale, XScale, YScale, 1.0f);

		D3DXMatrixIdentity(&matRot);
		D3DXMatrixRotationZ(&matRot, ZRot);

		if(FAILED(pSprite->SetTransform( &( matRot * matScale))))
			return false;

		if(FAILED(pSprite->Draw(m_Texture, &Rect, NULL, &position, Color)))
			return false;

		return true;
	}

} // namespace GameEngine
