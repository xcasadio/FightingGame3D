#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9core.h>

#include "Graphic.h"

#include "Window.h"

#ifdef _DEBUG
#	include "Logger/Logger.h"
#endif

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/**
	 *
	 */
	Graphic::Graphic()
	{
		m_HWnd = nullptr;

		m_pD3D = nullptr;
		m_pD3DDevice = nullptr;
		m_pSprite = nullptr;
		m_AmbientRed = m_AmbientGreen = m_AmbientBlue = 255;

		m_Width = 0;
		m_Height = 0;
		m_BPP = 0;

		m_Windowed = true;
		m_ZBuffer = false;
		m_HAL = false;

		m_bFrameSkip = false;
		//m_FPSTimeStartingPoint = 0;
		m_FrameTimeStartingPoint = 0;
		m_FPS = 0;
		m_lastFPS = 0;
		m_FrameSkipped = 0;
		m_FPSTimeTotal = 0;

		m_FOV = D3DX_PI / 4.0f;
		m_Near = m_Far = 0.0f;

		ZeroMemory(&m_Msg, sizeof(MSG));
	}

	/**
	 *
	 */
	Graphic::~Graphic()
	{
		Shutdown();
	}

	/**
	 *
	 */
	bool Graphic::Init()
	{
		Shutdown();

		if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
		{
			Window::Error(true, "Direct3DCreate9(D3D_SDK_VERSION) == NULL");
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetMode(HWND hWnd, bool Windowed, bool UseZBuffer, long Width, long Height, char BPP, bool bPerspective)
	{
		D3DPRESENT_PARAMETERS d3dpp;
		D3DFORMAT             Format, AltFormat;
		RECT                 /* WndRect,*/ ClientRect;
		//long                  WndWidth, WndHeight;

		// Error checking
		if ((m_HWnd = hWnd) == nullptr)
		{
			Window::Error(true, "(m_HWnd = hWnd) == NULL");
			return false;
		}

		if (m_pD3D == nullptr)
		{
			Window::Error(true, "m_pD3D == NULL");
			return false;
		}

		// Get the current display format
		if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm)))
		{
			Window::Error(true, "FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm))");
			return false;
		}

		// Configure width
		if (!Width)
		{
			// Default to screen width if fullscreen
			if (Windowed == false)
			{
				m_Width = m_d3ddm.Width;
			}
			else
			{
				// Otherwise grab from client size
				GetClientRect(hWnd, &ClientRect);
				m_Width = ClientRect.right;
			}
		}
		else
		{
			m_Width = Width;
		}

		// Configure height
		if (!Height)
		{
			// Default to screen height if fullscreen
			if (Windowed == false)
			{
				m_Height = m_d3ddm.Height;
			}
			else
			{
				// Otherwise grab from client size
				GetClientRect(hWnd, &ClientRect);
				m_Height = ClientRect.bottom;
			}
		}
		else
		{
			m_Height = Height;
		}

		// Configure BPP
		if (!(m_BPP = BPP) || Windowed == true)
		{
			if (!(m_BPP = GetFormatBPP(m_d3ddm.Format)))
			{
				Window::Error(true, "!(m_BPP = GetFormatBPP(m_d3ddm.Format))");
				return false;
			}
		}

#ifdef _DEBUG
		ILogger::Log() << m_Width << "x" << m_Height << "x" << (int)m_BPP << (Windowed == true ? " fenetre" : " plein ecran") << "\n";
#endif

		// Resize client window if using windowed mode
