#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Config/Export.h"
#include "Resource.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 * Gere les material de DirectX
	 */
	class GAME_ENGINE_EXPORT Material : public IResource
	{
	  public:
		Material();

		D3DMATERIAL9 *GetMaterial();

		bool SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		bool GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		bool SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		bool GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
	    
		bool SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		bool GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
	    
		bool SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		bool GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		bool  SetPower(float Power);
		float GetPower(float Power);

	protected:
		D3DMATERIAL9  m_Material;

	};
} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif //_MATERIAL_H_
