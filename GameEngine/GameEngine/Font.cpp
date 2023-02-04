#include "Font.h"
#include "GameCore.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	FontDX::FontDX(void)
	{
		m_pFont = nullptr;
		m_iSize = 16;
		m_bBold = false;
		m_bItalic = false;
		m_bUnderline = false;
		m_bStrikeout = false;
		m_bIsLoaded = false;
	}

	/**
	 *
	 */
	FontDX::FontDX(const FontDX& font_)
	{
		Copy(font_);
	}

	/**
	 *
	 */
	FontDX& FontDX::operator = (const FontDX& font_)
	{
		Copy(font_);

		return *this;
	}

	/**
	 *
	 */
	FontDX::~FontDX(void)
	{
		Free();
	}

	/**
	 *
	 */
	bool FontDX::Copy(const FontDX& font_)
	{
		if (&font_ != this)
		{
			m_iSize = font_.m_iSize;
			m_bBold = font_.m_bBold;
			m_bItalic = font_.m_bItalic;
			m_bUnderline = font_.m_bUnderline;
			m_bStrikeout = font_.m_bStrikeout;
			m_bIsLoaded = font_.m_bIsLoaded;
			m_pFont = font_.m_pFont;

			if (m_pFont)
			{
				m_pFont->AddRef();
			}
		}

		return true;
	}

	/**
	 *
	 */
	ID3DXFont* FontDX::GetFontCOM() const
	{
		return m_pFont;
	}

	/**
	 *
	 */
	bool FontDX::Create(const char* name, long Size, bool Bold, bool Italic, bool Underline, bool Strikeout)
	{
		/* TODO :
		D3DXCreateFont( m_d3dDevice, kFontSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font );
		*/

		D3DXFONT_DESC DxFontDesc;

		ZeroMemory(&DxFontDesc, sizeof(DxFontDesc));

		strcpy_s(DxFontDesc.FaceName, 32, "Arial");
		DxFontDesc.Height = -Size;
		DxFontDesc.Weight = (Bold == true) ? 700 : 0;
		DxFontDesc.Italic = Italic;
		//DxFontDesc.Underline = Underline;
		//DxFontDesc.StrikeOut = Strikeout;	  

		// Create the font object
		if (FAILED(D3DXCreateFontIndirect(GameCore::Instance().GetGraphic().GetDeviceCOM(), &DxFontDesc, &m_pFont)))
		{
			return false;
		}

		m_bIsLoaded = true;
		m_iSize = Size;
		m_bBold = Bold;
		m_bItalic = Italic;
		m_bUnderline = Underline;
		m_bStrikeout = Strikeout;

		return true;
	}

	/**
	 *
	 */
	void FontDX::Free()
	{
		if (m_pFont)
		{
			m_pFont->Release();
		}

		m_pFont = nullptr;
	}

	/**
	 *
	 */
	bool FontDX::Print(const char* Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format, LPD3DXSPRITE pSprite)
	{
		RECT Rect;

		if (m_pFont == nullptr)
		{
			return false;
		}

		if (!Width)
		{
			Width = 65535;
		}

		if (!Height)
		{
			Height = 65536;
		}

		Rect.left = XPos;
		Rect.top = YPos;
		Rect.right = Rect.left + Width;
		Rect.bottom = Rect.top + Height;

		if (FAILED(m_pFont->DrawText(pSprite, Text, -1, &Rect, Format, Color)))
		{
			return false;
		}

		return true;
	}

} // namespace GameEngine
