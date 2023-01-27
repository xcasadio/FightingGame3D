#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Camera.h"
#include "WorldPosition.h"
#include "Light.h"
#include "Material.h"
#include "Texture.h"

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	class GAME_ENGINE_EXPORT Graphic
	{
	public:
		typedef struct s_game_param
		{
			bool fpsLock, frameskip;
			
		} s_game_param;

		Graphic();
		~Graphic();

		IDirect3D9       *GetDirect3DCOM();
		IDirect3DDevice9 *GetDeviceCOM();
		ID3DXSprite      *GetSpriteCOM();

		bool Init();
		bool Shutdown();

		bool SetMode(HWND hWnd, bool Windowed = TRUE, bool UseZBuffer = FALSE, long Width = 0, long Height = 0, char BPP = 0, bool bPerspective = true);

		long GetNumDisplayModes(D3DFORMAT Format) ;
		bool GetDisplayModeInfo(long Num, D3DDISPLAYMODE *Mode, D3DFORMAT Format);

		char GetFormatBPP(D3DFORMAT Format) const;
		bool CheckFormat(D3DFORMAT Format, bool Windowed, bool HAL);

		bool AntiAliasingCapability(D3DMULTISAMPLE_TYPE type_)  const;

		bool Display(const RECT *pSourceRect = NULL, const RECT *pDestRect = NULL, HWND hDestWindowOverride = NULL, const RGNDATA* pDirtyRegion = NULL);

		bool BeginFrame();
		void EndFrame();
		void StartTime();
		DWORD FrameTime() const;
		bool FrameSkip() const;

		bool BeginScene();
		bool EndScene();

		bool BeginSprite(DWORD Flags);
		bool EndSprite();
		unsigned int GetFPS() const;

		bool Clear(long Color = 0, float ZBuffer = 1.0f);
		bool ClearDisplay(long Color = 0);
		bool ClearZBuffer(float ZBuffer = 1.0f);

		long GetWidth() const;
		long GetHeight() const;
		bool GetWindowed() const;
		char GetBPP() const;
		bool GetHAL() const;
		bool GetZBuffer() const;

		float GetFieldOfView();
		float GetViewNear();
		float GetViewFar();

		bool SetPerspective(float FOV = D3DX_PI / 4.0f, float Aspect=1.3333f, float Near=1.0f, float Far=10000.0f);
		bool SetOrtho(float width, float height, float Near=1.0f, float Far=10000.0f);
	    
		bool SetWorldPosition(WorldPosition *WorldPos);
		bool SetCamera(ICamera *Camera);
		bool SetLight(long Num, Light *Light);
		bool SetMaterial(Material *Material);
		bool SetTexture(short Num, Texture *Texture);
	    
		bool SetAmbientLight(unsigned char Red, unsigned char Green, unsigned char Blue);
		bool GetAmbientLight(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		bool EnableLight(long Num, bool Enable = true);
		bool EnableLighting(bool Enable = true);
		bool EnableZBuffer(bool Enable = true);
		bool EnableAlphaBlending(bool Enable = true, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
		bool EnableAlphaTesting(bool Enable = true);

	protected:
		IDirect3D9       *m_pD3D;
		IDirect3DDevice9 *m_pD3DDevice;
		ID3DXSprite      *m_pSprite;

		D3DDISPLAYMODE    m_d3ddm;

		bool              m_Windowed;
		bool              m_ZBuffer;
		bool              m_HAL;

		long              m_Width;
		long              m_Height;
		char              m_BPP;

		unsigned char     m_AmbientRed;
		unsigned char     m_AmbientGreen;
		unsigned char     m_AmbientBlue;

		MSG m_Msg;
		HWND m_HWnd;

		float m_FOV, m_Near, m_Far;

	private:
		bool m_bFrameSkip;
		//DWORD m_FPSTimeStartingPoint;// pour calculer la durée d'un frame
		DWORD m_FrameTimeStartingPoint, m_OldFrameTimeStartingPoint, m_FPSTimeTotal;
		unsigned int m_FPS, m_lastFPS, m_FrameSkipped;

		bool m_AntiAliasingCapability[16];
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _GRAPHIC_H_