#pragma once
#include "DeviceResources.h"

class Renderer
{
public:
	void Update();
	void Render();
	Renderer(std::shared_ptr<DeviceResources> deviceResources);
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	unsigned int  m_indexCount;
	unsigned int  m_frameCount;
	HRESULT CreateShaders();
	HRESULT CreateCube();
	void CreateViewAndPerspective();

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

	typedef struct _constantBufferStruct {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	} ConstantBufferStruct;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;

	typedef struct _vertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	} VertexPositionColor;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

	ConstantBufferStruct m_constantBufferData;
};