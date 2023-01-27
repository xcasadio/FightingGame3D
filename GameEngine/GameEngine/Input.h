#ifndef _INPUT_H_
#define _INPUT_H_

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dinputd.h>

#include "Utilities/Singleton.h"
#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	class Input;
	class InputDevice;

	/**
	 * enumeration des inputs qui peuvent etre créer
	 */
	typedef enum e_InputDevice
	{
		INPUT_DEVICE_TYPE_NONE = -1,
		INPUT_DEVICE_MOUSE = 0,
		INPUT_DEVICE_KEYBOARD,
		INPUT_DEVICE_JOYSTICK1,
		INPUT_DEVICE_JOYSTICK2,
		INPUT_DEVICE_JOYSTICK3,
		INPUT_DEVICE_JOYSTICK4,

		MAX_INPUT_DEVICE
	} e_InputDevice;

	/**
	 *
	 */
	class Input : public ISingleton<Input>
    {
		MAKE_SINGLETON(Input);

	public:
		/**
		 *
		 */
		~Input();

		/**
		 *
		 */
		IDirectInput8 *GetDirectInputCOM();

		/**
		 *
		 */
		HWND           GethWnd();

		/**
		 *
		 */
		bool Init(HWND hWnd, HINSTANCE hInst);

		/**
		 *
		 */
		bool Shutdown();

		/**
		 *
		 */
		static InputDevice *g_pInputDevice[ MAX_INPUT_DEVICE ];

	protected:
		/**
		 *
		 */
		HWND           m_hWnd;

		/**
		 *
		 */
		IDirectInput8 *m_pDI;

	private:
		/**
		 *
		 */
		Input();
	};

	/**
	 * Cette classe gere les input device. Elle agit selon le type spécifié
	 * TODO : Faire generique et derivé selon l'input
	 */
	class InputDevice
	{
	  public:
		/**
		 *
		 */
		typedef enum e_InputDeviceType
		{
			INPUT_DEVICE_TYPE_NONE = 0,
			INPUT_DEVICE_TYPE_KEYBOARD,
			INPUT_DEVICE_TYPE_MOUSE,
			INPUT_DEVICE_TYPE_JOYSTICK
		} e_InputDeviceType;

		/**
		 *
		 */
		short					m_Type;

		/**
		 *
		 */
		IDirectInputDevice8		*m_pDIDevice;

		/**
		 *
		 */
		bool					m_Windowed;

		/**
		 *
		 */
		char					m_State[256];

		/**
		 *
		 */
		DIMOUSESTATE			*m_MouseState;

		/**
		 *
		 */
		DIJOYSTATE				*m_JoystickState;

		/**
		 * pour le clavier
		 */
		bool					m_Locks[256];

		/**
		 * souris
		 */
		long					m_XPos, m_YPos;

		/**
		 * souris
		 */
		int						m_Wheel;
	     
		/**
		 *
		 */
		static BOOL FAR PASCAL EnumJoysticks(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef);

	  public:
		/**
		 *
		 */
		InputDevice();

		/**
		 *
		 */
		~InputDevice();

		/**
		 *
		 */
		bool Create( short Type, bool Windowed = TRUE);
		
		/**
		 *
		 */
		bool Free();
		
		/**
		 *
		 */
		bool Clear();
		
		/**
		 *
		 */
		bool Read();
		
		/**
		 *
		 */
		bool Acquire(bool Active = TRUE);
		
		/**
		 *
		 */
		bool GetLock(char Num);
		
		/**
		 *
		 */
		bool SetLock(char Num, bool State = TRUE);
		
		/**
		 *
		 */
		long GetXPos();
		
		/**
		 *
		 */
		bool SetXPos(long XPos);
		
		/**
		 *
		 */
		long GetYPos();
		
		/**
		 *
		 */
		bool SetYPos(long YPos);
		
		/**
		 *
		 */
		long GetXDelta();
		
		/**
		 *
		 */
		long GetYDelta();
		
		/**
		 *
		 */
		long GetWheel();		
		
		/**
		 *
		 */
		IDirectInputDevice8 **GetLPDirectInputDevice(void);
		
		/**
		 *
		 */
		IDirectInputDevice8 *GetDirectInputDevice(void);

		// Keyboard specific functions
		/**
		 *
		 */
		bool  GetKeyState(char Num);
		
		/**
		 *
		 */
		bool  SetKeyState(char Num, bool State);
		
		/**
		 *
		 */
		bool  GetPureKeyState(char Num);
		
		/**
		 *
		 */
		short GetKeypress(long TimeOut = 0);
		
		/**
		 *
		 */
		long  GetNumKeyPresses();
		
		/**
		 *
		 */
		long  GetNumPureKeyPresses();

		// Mouse/Joystick specific functions
		/**
		 *
		 */
		bool  GetButtonState(char Num);
		
		/**
		 *
		 */
		bool  SetButtonState(char Num, BYTE State);
		
		/**
		 *
		 */
		BYTE  GetPureButtonState(char Num);
		
		/**
		 *
		 */
		long  GetNumButtonPresses();
		
		/**
		 *
		 */
		long  GetNumPureButtonPresses();
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif
