#ifndef _PAD_H_
#define _PAD_H_

#include "Input.h"
#include "Config/Export.h"

#include <string>

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	/*
	enum
	{
		JOYSTICK_1 = 0,
		JOYSTICK_2,
		MAX_JOYSTICK
	};
	*/
	typedef enum ePadButton
	{
		PAD_UP = 0,
		PAD_RIGHT,
		PAD_DOWN,
		PAD_LEFT,
		PAD_BUTTON_1,
		PAD_BUTTON_2,
		PAD_BUTTON_3,
		PAD_BUTTON_4,
		PAD_BUTTON_5,
		PAD_BUTTON_6,
		PAD_BUTTON_7,
		PAD_BUTTON_8,

		MAX_PAD_BUTTON,

		PAD_FIRE = PAD_BUTTON_1,
		PAD_SPECIAL = PAD_BUTTON_2,
		PAD_OK = PAD_BUTTON_1,
		PAD_CANCEL = PAD_BUTTON_2
	}ePadButton;

	typedef enum ePadButtonMask
	{
		MASK_UP = 0x1,
		MASK_RIGHT = 0x2,
		MASK_DOWN = 0x4,
		MASK_LEFT = 0x8,
		MASK_BUTTON_1 = 0x10,
		MASK_BUTTON_2 = 0x20,
		MASK_BUTTON_3 = 0x40,
		MASK_BUTTON_4 = 0x80,
		MASK_BUTTON_5 = 0x100,
		MASK_BUTTON_6 = 0x200,
		MASK_BUTTON_7 = 0x400,
		MASK_BUTTON_8 = 0x800
	}ePadButtonMask;

	typedef enum eMouseButton
	{
		MOUSE_BUTTON0 = 0,
		MOUSE_BUTTON1,
		MOUSE_BUTTON2,
		MOUSE_BUTTON3,
		MOUSE_BUTTON4,
		MOUSE_BUTTON5,
		MOUSE_BUTTON6,
		MOUSE_BUTTON7,

		MOUSE_LBUTTON = MOUSE_BUTTON0,
		MOUSE_RBUTTON = MOUSE_BUTTON1,
		MOUSE_MBUTTON = MOUSE_BUTTON2,
	} eMouseButton;

	/*
	Cette classe permet emuler un pad virtuel. On s'abstrait des divers input
	pour seulement se concentrer sur des touches predefinis.
	*/
	class GAME_ENGINE_EXPORT Pad
	{

	public:
		//Struture d'une clé, c-a-d un bouton d'un input
		typedef struct sPadKey
		{
			unsigned long int key;//bouton de l'input
			e_InputDevice input;
			char joystickNum;

			sPadKey()
			{
				key = 0;
				input = INPUT_DEVICE_TYPE_NONE;
				joystickNum = -1;
			}

		}sPadKey;

		//On definit 2 keys pour une command
		typedef struct sPad
		{
			std::string description;
			sPadKey primaryKey;
			sPadKey secondaryKey;
		}sPadButton;


		/**
		 *
		 */
		Pad();

		/**
		 *
		 */
		~Pad();

		/**
		 * Initilise tous les device
		 */
		 //bool Init(HWND hWnd, HINSTANCE hInst, bool windowed);

		bool Pad::Load(const char* fileName_);
		void Load(sPad* pConfig);

		/**
		 * mise a jour du pad
		 */
		bool Read();

		/**
		 *
		 */
		bool MouseButton(eMouseButton button_);

		/**
		 *
		 */
		int MouseX() const;

		/**
		 *
		 */
		int MouseY() const;

		/**
		 *
		 */
		int GetMouseMotionX() const;

		/**
		 *
		 */
		int GetMouseMotionY() const;

		/**
		 *
		 */
		int MouseWheel() const;

		//TODO : permet d'obtenir les touches appuyés du clavier

		/**
		 *
		 */
		int Key();

		/**
		 *
		 */
		bool GetPureKeyState(char Num);

		/**
		 *
		 */
		bool ButtonPressed(ePadButton button);

		/**
		 *
		 */
		bool ButtonReleased(ePadButton button);

		/**
		 *
		 */
		bool ButtonHeld(ePadButton button);

		/**
		 *
		 */
		DWORD GetButtonPressedValue() const;

		/**
		 *
		 */
		DWORD GetButtonHeldValue() const;

		/**
		 *
		 */
		DWORD GetButtonReleasedValue() const;

		/**
		 *
		 */
		void Shutdown();

		/**
		 *
		 */
		void SetButton(ePadButton button, sPad& pad);

		/**
		 *
		 */
		void SetButtonDescription(ePadButton button_, const char* desc_);

		/**
		 *
		 */
		std::string GetButtonDescription(ePadButton button_) const;

	private:
		//cInput *mInput;
		//cInputDevice mMouse;
		//cInputDevice mKeyboard;
		//cInputDevice *mJoystick[MAX_JOYSTICK];

		sPadButton m_PadButton[MAX_PAD_BUTTON]; // definition des boutons du pad virtuel
		// pour connaitre l'etat des boutons
		DWORD m_ButtonPressed, m_ButtonHeld, m_ButtonReleased;
		DWORD m_PrevButtonHeld, m_PrevButtonState, m_ButtonState;


		/**
		 *
		 */
		void UpdateButton();

		/**
		 *
		 */
		DWORD GetMaskButton(ePadButton button_);
	};

} // namespace GameEngine

#include "DebugEngine/DebugNewOff.h"

#endif // _PAD_H_
