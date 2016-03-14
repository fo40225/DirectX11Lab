#pragma once

HINSTANCE g_hInst = nullptr;
HWND      g_hWnd  = nullptr;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);