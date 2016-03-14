// DirectX11Lab.cpp : 定義應用程式的進入點。
//

#include "stdafx.h"
#include "DirectX11Lab.h"

int APIENTRY wWinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPWSTR    lpCmdLine,
	_In_     int       nCmdShow
	)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	//
	// Main message loop
	//
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	//
	// Register class
	//
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	if (!RegisterClassEx(&wcex))
		return FALSE;

	//
	// Create window
	//
	g_hInst = hInstance; // Store instance handle in our global variable
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 10 Tutorial 0: Setting Up Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
		return FALSE;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}