#include "Pad.h"
#include "Logger/Logger.h"
#include "Window.h"
#include "Keys.h"

#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	Pad::Pad()
	{
		m_PrevButtonHeld = m_ButtonPressed = m_ButtonHeld = m_ButtonReleased = m_ButtonState = 0;

		/*
		m_PadButton[PAD_OK].primaryKey.key = KEY_RETURN;
		m_PadButton[PAD_OK].primaryKey.input = INPUT_DEVICE_KEYBOARD;
		m_PadButton[PAD_OK].secondaryKey.key = KEY_ENTER;
		m_PadButton[PAD_OK].secondaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_CANCEL].primaryKey.key = KEY_ESCAPE;
		m_PadButton[PAD_CANCEL].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_UP].primaryKey.key = KEY_UP;
		m_PadButton[PAD_UP].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_DOWN].primaryKey.key = KEY_DOWN;
		m_PadButton[PAD_DOWN].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_LEFT].primaryKey.key = KEY_LEFT;
		m_PadButton[PAD_LEFT].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_RIGHT].primaryKey.key = KEY_RIGHT;
		m_PadButton[PAD_RIGHT].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_BUTTON_3].primaryKey.key = KEY_P;
		m_PadButton[PAD_BUTTON_3].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_BUTTON_4].primaryKey.key = KEY_O;
		m_PadButton[PAD_BUTTON_4].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_BUTTON_5].primaryKey.key = KEY_I;
		m_PadButton[PAD_BUTTON_5].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_BUTTON_6].primaryKey.key = KEY_U;
		m_PadButton[PAD_BUTTON_6].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_BUTTON_7].primaryKey.key = KEY_Y;
		m_PadButton[PAD_BUTTON_7].primaryKey.input = INPUT_DEVICE_KEYBOARD;

		m_PadButton[PAD_BUTTON_8].primaryKey.key = KEY_T;
		m_PadButton[PAD_BUTTON_8].primaryKey.input = INPUT_DEVICE_KEYBOARD;
		*/
	}

	/**
	 *
	 */
	Pad::~Pad()
	{
		Shutdown();
	}

	/**
	 *
	 */
	void Pad::Shutdown()
	{
		/*
		int i;

		g_pInputDevice[INPUT_DEVICE_MOUSE]->Free();
		g_pInputDevice[INPUT_DEVICE_KEYBOARD]->Free();

		for ( i=0; i<MAX_JOYSTICK; i++)
		{
			if ( g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i] != NULL)
			{
				g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i]->Free();
				delete g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i];
				g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i] = NULL;
			}
		}


		cInput::Instance().Shutdown();
		cInput::Destroy();
		*/
	}

	/**
	 *
	 *
	bool Pad::Init(HWND hWnd, HINSTANCE hInst, bool windowed)
	{
		int i;

		if ( !cInput::Instance().Init(hWnd, hInst) )
		{
			Window::Error( false, "Echec de la création de l'input");
			return false;
		}

		if ( !g_pInputDevice[INPUT_DEVICE_MOUSE]->Create(cInputDevice::INPUT_DEVICE_TYPE_MOUSE, windowed) )
		{
			Window::Error( false, "Echec de la création de la souris");
			return false;
		}

		if ( !g_pInputDevice[INPUT_DEVICE_KEYBOARD]->Create(cInputDevice::INPUT_DEVICE_TYPE_KEYBOARD, windowed) )
		{
			Window::Error( false, "Echec de la création du clavier");
			return false;
		}

		for ( i=0; i<MAX_JOYSTICK; i++)
		{
			g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i] = NULL;
			g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i] = new cInputDevice();

			if ( !g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i]->Create( cInputDevice::INPUT_DEVICE_TYPE_JOYSTICK, windowed) )
			{
				delete g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i];
				g_pInputDevice[INPUT_DEVICE_JOYSTICK]->[i] = NULL;

	#ifdef _DEBUG
				ILogger::Log() << "joystick absent :" << i << "\n";
	#endif
			}
		}

		return true;
	}
	*/
	/**
	 *
	 */
	bool Pad::Read()
	{
		m_PrevButtonHeld = m_ButtonHeld;
		m_PrevButtonState = m_ButtonState;

		UpdateButton();

		m_ButtonPressed = ~m_PrevButtonState & m_ButtonState;
		m_ButtonHeld = m_ButtonState & m_PrevButtonState;
		m_ButtonReleased = (m_PrevButtonHeld & m_ButtonHeld) ^ m_PrevButtonHeld;

		return true;
	}

	/**
	 *
	 */
	void Pad::UpdateButton()
	{
		bool state;
		int i;

		m_ButtonState = 0;

		for (i = 0; i < MAX_PAD_BUTTON; i++)
		{
			state = false;

			if (m_PadButton[i].primaryKey.input != InputDevice::INPUT_DEVICE_TYPE_NONE)
			{
				switch (m_PadButton[i].primaryKey.input)
				{
				case InputDevice::INPUT_DEVICE_TYPE_MOUSE:
					if (Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetButtonState((char)m_PadButton[i].primaryKey.key))
					{
						m_ButtonState |= 1 << i;
						state = true;
					}
					break;

				case InputDevice::INPUT_DEVICE_TYPE_KEYBOARD:
					// TODO -- GetKeypress car limité sinon aux touches alphabetiques
					if (Input::g_pInputDevice[INPUT_DEVICE_KEYBOARD]->GetPureKeyState((char)m_PadButton[i].primaryKey.key))
					{
						m_ButtonState |= 1 << i;
						state = true;
					}
					break;

				case InputDevice::INPUT_DEVICE_TYPE_JOYSTICK:
					if (Input::g_pInputDevice[m_PadButton[i].primaryKey.joystickNum] != NULL)
					{
						if (Input::g_pInputDevice[m_PadButton[i].primaryKey.joystickNum]->GetButtonState((char)m_PadButton[i].primaryKey.key))
						{
							m_ButtonState |= 1 << i;
							state = true;
						}
					}
					break;
				}
			}

			if (state || m_PadButton[i].secondaryKey.input == InputDevice::INPUT_DEVICE_TYPE_NONE)
			{
				continue;
			}


			switch (m_PadButton[i].secondaryKey.input)
			{
			case InputDevice::INPUT_DEVICE_TYPE_MOUSE:
				if (Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetButtonState((char)m_PadButton[i].secondaryKey.key))
				{
					m_ButtonState |= 1 << i;
				}
				break;

			case InputDevice::INPUT_DEVICE_TYPE_KEYBOARD:
				// TODO -- GetKeypress car limité sinon aux touches alphabetiques
				if (Input::g_pInputDevice[INPUT_DEVICE_KEYBOARD]->GetPureKeyState((char)m_PadButton[i].secondaryKey.key))
				{
					m_ButtonState |= 1 << i;
				}
				break;

			case InputDevice::INPUT_DEVICE_TYPE_JOYSTICK:
				if (Input::g_pInputDevice[m_PadButton[i].secondaryKey.joystickNum] != NULL)
				{
					if (Input::g_pInputDevice[m_PadButton[i].secondaryKey.joystickNum]->GetButtonState((char)m_PadButton[i].secondaryKey.key))
					{
						m_ButtonState |= 1 << i;
					}
				}
				break;
			}
		}
	}

	/**
	 *
	 */
	bool Pad::MouseButton(eMouseButton button_)
	{
		return Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetButtonState((char)button_);
	}

	/**
	 *
	 */
	int Pad::MouseX() const
	{
		return Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetXPos();
	}

	/**
	 *
	 */
	int Pad::MouseY() const
	{
		return Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetYPos();
	}

	/**
	 *
	 */
	int Pad::GetMouseMotionX() const
	{
		return Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetXDelta();
	}

	/**
	 *
	 */
	int Pad::GetMouseMotionY() const
	{
		return Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetYDelta();
	}

	/**
	 *
	 */
	int Pad::MouseWheel() const
	{
		return Input::g_pInputDevice[INPUT_DEVICE_MOUSE]->GetWheel();
	}

	/**
	 *
	 */
	int Pad::Key()
	{
		return Input::g_pInputDevice[INPUT_DEVICE_KEYBOARD]->GetKeypress();
	}

	/**
	 *
	 */
	bool Pad::GetPureKeyState(char Num_)
	{
		return Input::g_pInputDevice[INPUT_DEVICE_KEYBOARD]->GetPureKeyState(Num_);
	}

	/**
	 *
	 */
	bool Pad::ButtonPressed(ePadButton key_)
	{
		return (m_ButtonPressed & GetMaskButton(key_)) == GetMaskButton(key_);
	}

	/**
	 *
	 */
	bool Pad::ButtonReleased(ePadButton key_)
	{
		return (m_ButtonReleased & GetMaskButton(key_)) == GetMaskButton(key_);
	}

	/**
	 *
	 */
	bool Pad::ButtonHeld(ePadButton key_)
	{
		return (m_ButtonHeld & GetMaskButton(key_)) == GetMaskButton(key_);
	}

	/**
	 *
	 */
	DWORD Pad::GetMaskButton(ePadButton button_)
	{
		switch (button_)
		{
		case PAD_UP: return MASK_UP;
		case PAD_DOWN: return MASK_DOWN;
		case PAD_RIGHT: return MASK_RIGHT;
		case PAD_LEFT: return MASK_LEFT;
		case PAD_BUTTON_1: return MASK_BUTTON_1;
		case PAD_BUTTON_2: return MASK_BUTTON_2;
		case PAD_BUTTON_3: return MASK_BUTTON_3;
		case PAD_BUTTON_4: return MASK_BUTTON_4;
		case PAD_BUTTON_5: return MASK_BUTTON_5;
		case PAD_BUTTON_6: return MASK_BUTTON_6;
		case PAD_BUTTON_7: return MASK_BUTTON_7;
		case PAD_BUTTON_8: return MASK_BUTTON_8;
		default: return 0;
		}
	}

	/**
	 *
	 */
	void Pad::Load(sPad *pConfig)
	{
		for (int i = 0; i < MAX_PAD_BUTTON; i++)
		{
			m_PadButton[i].description = pConfig[i].description;
			m_PadButton[i].primaryKey.input = pConfig[i].primaryKey.input;
			m_PadButton[i].primaryKey.key = pConfig[i].primaryKey.key;
			m_PadButton[i].primaryKey.joystickNum = pConfig[i].primaryKey.joystickNum;
			m_PadButton[i].secondaryKey.input = pConfig[i].secondaryKey.input;
			m_PadButton[i].secondaryKey.key = pConfig[i].secondaryKey.key;
			m_PadButton[i].secondaryKey.joystickNum = pConfig[i].secondaryKey.joystickNum;
		}
	}

	/**
	 *
	 */
	DWORD Pad::GetButtonPressedValue() const
	{
		return m_ButtonPressed;
	}

	/**
	 *
	 */
	DWORD Pad::GetButtonHeldValue() const
	{
		return m_ButtonHeld;
	}

	/**
	 *
	 */
	DWORD Pad::GetButtonReleasedValue() const
	{
		return m_ButtonReleased;
	}

	/**
	 *
	 */
	void Pad::SetButton(ePadButton button_, sPad& pad_)
	{
		m_PadButton[button_].description = pad_.description;

		m_PadButton[button_].primaryKey.key = pad_.primaryKey.key;
		m_PadButton[button_].primaryKey.input = pad_.primaryKey.input;
		m_PadButton[button_].primaryKey.joystickNum = pad_.primaryKey.joystickNum;

		m_PadButton[button_].secondaryKey.key = pad_.secondaryKey.key;
		m_PadButton[button_].secondaryKey.input = pad_.secondaryKey.input;
		m_PadButton[button_].secondaryKey.joystickNum = pad_.secondaryKey.joystickNum;
	}

	/**
	 *
	 */
	void Pad::SetButtonDescription(ePadButton button_, const char* desc_)
	{
		m_PadButton[button_].description.clear();
		m_PadButton[button_].description.append(desc_);
	}

	/**
	 *
	 */
	std::string Pad::GetButtonDescription(ePadButton button_) const
	{
		return m_PadButton[button_].description;
	}

} // namespace GameEgine