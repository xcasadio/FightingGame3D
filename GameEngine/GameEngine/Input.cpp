#include "Input.h"
//#include "Utilities/Macros.h"

#include "Window.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	InputDevice *Input::g_pInputDevice[ MAX_INPUT_DEVICE ];

	///////////////////////////////////////////////////////////////////////
	SINGLETON_IMPL(Input)

	Input::Input()
	{
		m_pDI = NULL;

		for ( int i = 0; i < MAX_INPUT_DEVICE; i++ )
		{
			g_pInputDevice[ MAX_INPUT_DEVICE ] = NULL;
		}
	}

	Input::~Input()
	{
		Shutdown();
	}

	HWND Input::GethWnd()
	{
		return m_hWnd;
	}

	IDirectInput8 *Input::GetDirectInputCOM()
	{
		return m_pDI;
	}

	bool Input::Init(HWND hWnd, HINSTANCE hInst)
	{
		Shutdown();

		m_hWnd = hWnd;

		if( FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &m_pDI, NULL)) )
		{
			Window::Error( true, "FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &m_pDI, NULL))" );
			return false;
		}

		return true;
	}

	bool Input::Shutdown()
	{
		for ( int i = 0; i < MAX_INPUT_DEVICE; i++ )
		{
			if ( g_pInputDevice[i] )
			{
				delete g_pInputDevice[i];
				g_pInputDevice[i] = NULL;
			}
		}

		if ( m_pDI )
			m_pDI->Release();

		m_pDI = NULL;

		m_hWnd = NULL;

		return true;
	}


	//===================================================================================
	InputDevice::InputDevice()
	{
		m_Type = INPUT_DEVICE_TYPE_NONE;
		m_Windowed = true;
		m_pDIDevice = NULL;

		m_MouseState    = (DIMOUSESTATE*) &m_State;
		m_JoystickState = (DIJOYSTATE*) &m_State;

		Clear();
	}

	InputDevice::~InputDevice()
	{
		Free();
	}

	bool InputDevice::Create( short Type, bool Windowed)
	{
		DIDATAFORMAT *DataFormat;
		DIPROPRANGE   DIprg;
		DIPROPDWORD   DIpdw;


		Free();

		switch(Type)
		{
		case INPUT_DEVICE_TYPE_KEYBOARD:
			if( FAILED( Input::Instance().GetDirectInputCOM()->CreateDevice(GUID_SysKeyboard, /*&m_pDIDevice*/ &m_pDIDevice, NULL)) ) 
			{
				Window::Error( true, "FAILED( cInput::Instance().GetDirectInputCOM()->CreateDevice(GUID_SysKeyboard, /*&m_pDIDevice*/ &m_pDIDevice, NULL))" );
				return false;
			}
			DataFormat = (DIDATAFORMAT*)&c_dfDIKeyboard;
			break;

		case INPUT_DEVICE_TYPE_MOUSE:
			if( FAILED( Input::Instance().GetDirectInputCOM()->CreateDevice(GUID_SysMouse, /*&m_pDIDevice*/ &m_pDIDevice, NULL)) ) 
			{
				Window::Error( true, "FAILED( cInput::Instance().GetDirectInputCOM()->CreateDevice(GUID_SysMouse, /*&m_pDIDevice*/ &m_pDIDevice, NULL))" );
				return false;
			}
			DataFormat = (DIDATAFORMAT*)&c_dfDIMouse;
			break;

		case INPUT_DEVICE_TYPE_JOYSTICK:
			if( FAILED( Input::Instance().GetDirectInputCOM()->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticks, this, DIEDFL_ATTACHEDONLY)) )
			{
				Window::Error( true, "FAILED( cInput::Instance().GetDirectInputCOM()->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticks, this, DIEDFL_ATTACHEDONLY))" );
				return false;
			}

			if(m_pDIDevice == NULL)
			{
				//Window::Error( true, "m_pDIDevice == NULL" );
				return false;
			}

			DataFormat = (DIDATAFORMAT*)&c_dfDIJoystick;
			break;

		default: return false;
		}

		m_Windowed = Windowed;

		if( FAILED( m_pDIDevice->SetDataFormat(DataFormat)) )
		{
			Window::Error( true, "FAILED( m_pDIDevice->SetDataFormat(DataFormat))" );
			return false;
		}

		// Set the cooperative level - Foreground & Nonexclusive
		if( FAILED( m_pDIDevice->SetCooperativeLevel(Input::Instance().GethWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)) )
		{
			Window::Error( true, "FAILED( m_pDIDevice->SetCooperativeLevel(cInput::Instance().GethWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))" );
			return false;
		}

		if(Type == INPUT_DEVICE_TYPE_JOYSTICK)
		{
			DIprg.diph.dwSize       = sizeof(DIPROPRANGE);
			DIprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			DIprg.diph.dwHow        = DIPH_BYOFFSET;
			DIprg.lMin              = -1024;
			DIprg.lMax              = +1024;

			DIprg.diph.dwObj = DIJOFS_X;
			if( FAILED( m_pDIDevice->SetProperty(DIPROP_RANGE, &DIprg.diph)) )
			{
				Window::Error( true, "FAILED( m_pDIDevice->SetProperty(DIPROP_RANGE, &DIprg.diph))" );
				return false;
			}

			DIprg.diph.dwObj = DIJOFS_Y;
			if( FAILED( m_pDIDevice->SetProperty(DIPROP_RANGE, &DIprg.diph)) )
			{
				Window::Error( true, "FAILED( m_pDIDevice->SetProperty(DIPROP_RANGE, &DIprg.diph))" );
				return false;
			}

			// Set the special properties of the joystick - deadzone 12%
			DIpdw.diph.dwSize       = sizeof(DIPROPDWORD);
			DIpdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			DIpdw.diph.dwHow        = DIPH_BYOFFSET;
			DIpdw.dwData            = 128;

			// Set X deadzone
			DIpdw.diph.dwObj = DIJOFS_X;
			if( FAILED( m_pDIDevice->SetProperty(DIPROP_DEADZONE, &DIpdw.diph)) )
			{
				Window::Error( true, "FAILED( m_pDIDevice->SetProperty(DIPROP_DEADZONE, &DIpdw.diph))" );
				return false;
			}

			// Set Y deadzone
			DIpdw.diph.dwObj = DIJOFS_Y;
			if( FAILED( m_pDIDevice->SetProperty(DIPROP_DEADZONE, &DIpdw.diph)) )
			{
				Window::Error( true, "FAILED( m_pDIDevice->SetProperty(DIPROP_DEADZONE, &DIpdw.diph))" );
				return false;
			}
		}

		HRESULT hr = m_pDIDevice->Acquire();

		if ( hr != S_OK && hr != DIERR_OTHERAPPHASPRIO )
		{
			Window::Error( true, "hr != S_OK && hr != DIERR_OTHERAPPHASPRIO" );
			return false;
		}

		m_Type = Type;

		Clear();

		return true;
	}

	bool InputDevice::Free()
	{
		if( m_pDIDevice != NULL)
		{
			m_pDIDevice->Unacquire();
			m_pDIDevice->Release();
			m_pDIDevice = NULL;
		}

		m_Type = INPUT_DEVICE_TYPE_NONE;
	 
		Clear();

		return true;
	}

	bool InputDevice::Clear()
	{
		short i;

		ZeroMemory(&m_State, 256);

		for(i=0;i<256;i++)
		m_Locks[i]  = false;

		m_XPos = 0;
		m_YPos = 0;
		m_Wheel = 0;

		return true;
	}

	bool InputDevice::Read()
	{
		HRESULT hr;
		long BufferSizes[3] = { 256, sizeof(DIMOUSESTATE), sizeof(DIJOYSTATE) };
		short i;

		if(m_pDIDevice == NULL)
			return false;

		if(m_Type < 1 || m_Type > 3)
			return false;

		// Loop polling and reading until succeeded or unknown error
		// Also take care of lost-focus problems
		while(1)
		{
			m_pDIDevice->Poll();

			if(SUCCEEDED(hr = m_pDIDevice->GetDeviceState(BufferSizes[m_Type-1], (LPVOID)&m_State)))
				break;

			if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
				return false;

			if(FAILED(m_pDIDevice->Acquire()))
			{
				hr = m_pDIDevice->Acquire();
				while( hr == DIERR_INPUTLOST ) 
					hr = m_pDIDevice->Acquire();

				if( hr == DIERR_OTHERAPPHASPRIO || 
					hr == DIERR_NOTACQUIRED ) 
					return false;

				return true;
			}
			
		}

		// Since only the mouse coordinates are relative, you'll
		// have to deal with them now
		if(m_Type == INPUT_DEVICE_TYPE_MOUSE)
		{
			if(m_Windowed == true)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(Input::Instance().GethWnd(), &pt);
				m_XPos = pt.x;
				m_YPos = pt.y;
			}
			else
			{
				m_XPos += m_MouseState->lX;
				m_YPos += m_MouseState->lY;
			}

			m_Wheel = m_MouseState->lZ;
		}

		switch(m_Type) 
		{
		case INPUT_DEVICE_TYPE_KEYBOARD:
			for(i=0;i<256;i++) 
			{
				if(!(m_State[i] & 0x80))
				m_Locks[i] = false;
			}
			break;

		case INPUT_DEVICE_TYPE_MOUSE:
			for(i=0;i<4;i++)
			{
				if(!(m_MouseState->rgbButtons[i]))
				m_Locks[i] = false;
			}
		break;

		case INPUT_DEVICE_TYPE_JOYSTICK:
			for(i=0;i<32;i++) 
			{
				if(!(m_JoystickState->rgbButtons[i]))
				m_Locks[i] = false;
			}
		break;
		}

		return true;
	}

	bool InputDevice::Acquire(bool Active)
	{
		if(m_pDIDevice == NULL)
			return false;

		if(Active == true)
			m_pDIDevice->Acquire();
		else
			m_pDIDevice->Unacquire();

		return true;
	}

	bool InputDevice::GetLock(char Num)
	{
		return m_Locks[Num];
	}

	bool InputDevice::SetLock(char Num, bool State)
	{
		m_Locks[Num] = State;
		return true;
	}

	long InputDevice::GetXPos()
	{
		if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
			m_XPos = m_JoystickState->lX;

		return m_XPos;
	}

	bool InputDevice::SetXPos(long XPos)
	{
		m_XPos = XPos;
		return true;
	}

	long InputDevice::GetYPos()
	{
		if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
			m_YPos = m_JoystickState->lY;

		return m_YPos;
	}

	bool InputDevice::SetYPos(long YPos)
	{
		m_YPos = YPos;
		return true;
	}

	long InputDevice::GetXDelta()
	{
		switch(m_Type)
		{
		case INPUT_DEVICE_TYPE_MOUSE:
			return m_MouseState->lX;

		case INPUT_DEVICE_TYPE_JOYSTICK:
			return m_JoystickState->lX - m_XPos;

		default: return 0;
		}
	}

	long InputDevice::GetYDelta()
	{
		switch(m_Type)
		{
		case INPUT_DEVICE_TYPE_MOUSE:
			return m_MouseState->lY;

		case INPUT_DEVICE_TYPE_JOYSTICK:
			return m_JoystickState->lY - m_YPos;

		default: return 0;
		}
	}

	long InputDevice::GetWheel()
	{
		if (m_Type == INPUT_DEVICE_TYPE_MOUSE )
			return m_Wheel;

		return 0;
	}

	bool InputDevice::GetKeyState(char Num)
	{
		if(m_State[Num] & 0x80 && m_Locks[Num] == false)
			return true;
		return false;
	}

	bool InputDevice::SetKeyState(char Num, bool State)
	{
		m_State[Num] = State;
		return true;
	}

	bool InputDevice::GetPureKeyState(char Num)
	{
		return ((m_State[Num] & 0x80) ? true : false);
	}

	short InputDevice::GetKeypress(long TimeOut)
	{
		static HKL KeyboardLayout = GetKeyboardLayout(0);
		unsigned char WinKeyStates[256], DIKeyStates[256];
		unsigned short i, ScanCode, VirtualKey, Keys, Num;
		unsigned long EndTime;

		if((m_Type != INPUT_DEVICE_TYPE_KEYBOARD) || (m_pDIDevice == NULL))
			return 0;

		EndTime = GetTickCount() + TimeOut;

		// Loop until timeout or key pressed
		while(1)
		{
			GetKeyboardState(WinKeyStates);

			m_pDIDevice->GetDeviceState(256, DIKeyStates);

			// Scan through looking for key presses
			for(i=0;i<256;i++)
			{
				if(DIKeyStates[i] & 0x80)
				{
					if((VirtualKey = MapVirtualKeyEx((ScanCode = i), 1, KeyboardLayout)))
					{
						Num = ToAsciiEx(VirtualKey, ScanCode, WinKeyStates, &Keys, 0, KeyboardLayout);
						if(Num)
							return Keys;
					}
				}
			}

			if(TimeOut)
			{
				if(GetTickCount() > EndTime)
					return 0;
			}
		}

		return 0;
	}

	long InputDevice::GetNumKeyPresses()
	{
		long i, Num = 0;

		for(i=0;i<256;i++)
		{
			if(m_State[i] & 0x80 && m_Locks[i] == false)
				Num++;
		}

		return Num;
	}

	long InputDevice::GetNumPureKeyPresses()
	{
		long i, Num = 0;

		for(i=0;i<256;i++)
		{
			if(m_State[i] & 0x80)
				Num++;
		}

		return Num;
	}

	bool InputDevice::GetButtonState(char Num)
	{
		char State = 0;

		if(m_Type == INPUT_DEVICE_TYPE_MOUSE)
			State = m_MouseState->rgbButtons[Num];

		if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
			State = m_JoystickState->rgbButtons[Num];

		if(State & 0x80 && m_Locks[Num] == false)
			return true;

		return false;
	}

	bool InputDevice::SetButtonState(char Num, BYTE State)
	{
		if(m_Type == INPUT_DEVICE_TYPE_MOUSE)
		{
			m_MouseState->rgbButtons[Num] = State;
			return true;
		}

		if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
		{
			m_JoystickState->rgbButtons[Num] = State;
			return true;
		}

		return false;
	}

	BYTE InputDevice::GetPureButtonState(char Num)
	{
		if(m_Type == INPUT_DEVICE_TYPE_MOUSE)
			return m_MouseState->rgbButtons[Num];

		if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
			return m_JoystickState->rgbButtons[Num];

		return false;
	}

	long InputDevice::GetNumButtonPresses()
	{
		long i, Num = 0;

		if(m_Type == INPUT_DEVICE_TYPE_MOUSE)
		{
			for(i=0;i<4;i++)
			{
				if(m_MouseState->rgbButtons[i] & 0x80 && m_Locks[i] == false)
					Num++;
			}
		}
		else
		{
			if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
			{
				for(i=0;i<32;i++)
				{
					if(m_JoystickState->rgbButtons[i] & 0x80 && m_Locks[i] == false)
						Num++;
				}
			}
		}

	  return Num;
	}

	long InputDevice::GetNumPureButtonPresses()
	{
		long i, Num = 0;

		if(m_Type == INPUT_DEVICE_TYPE_MOUSE)
		{
			for(i=0;i<4;i++)
			{
			  if(m_MouseState->rgbButtons[i] & 0x80)
				Num++;
			}
		}
		else
		{
			if(m_Type == INPUT_DEVICE_TYPE_JOYSTICK)
			{
				for(i=0;i<32;i++)
				{
				  if(m_JoystickState->rgbButtons[i] & 0x80)
					Num++;
				}
			}
		}

		return Num;
	}

	BOOL FAR PASCAL InputDevice::EnumJoysticks(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef)
	{
		InputDevice *Input;

		if((Input = (InputDevice*) pvRef) == NULL)
			return DIENUM_STOP;

		if(FAILED( Input::Instance().GetDirectInputCOM()->CreateDevice(pdInst->guidInstance, Input->GetLPDirectInputDevice(), NULL)))
			return DIENUM_CONTINUE;

		return DIENUM_STOP;
	}

	IDirectInputDevice8 **InputDevice::GetLPDirectInputDevice(void)
	{ 
		return &m_pDIDevice; 
	}

	IDirectInputDevice8 *InputDevice::GetDirectInputDevice(void)
	{ 
		return m_pDIDevice; 
	}

} // namespace GameEgine

