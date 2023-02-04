#ifndef _FONT_H_
#define _FONT_H_

#include <d3d9.h>
#include <d3dx9core.h>

#include "Config/Export.h"
#include "Resource.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	//TODO créer la classe qui gere les font en image

	class GAME_ENGINE_EXPORT FontDX : public IResource
	{
	public:
		/**
		 * Constructeur par defaut
		 */
		FontDX(void);

		/**
		 * Constructeur par copie
		 * @param font_ le font à copier
		 */
		FontDX(const FontDX& font_);

		/**
		 * Copie avec =
		 * @param font_ le font à copier
		 * @return une reference sur l'objet courant
		 */
		FontDX& operator = (const FontDX& font_);

		/**
		 * Destructeur
		 */
		~FontDX(void);

		/**
		 * @return le pointeur sur le ID3DXFont
		 */
		ID3DXFont* GetFontCOM() const;

		/**
		 * Charge une police de windows avec les parametres spécifiés
		 * @param szName_ le nom de la police
		 * @param size_ la taille de la police
		 * @param bBold_ gras
		 * @param bItalic italique
		 * @param bUnderline souligné
		 * @param bStrikeout barré
		 * @return true si reussi sinon false
		 */
		bool Create(const char* szName_, long size_ = 16, bool bBold_ = false, bool bItalic = false, bool bUnderline = false, bool bStrikeout = false);

		/**
		 * Libere la memoire
		 */
		void Free();

		/**
		 * Ecrit du texte sur le LPD3DXSPRITE ou sur l'ecran si LPD3DXSPRITE = NULL, aux coordonnées XPos et Ypos.
		 * @param pText le texte à ecrire
		 * @param Xpos coordonnées en x
		 * @param Ypos coordonnées en y
		 * @param Width largeur du rectangle conteneur
		 * @param Height hauteur du rectangle conteneur
		 * @param Color la couleur du texte
		 * @param Format
		 * @param LPD3DXSPRITE spécifie le sprite sur lequel le texte est ecrit, si NULL le texte est ecrit sur l'ecran
		 * @return true si reussi
		 */
		bool Print(const char* pText, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xffffffff, DWORD Format = 0, LPD3DXSPRITE pSprite = nullptr);

	private:
		ID3DXFont* m_pFont;

		int m_iSize;
		bool m_bBold;
		bool m_bItalic;
		bool m_bUnderline;
		bool m_bStrikeout;
		bool m_bIsLoaded;

		/**
		 * @param font_ le font à copier
		 * @return true si la copie à reussi, sinon false
		 */
		bool Copy(const FontDX& font_);
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif _FONT_H_