/*
		if(Windowed == true)
		{
			GetWindowRect(hWnd, &WndRect);
			GetClientRect(hWnd, &ClientRect);

			WndWidth  = (WndRect.right  - (ClientRect.right  - m_Width))  - WndRect.left;
			WndHeight = (WndRect.bottom - (ClientRect.bottom - m_Height)) - WndRect.top;

			MoveWindow(hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, true);

#ifdef _DEBUG
			ILogger::Log() << "Fenetre : " << WndRect.left << "," << WndRect.top << "," << WndWidth << "," << WndHeight << "\n";
#endif
		}
*/
// Clear presentation structure
		ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		// Default to no hardware acceleration detected
		m_HAL = false;

		// Setup Windowed or fullscreen usage
		if ((m_Windowed = Windowed) == true)
		{
			d3dpp.Windowed = true;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_COPY;
			d3dpp.BackBufferFormat = m_d3ddm.Format;

			// See if card supports HAL
			if (CheckFormat(m_d3ddm.Format, true, true) == true)
			{
				m_HAL = true;

#ifdef _DEBUG
				ILogger::Log() << "GPU vertex processing" << "\n";
#endif
			}
			else
			{
				// Return error if not emulated
				if (CheckFormat(m_d3ddm.Format, true, false) == false)
				{
					Window::Error(true, "CheckFormat( m_d3ddm.Format, true, false ) == false");
					return false;
				}
#ifdef _DEBUG
				ILogger::Log() << "CPU vertex processing" << "\n";
#endif
			}
		}
		else
		{
			d3dpp.Windowed = false;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_COPY; FLIP

			d3dpp.BackBufferWidth = m_Width;
			d3dpp.BackBufferHeight = m_Height;
			d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //D3DPRESENT_INTERVAL_DEFAULT or D3DPRESENT_INTERVAL_ONE or D3DPRESENT_INTERVAL_IMMEDIATE

			// Figure display format to use
			if (m_BPP == 32)
			{
				Format = D3DFMT_X8R8G8B8;
				AltFormat = D3DFMT_X8R8G8B8;
			}
			if (m_BPP == 24)
			{
				Format = D3DFMT_R8G8B8;
				AltFormat = D3DFMT_R8G8B8;
			}
			if (m_BPP == 16)
			{
				Format = D3DFMT_R5G6B5;
				AltFormat = D3DFMT_X1R5G5B5;
			}
			if (m_BPP == 8)
			{
				Format = D3DFMT_P8;
				AltFormat = D3DFMT_P8;
			}

			// Check for HAL device
			if (CheckFormat(Format, false, true) == true)
			{
				m_HAL = true;
#ifdef _DEBUG
				ILogger::Log() << "GPU vertex processing" << "\n";
#endif
			}
			else
			{
				// Check for HAL device in alternate format
				if (CheckFormat(AltFormat, false, true) == true)
				{
					m_HAL = true;
#ifdef _DEBUG
					ILogger::Log() << "GPU vertex processing" << "\n";
#endif
					Format = AltFormat;
				}
				else
				{
					// Check for Emulation device
					if (CheckFormat(Format, false, false) == false)
					{
						// Check for Emulation device in alternate format
						if (CheckFormat(AltFormat, false, false) == false)
						{
							Window::Error(true, "CheckFormat(AltFormat, false, false) == false");
							return false;
						}
						else
						{
							Format = AltFormat;
						}
#ifdef _DEBUG
						ILogger::Log() << "CPU vertex processing" << "\n";
#endif
					}
				}
			}

			d3dpp.BackBufferFormat = Format;
		}

		// Setup Zbuffer format - 16 bit
		if ((m_ZBuffer = UseZBuffer) == true)
		{
			d3dpp.EnableAutoDepthStencil = true;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16; //D3DFMT_D16
		}
		else
		{
			d3dpp.EnableAutoDepthStencil = false;
		}

		//Anti Aliasing
		DWORD total;

		for (int i = 0; i < 16; i++)
		{
			if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, (m_HAL == true) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
				m_d3ddm.Format,
				Windowed,
				(D3DMULTISAMPLE_TYPE)(i + 1),
				&total)))
			{
				d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)(i + 1);
				d3dpp.MultiSampleQuality = total - 1;
				m_AntiAliasingCapability[i] = true;
			}
			else
			{
				m_AntiAliasingCapability[i] = false;
			}
		}

#ifdef _DEBUG
		ILogger::Log() << "Anti-Aliasing x" << d3dpp.MultiSampleType << ", " << d3dpp.MultiSampleQuality << "\n";
