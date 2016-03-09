#pragma once
#include "DeviceResources.h"
#include "Renderer.h"

class MainClass
{
public:
	MainClass();
	HRESULT CreateDesktopWindow();
	static LRESULT CALLBACK StaticWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
		);
	HRESULT Run(
		std::shared_ptr<DeviceResources> deviceResources,
		std::shared_ptr<Renderer> renderer
		);
	HWND GetWindowHandle();
private:
	HMENU     m_hMenu;
	RECT      m_rc;
	HWND      m_hWnd;
};

// These are STATIC because this sample only creates one window.
// If your app can have multiple windows, MAKE SURE this is dealt with
// differently.
static HINSTANCE m_hInstance;
static std::wstring m_windowClassName;