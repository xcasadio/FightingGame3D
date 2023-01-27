#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

#include "Config/Export.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{

	class GAME_ENGINE_EXPORT Window
	{
	public:
		Window();
		~Window();

		bool Create( unsigned int width_, unsigned int height_, const char *caption, bool windowed, HWND parent = NULL);
		HWND GethWnd();
		HINSTANCE GethInst();

		DWORD GetScreenWidth();
		DWORD GetScreenHeight();
		WNDCLASSEX &GetWndClassEX();
		DWORD GetStyle();
		DWORD GetXPos();
		DWORD GetYPos();

		void SetScreenWidth(DWORD w);
		void SetScreenHeight(DWORD h);
		void SetStyle(DWORD s);
		void SetXPos(DWORD x);
		void SetYPos(DWORD y);

		static bool Error(bool fatal_, const char *text_, ...);

		bool Move(long XPos, long YPos);
		bool Resize(long Width, long Height);

		bool ShowMouse(bool Show = true);
		void Close();

	private:
		HINSTANCE     m_hInst;
		HWND		  m_hWnd;

	protected:
		char          m_Class[MAX_PATH];
		char          m_Caption[MAX_PATH];

		WNDCLASSEX    m_wcex;

		DWORD         m_Style;
		DWORD         m_XPos;
		DWORD         m_YPos;
		DWORD         m_Width;
		DWORD         m_Height;
	};

} // namespace GameEngine

//a inclure en dernier
#include "DebugEngine/DebugNewOff.h"

#endif // _WINDOW_H_