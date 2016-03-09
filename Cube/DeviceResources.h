#pragma once

class DeviceResources
{
public:
	void Present();
	HRESULT DeviceResources::CreateDeviceResources();
	HRESULT CreateWindowResources(HWND hWnd);
	HRESULT DeviceResources::ConfigureBackBuffer();
private:
	D3D_FEATURE_LEVEL m_featureLevel;

	Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pd3dDeviceContext; // immediate context

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pDXGISwapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;
	D3D11_TEXTURE2D_DESC m_bbDesc;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_pDepthStencilView;
	D3D11_VIEWPORT m_viewport;
};