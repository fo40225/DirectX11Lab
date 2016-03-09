#include "stdafx.h"
#include "MainClass.h"

MainClass::MainClass()
{
	m_windowClassName = L"Direct3DWindowClass";
	m_hInstance = NULL;
}

HRESULT MainClass::CreateDesktopWindow()
{
	// Window resources are dealt with here.
	if (m_hInstance == NULL)
		m_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	// Register the windows class
	WNDCLASS wndClass = {};
	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = MainClass::StaticWindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = m_windowClassName.c_str();

	if (!RegisterClass(&wndClass))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(dwError);
	}

	m_rc = {};
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;

	// No menu in this example.
	m_hMenu = NULL;

	// This example uses a non-resizable 640 by 480 viewport for simplicity.
	int nDefaultWidth = 640;
	int nDefaultHeight = 480;
	SetRect(&m_rc, 0, 0, nDefaultWidth, nDefaultHeight);
	AdjustWindowRect(
		&m_rc,
		WS_OVERLAPPEDWINDOW,
		(m_hMenu != NULL) ? true : false
		);

	// Create the window for our viewport.
	m_hWnd = CreateWindow(
		m_windowClassName.c_str(),
		L"Cube11",
		WS_OVERLAPPEDWINDOW,
		x, y,
		(m_rc.right - m_rc.left), (m_rc.bottom - m_rc.top),
		0,
		m_hMenu,
		m_hInstance,
		0
		);

	if (m_hWnd == NULL)
	{
		DWORD dwError = GetLastError();
		return HRESULT_FROM_WIN32(dwError);
	}

	return S_OK;
}

LRESULT CALLBACK MainClass::StaticWindowProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		HMENU hMenu;
		hMenu = GetMenu(hWnd);
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(hWnd);
		UnregisterClass(
			m_windowClassName.c_str(),
			m_hInstance
			);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HRESULT MainClass::Run(
	std::shared_ptr<DeviceResources> deviceResources,
	std::shared_ptr<Renderer> renderer
	)
{
	HRESULT hr = S_OK;

	if (!IsWindowVisible(m_hWnd))
		ShowWindow(m_hWnd, SW_SHOW);

	bool bGotMsg;
	MSG  msg = {};
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Process window events.
		// Use PeekMessage() so we can use idle time to render the scene.
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update the scene.
			renderer->Update();

			// Render frames during idle time (when no messages are waiting).
			renderer->Render();

			// Present the frame to the screen.
			deviceResources->Present();
		}
	}

	return hr;
}

HWND MainClass::GetWindowHandle() { return m_hWnd; }
