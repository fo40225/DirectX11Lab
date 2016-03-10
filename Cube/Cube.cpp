// Cube.cpp : 定義應用程式的進入點。
//

#include "stdafx.h"
#include "Cube.h"

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HRESULT hr = S_OK;

	// Begin initialization.

	// Instantiate the window manager class.
	std::shared_ptr<MainClass> winMain = std::shared_ptr<MainClass>(new MainClass());
	// Create a window.
	hr = winMain->CreateDesktopWindow();

	if (SUCCEEDED(hr))
	{
		// Instantiate the device manager class.
		std::shared_ptr<DeviceResources> deviceResources = std::shared_ptr<DeviceResources>(new DeviceResources());
		// Create device resources.
		deviceResources->CreateDeviceResources();

		// Instantiate the renderer.
		std::shared_ptr<Renderer> renderer = std::shared_ptr<Renderer>(new Renderer(deviceResources));
		renderer->CreateDeviceDependentResources();

		// We have a window, so initialize window size-dependent resources.
		deviceResources->CreateWindowResources(winMain->GetWindowHandle());
		renderer->CreateWindowSizeDependentResources();

		// Run the program.
		hr = winMain->Run(deviceResources, renderer);
	}

	return hr;
}