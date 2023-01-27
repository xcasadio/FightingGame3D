#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include <string>

#include "Config/Export.h"
#include "Resource.h"

#include "Utilities/Serializable.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Gere les textures de DirectX
	 */
	class GAME_ENGINE_EXPORT Texture : 
		public IResource,
		public ISerializable
	{
	public:
		/**
		 *
		 */
		Texture();

		/**
		 *
		 */
		~Texture();

		/**
		 * Permet de charger le fichier de serialisation
		 * pour initialiser un objet
		 */
		//virtual bool LoadSerialized(const char *fileName_);

		/**
		 * Permet sauvegarder (serialisation) l'etat
		 * de l'objet courant
		 */
		//virtual bool SaveSerialized(const char *fileName_);

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
		 * @ return texture COM
		 */
		IDirect3DTexture9 *GetTextureCOM();
		
		/**
		 * Charge une image png, jpg, gif ou bmp
		 * @param Filename le nom de l'image à charger
		 * @param Transparent la couleur transparente
		 * @param Format le format de l'image
		 * @return true si reussi sinon false
		 */
		bool Load(const char *Filename,
				  DWORD Transparent = 0,
				  D3DFORMAT Format = D3DFMT_UNKNOWN);

		/**
		 * Créer une texture avec les caracteristiques spécifiées
		 * @param Width largeur
		 * @param Height hauteur
		 * @param Format le format de l'image
		 * @return true si reussi sinon false
		 */
		bool Create(DWORD Width, DWORD Height, D3DFORMAT Format);

		// Configure a Texture class from an existing
		// IDirect3DTexture9 object instance.
		/**
		 * Recopie la texture d'un autre objet
		 * @param Texture le pointeur IDirect3DTexture9 sur le COM de la texture
		 * @return true si reussi sinon false
		 */
		bool Create(IDirect3DTexture9 *Texture);

		/**
		 * @return true si reussi sinon false
		 */
		bool Free();

		/**
		 * @return true si la texture est chargée correctement sinon false
		 */
		bool IsLoaded();

		/**
		 * @return la largeur de la texture
		 */
		unsigned long GetWidth();
		
		/**
		 * @return la hauteur de la texture
		 */
		unsigned long GetHeight();
		
		/**
		 * @return le format de la texture
		 */
		D3DFORMAT GetFormat();

		/**
		 * Affiche la texture en appliquant la transformation
		 * contenu dans la matrix matTransform
		 */
		bool Blit(long DestX, long DestY,                          \
				  D3DXMATRIX &matTransform,						   \
				  long SrcX = 0, long SrcY = 0,                    \
				  long Width = 0, long Height = 0,                 \
				  D3DCOLOR Color = 0xFFFFFFFF);

		/**
		 * Affiche la texture
		 */
		bool Blit(long DestX, long DestY,                          \
				  long SrcX = 0, long SrcY = 0,                    \
				  long Width = 0, long Height = 0,                 \
				  float XScale = 1.0f, float YScale = 1.0f,        \
				  float ZRot = 0.0f,							   \
				  D3DCOLOR Color = 0xFFFFFFFF);

	protected:
		IDirect3DTexture9 *m_Texture;
		unsigned long m_Width, m_Height;
		DWORD m_Transparent;
	};
} // namespace GameEngine

//a inclure en dernier
#include "DebugEngine/DebugNewOff.h"

#endif