#endif

		// Create the Direct3D Device object
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			(m_HAL == true) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
			hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &m_pD3DDevice)))
		{
			bool CreateDeviceOK = false;
			// Try to create Direct3D without ZBuffer support 
			// if selected and first call failed.
			/*if(m_ZBuffer == true)
			{
				m_ZBuffer = false;
				d3dpp.EnableAutoDepthStencil = false;

				if( FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
						   (m_HAL == true) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
						   hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
						   &d3dpp, &m_pD3DDevice )) )
				{
					//Window::Error( false, "FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, ...) m_ZBuffer = false, D3DCREATE_HARDWARE_VERTEXPROCESSING )" );
					//return false;
					m_ZBuffer = true;
				}
				else
				{
					CreateDeviceOK = true;
				}
			}*/

			if (CreateDeviceOK == false)
			{
				if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
					(m_HAL == true) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
					hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp, &m_pD3DDevice)))
				{
					// Try to create Direct3D without ZBuffer support 
					// if selected and first call failed.
					/*if(m_ZBuffer == true)
					{
						m_ZBuffer = false;
						d3dpp.EnableAutoDepthStencil = false;

						if( FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
								   (m_HAL == true) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
								   hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								   &d3dpp, &m_pD3DDevice )) )
						{
							Window::Error( true, "FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, ...) m_ZBuffer = false ), D3DCREATE_SOFTWARE_VERTEXPROCESSING" );
							return false;
						}
					}
					else
					{*/
					Window::Error(true, "FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, ...) ), D3DCREATE_SOFTWARE_VERTEXPROCESSING");
					return false;
					//}
				}
			}
		}

		/*
		#ifdef _DEBUG
				else
					ILogger::Log() << (m_HAL == true ? "D3DDEVTYPE_HAL " : "D3DDEVTYPE_REF ") << "- D3DCREATE_HARDWARE_VERTEXPROCESSING" << "\n";
		#endif
		*/
		// Set default rendering states
		EnableLighting(false);
		EnableZBuffer(m_ZBuffer);
		EnableAlphaBlending(false);
		EnableAlphaTesting(false);

		// Enable texture rendering stages and filter types
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

		m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

		// Set default ambient color to white
		SetAmbientLight(255, 255, 255);

		// Calculate the aspect ratio based on window size
		if (bPerspective)
		{
			SetPerspective(D3DX_PI / 4.0f, (float)m_Width / (float)m_Height, 1.0f, 10000.0f);
		}
		else
		{
			SetOrtho((float)m_Width, (float)m_Height);
		}

		// Create a sprite interface
		if (FAILED(D3DXCreateSprite(m_pD3DDevice, &m_pSprite)))
		{
			Window::Error(true, "FAILED(D3DXCreateSprite( m_pD3DDevice, &m_pSprite ))");
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::Shutdown()
	{
		if (m_pSprite)
		{
			m_pSprite->Release();
		}

		if (m_pD3DDevice)
		{
			m_pD3DDevice->Release();
		}

		if (m_pD3D)
		{
			m_pD3D->Release();
		}

		m_pSprite = nullptr;
		m_pD3DDevice = nullptr;
		m_pD3D = nullptr;

		return true;
	}

	/**
	 *
	 */
	IDirect3D9* Graphic::GetDirect3DCOM()
	{
		return m_pD3D;
	}

	/**
	 *
	 */
	IDirect3DDevice9* Graphic::GetDeviceCOM()
	{
		return m_pD3DDevice;
	}

	/**
	 *
	 */
	ID3DXSprite* Graphic::GetSpriteCOM()
	{
		return m_pSprite;
	}

	/**
	 *
	 */
	long Graphic::GetNumDisplayModes(D3DFORMAT Format)
	{
		if (m_pD3D == nullptr)
		{
			return 0;
		}

		return (long)m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, Format);
	}

	/**
	 *
	 */
	bool Graphic::GetDisplayModeInfo(long Num, D3DDISPLAYMODE* Mode, D3DFORMAT Format)
	{
		if (m_pD3D == nullptr)
		{
			return false;
		}

		long Max = GetNumDisplayModes(Format);
		if (Num >= Max)
		{
			return false;
		}

		if (FAILED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, Format, Num, Mode)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	char Graphic::GetFormatBPP(D3DFORMAT Format) const
	{
		switch (Format)
		{
			// 32 bit modes
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			return 32;
			break;

			// 24 bit modes
		case D3DFMT_R8G8B8:
			return 24;
			break;

			// 16 bit modes
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
			return 16;
			break;

			// 8 bit modes
		case D3DFMT_A8P8:
		case D3DFMT_P8:
			return 8;
			break;

		default:
			return 0;
		}
	}

	/**
	 *
	 */
	bool Graphic::CheckFormat(D3DFORMAT Format, bool Windowed, bool HAL)
	{
		if (FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,
			(HAL == true) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
			Format, Format, Windowed)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::AntiAliasingCapability(D3DMULTISAMPLE_TYPE type_) const
	{
		return m_AntiAliasingCapability[(int)type_ - 1];
	}

	/**
	 *
	 */
	bool Graphic::BeginFrame()
	{
		if (PeekMessage(&m_Msg, m_HWnd, 0, 0, PM_REMOVE))
		{
			if (m_Msg.message == WM_CLOSE)
			{
				//DestroyWindow( m_HWnd );
				//PostQuitMessage(0);
				return false;
			}
			else if (m_Msg.message == WM_QUIT)
			{
				return false;
			}

			TranslateMessage(&m_Msg);
			DispatchMessage(&m_Msg);
		}

		DWORD time = GetTickCount();

		m_OldFrameTimeStartingPoint = time - m_FrameTimeStartingPoint;
		m_FrameTimeStartingPoint = time;

		return true;
	}

	/**
	 *
	 */
	void Graphic::EndFrame()
	{
		m_FPS++;
		DWORD time = GetTickCount();
		DWORD timeDiff = time - m_FPSTimeTotal;

		if (timeDiff >= 1000)
		{
			m_lastFPS = m_FPS - m_FrameSkipped;
			m_FrameSkipped = 0;
			m_bFrameSkip = false;
			m_FPS = 0;
			//TODO moins le temps sup a 1000
			m_FPSTimeTotal = time;
		}
		else if (m_FPS) // frameskipping
		{
			//On teste si le nombre de frame affiché est inferieur à 30fps
			if (time - m_FrameTimeStartingPoint > 33)
			{
				m_bFrameSkip = true;
				m_FrameSkipped++;
			}
			else
			{
				m_bFrameSkip = false;
			}
		}

		//cas fenetre
	}

	/**
	 *
	 */
	void Graphic::StartTime()
	{
		m_FrameTimeStartingPoint = GetTickCount();
		m_OldFrameTimeStartingPoint = 0;
	}

	/**
	 *
	 */
	DWORD Graphic::FrameTime() const
	{
		return m_OldFrameTimeStartingPoint;
	}

	/**
	 *
	 */
	bool Graphic::FrameSkip() const
	{
		return m_bFrameSkip;
	}

	/**
	 *
	 */
	unsigned int Graphic::GetFPS() const
	{
		return m_lastFPS;
	}

	/**
	 *
	 */
	bool Graphic::BeginScene()
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->BeginScene()))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EndScene()
	{
		// Error checking
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		// Release all textures
		for (short i = 0; i < 8; i++)
		{
			m_pD3DDevice->SetTexture(i, nullptr);
		}

		// End the scene
		if (FAILED(m_pD3DDevice->EndScene()))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::BeginSprite(DWORD Flags)
	{
		if (m_pSprite == nullptr)
		{
			return false;
		}

		if (FAILED(m_pSprite->Begin(Flags)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EndSprite()
	{
		if (m_pSprite == nullptr)
		{
			return false;
		}

		if (FAILED(m_pSprite->End()))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::Display(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion)))
		{
			return false;
		}

		/*
		hr=D3D_Device->Present( NULL, NULL, NULL, NULL );
		if (hr==D3DERR_DEVICELOST) D3D_Device->Reset(&d3dpp);
		*/

		return true;
	}

	/**
	 *
	 */
	bool Graphic::Clear(long Color, float ZBuffer)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		// Only clear screen if no zbuffer
		if (m_ZBuffer == false)
		{
			return ClearDisplay(Color);
		}

		// Clear display and zbuffer
		if (FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Color, ZBuffer, 0)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::ClearDisplay(long Color)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::ClearZBuffer(float ZBuffer)
	{
		if (m_pD3DDevice == nullptr || m_ZBuffer == false)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	long Graphic::GetWidth() const
	{
		return m_Width;
	}

	/**
	 *
	 */
	long Graphic::GetHeight() const
	{
		return m_Height;
	}

	/**
	 *
	 */
	bool Graphic::GetWindowed() const
	{
		return m_Windowed;
	}

	/**
	 *
	 */
	char Graphic::GetBPP() const
	{
		return m_BPP;
	}

	/**
	 *
	 */
	bool Graphic::GetHAL() const
	{
		return m_HAL;
	}

	/**
	 *
	 */
	bool Graphic::GetZBuffer() const
	{
		return m_ZBuffer;
	}

	/**
	 *
	 */
	bool Graphic::SetCamera(ICamera* Camera)
	{
		if (m_pD3DDevice == nullptr || Camera == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, Camera->GetMatrix())))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetWorldPosition(WorldPosition* WorldPos)
	{
		if (WorldPos == nullptr || m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLD, WorldPos->GetMatrix())))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetLight(long Num, Light* Light)
	{
		if (Light == nullptr)
		{
			return false;
		}

		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->SetLight(Num, Light->GetLight())))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetAmbientLight(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		D3DCOLOR Color = D3DCOLOR_XRGB((m_AmbientRed = Red), (m_AmbientGreen = Green), (m_AmbientBlue = Blue));
		if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Color)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::GetAmbientLight(unsigned char* Red, unsigned char* Green, unsigned char* Blue)
	{
		if (Red != nullptr)
		{
			*Red = m_AmbientRed;
		}

		if (Green != nullptr)
		{
			*Green = m_AmbientGreen;
		}

		if (Blue != nullptr)
		{
			*Blue = m_AmbientBlue;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetMaterial(Material* Material)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (Material != nullptr)
		{
			if (FAILED(m_pD3DDevice->SetMaterial(Material->GetMaterial())))
			{
				return false;
			}
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetTexture(short Num, Texture* Texture)
	{
		// Error checking
		if (m_pD3DDevice == nullptr || Num < 0 || Num > 7)
		{
			return false;
		}

		if (Texture == nullptr)
		{
			// Clear the texture
			if (FAILED(m_pD3DDevice->SetTexture(Num, NULL)))
			{
				return NULL;
			}
		}
		else
		{
			// Set the texture
			if (FAILED(m_pD3DDevice->SetTexture(Num, Texture->GetTextureCOM())))
			{
				return false;
			}
		}

		return true;
	}

	/**
	 *
	 */
	float Graphic::GetFieldOfView()
	{
		return m_FOV;
	}

	/**
	 *
	 */
	float Graphic::GetViewNear()
	{
		return m_Near;
	}

	/**
	 *
	 */
	float Graphic::GetViewFar()
	{
		return m_Far;
	}

	/**
	 *
	 */
	bool Graphic::SetPerspective(float FOV, float Aspect, float Near, float Far)
	{
		D3DXMATRIX matProjection;

		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		m_FOV = FOV;

		D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);

		if (FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::SetOrtho(float width, float height, float Near, float Far)
	{
		D3DXMATRIX matProjection;

		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		D3DXMatrixOrthoLH(&matProjection, width, height, Near, Far);

		if (FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EnableLight(long Num, bool Enable)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->LightEnable(Num, Enable)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EnableLighting(bool Enable)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, Enable)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EnableZBuffer(bool Enable)
	{
		if (m_pD3DDevice == nullptr || m_ZBuffer == false)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, (Enable == true) ? D3DZB_TRUE : D3DZB_FALSE)))
		{
			return false;
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EnableAlphaBlending(bool Enable, DWORD Src, DWORD Dest)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		// Enable or disable
		if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable)))
		{
			return false;
		}

		// Set blend type
		if (Enable == true)
		{
			m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, Src);
			m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, Dest);
		}

		return true;
	}

	/**
	 *
	 */
	bool Graphic::EnableAlphaTesting(bool Enable)
	{
		if (m_pD3DDevice == nullptr)
		{
			return false;
		}

		if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, Enable)))
		{
			return false;
		}

		// Set test type
		if (Enable == true)
		{
			m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
			m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		}

		return true;
	}

} // namespace GameEgine
