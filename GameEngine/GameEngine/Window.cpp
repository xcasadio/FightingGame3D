#include "Window.h"

#include "GameCore.h"

#include <stdio.h>

namespace GameEngine
{
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	Window::Window()
	{
		static int nb = 0;

		m_XPos = 0;
		m_YPos = 0;
		m_Width = 800;
		m_Height = 600;
		m_Style = WS_OVERLAPPEDWINDOW;
		m_hWnd = nullptr;
		m_hInst = nullptr;

		sprintf_s(m_Class, MAX_PATH, "WindowClass%d", nb);
		nb++;

		ZeroMemory(&m_wcex, sizeof(WNDCLASSEX));

		m_wcex.cbSize = sizeof(WNDCLASSEX);
		m_wcex.style = CS_CLASSDC;//CS_HREDRAW | CS_VREDRAW;
		m_wcex.lpfnWndProc = (WNDPROC)WindowProc;
		m_wcex.cbClsExtra = 0L;
		m_wcex.cbWndExtra = 0L;
		m_wcex.hInstance = m_hInst;
		m_wcex.hIcon = nullptr;
		m_wcex.hCursor = nullptr;
		m_wcex.hbrBackground = nullptr;
		m_wcex.lpszMenuName = nullptr;
		m_wcex.lpszClassName = m_Class;
		m_wcex.hIconSm = nullptr;
	}

	Window::~Window()
	{

	}

	bool Window::Create(unsigned int width_, unsigned int height_, const char* caption, bool windowed, HWND parent)
	{
		m_hInst = GetModuleHandle(nullptr);

		if (caption != nullptr)
		{
			strcpy_s(m_Caption, MAX_PATH, caption);
		}
		else
		{
			strcpy_s(m_Caption, MAX_PATH, "Title");
		}

		if (!RegisterClassEx(&m_wcex))
		{
			Window::Error(true, "RegisterClassEx echec");
			return false;
		}

		m_Style = windowed ? WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU : WS_EX_TOPMOST | WS_POPUP;

		m_Width = width_;
		m_Height = height_;

		m_hWnd = CreateWindowEx(NULL,
			m_Class,
			m_Caption,
			m_Style,
			m_XPos, m_YPos,
			m_Width, m_Height,
			parent, nullptr, m_hInst, nullptr);

		// return on error creating the window
		if (m_hWnd == nullptr)
		{
			Window::Error(true, "m_hWnd == NULL");
			return false;
		}

		return true;
	}

	HWND Window::GethWnd()
	{
		return m_hWnd;
	}

	HINSTANCE Window::GethInst()
	{
		return m_hInst;
	}

	DWORD Window::GetScreenWidth()
	{
		return m_Width;
	}

	DWORD Window::GetScreenHeight()
	{
		return m_Height;
	}

	DWORD Window::GetStyle()
	{
		return m_Style;
	}

	DWORD Window::GetXPos()
	{
		return m_XPos;
	}

	DWORD Window::GetYPos()
	{
		return m_YPos;
	}

	void Window::SetScreenWidth(DWORD w)
	{
		m_Width = w;
	}

	void Window::SetScreenHeight(DWORD h)
	{
		m_Height = h;
	}

	void Window::SetStyle(DWORD s)
	{
		m_Style = s;
	}

	void Window::SetXPos(DWORD x)
	{
		m_XPos = x;
	}

	void Window::SetYPos(DWORD y)
	{
		m_YPos = y;
	}

	bool Window::Error(bool fatal_, const char* text_, ...)
	{
		char CaptionText[16];
		char ErrorText[2048];
		va_list valist;

		// Build the message box caption based on fatal flag
		if (fatal_ == false)
		{
			strcpy_s(CaptionText, 16, "Error");
		}
		else
		{
			strcpy_s(CaptionText, 16, "Fatal Error");
		}

		// Build variable text buffer
		va_start(valist, text_);
		vsprintf_s(ErrorText, 2048, text_, valist);
		va_end(valist);

		// Display the message box
		MessageBox(nullptr, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION);

		// Post a quit message if error was fatal
		if (fatal_ == true)
		{
			//PostQuitMessage(0);
			exit(-1);
		}

		return true;
	}

	bool Window::Move(long XPos, long YPos)
	{

		return true;
	}

	bool Window::Resize(long Width, long Height)
	{

		return true;
	}

	// The message procedure
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			GameCore::Instance().CleanUp();
			//PostQuitMessage(0);  // Tell Windows to close application
			DestroyWindow(hWnd);
			break;

			// Handle every other message
		default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		return 0;
	}

	bool Window::ShowMouse(bool show)
	{
		ShowCursor(show);
		return true;
	}

	void Window::Close()
	{
		UnregisterClass(m_Class, m_hInst);
	}

	WNDCLASSEX& Window::GetWndClassEX()
	{
		return m_wcex;
	}

} // namespace GameEgine
