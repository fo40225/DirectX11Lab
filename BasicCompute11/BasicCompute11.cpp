// BasicCompute11.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "BasicCompute11.h"

int main()
{
	HRESULT hr = S_OK;

	printf("Creating device...");
	hr = CreateComputeDevice(&g_pDevice, &g_pContext);
	if (FAILED(hr))
		return hr;
	printf("done\n");

	printf("Creating Compute Shader...");
	hr = CreateComputeShader(L"BasicCompute11.hlsl", "CSMain", g_pDevice, &g_pCS);
	if (FAILED(hr))
		return hr;
	printf("done\n");

	printf("Creating buffers and filling them with initial data...");
	for (int i = 0; i < NUM_ELEMENTS; ++i)
	{
		g_vBuf0[i].i = i;
		g_vBuf1[i].i = i;
	}

	CreateStructuredBuffer(g_pDevice, sizeof(BufType), NUM_ELEMENTS, &g_vBuf0[0], &g_pBuf0);
	CreateStructuredBuffer(g_pDevice, sizeof(BufType), NUM_ELEMENTS, &g_vBuf1[0], &g_pBuf1);
	CreateStructuredBuffer(g_pDevice, sizeof(BufType), NUM_ELEMENTS, nullptr, &g_pBufResult);
	printf("done\n");

	printf("Creating buffer views...");
	CreateBufferSRV(g_pDevice, g_pBuf0, &g_pBuf0SRV);
	CreateBufferSRV(g_pDevice, g_pBuf1, &g_pBuf1SRV);
	CreateBufferUAV(g_pDevice, g_pBufResult, &g_pBufResultUAV);
	printf("done\n");

	printf("Running Compute Shader...");
	ID3D11ShaderResourceView* aRViews[2] = { g_pBuf0SRV, g_pBuf1SRV };
	RunComputeShader(g_pContext, g_pCS, 2, aRViews, nullptr, nullptr, 0, g_pBufResultUAV, NUM_ELEMENTS, 1, 1);
	printf("done\n");

	// Read back the result from GPU, verify its correctness against result computed by CPU
	ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(g_pDevice, g_pContext, g_pBufResult);
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	BufType *p;
	g_pContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

	// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
	// This is also a common trick to debug CS programs.
	p = (BufType*)MappedResource.pData;

	// Verify that if Compute Shader has done right
	printf("Verifying against CPU result...");
	bool bSuccess = true;
	for (int i = 0; i < NUM_ELEMENTS; ++i)
		if (p[i].i != g_vBuf0[i].i + g_vBuf1[i].i)
		{
			printf("failure\n");
			bSuccess = false;

			break;
		}

	if (bSuccess)
		printf("succeeded\n");

	return hr;
}

//--------------------------------------------------------------------------------------
// Create the D3D device and device context suitable for running Compute Shaders(CS)
//--------------------------------------------------------------------------------------
HRESULT CreateComputeDevice(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	*ppDeviceOut = nullptr;
	*ppContextOut = nullptr;

	HRESULT hr = S_OK;

	UINT uCreationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	uCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL flvl[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	D3D_FEATURE_LEVEL flOut;

	hr = D3D11CreateDevice(
		nullptr,                  // Use default graphics card
		D3D_DRIVER_TYPE_HARDWARE, // Try to create a hardware accelerated device
		nullptr,                  // Do not use external software rasterizer module
		uCreationFlags,           // Device creation flags
		flvl,                     // The order of feature levels attempt to create
		_countof(flvl),           // The number of elements in pFeatureLevels.
		D3D11_SDK_VERSION,        // SDK version
		ppDeviceOut,              // Device out
		&flOut,                   // Actual feature level created
		ppContextOut              // Context out
		);

	bool bNeedRefDevice = false;
	if (SUCCEEDED(hr))
	{
		// A hardware accelerated device has been created, so check for Compute Shader support

		// If we have a device >= D3D_FEATURE_LEVEL_11_0 created, full CS5.0 support is guaranteed, no need for further checks
		if (flOut < D3D_FEATURE_LEVEL_11_0)
		{
			// Otherwise, we need further check whether this device support CS4.x (Compute on 10)
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
			(*ppDeviceOut)->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
			if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
			{
				bNeedRefDevice = true;
				printf("\nNo hardware Compute Shader capable device found, trying to create software (WARP) device.\n");
			}
		}
	}

	if (FAILED(hr) || bNeedRefDevice)
	{
		// Either because of failure on creating a hardware device or hardware lacking CS capability, we create a WARP device here

		if (*ppDeviceOut)
		{
			(*ppDeviceOut)->Release();
		}

		if (*ppContextOut)
		{
			(*ppContextOut)->Release();
		}

		hr = D3D11CreateDevice(
			nullptr,              // Use default graphics card
			D3D_DRIVER_TYPE_WARP, // Try to create a software implemented device
			nullptr,              // Do not use external software rasterizer module
			uCreationFlags,       // Device creation flags
			flvl,                 // The order of feature levels attempt to create
			_countof(flvl),       // The number of elements in pFeatureLevels.
			D3D11_SDK_VERSION,    // SDK version
			ppDeviceOut,          // Device out
			&flOut,               // Actual feature level created
			ppContextOut          // Context out
			);
	}

	return hr;
}

//--------------------------------------------------------------------------------------
// Compile and create the CS
//--------------------------------------------------------------------------------------
HRESULT CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName, ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut)
{
	HRESULT hr = S_OK;

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows
	// the shaders to be optimized and to run exactly the way they will run in
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		pSrcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pFunctionName,
		pProfile,
		dwShaderFlags,
		0,
		&pBlob,
		&pErrorBlob
		);

	if (FAILED(hr))
	{
		if (pErrorBlob)
			printf((char*)pErrorBlob->GetBufferPointer());

		return hr;
	}

	hr = pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, ppShaderOut);

	return hr;
}

//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, ppBufOut);
}

//--------------------------------------------------------------------------------------
// Create Shader Resource View for Structured Buffers
//--------------------------------------------------------------------------------------
HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	// This is a Structured Buffer
	// Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured Buffers
//--------------------------------------------------------------------------------------
HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	// This is a Structured Buffer
	// Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

//--------------------------------------------------------------------------------------
// Run CS
//--------------------------------------------------------------------------------------
void RunComputeShader(
	ID3D11DeviceContext* pd3dImmediateContext,
	ID3D11ComputeShader* pComputeShader,
	UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews,
	ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
	ID3D11UnorderedAccessView* pUnorderedAccessView,
	UINT X, UINT Y, UINT Z
	)
{
	pd3dImmediateContext->CSSetShader(pComputeShader, nullptr, 0);
	pd3dImmediateContext->CSSetShaderResources(0, nNumViews, pShaderResourceViews);
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &pUnorderedAccessView, nullptr);

	pd3dImmediateContext->Dispatch(X, Y, Z);

	pd3dImmediateContext->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, ppUAViewnullptr, nullptr);

	ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
	pd3dImmediateContext->CSSetShaderResources(0, 2, ppSRVnullptr);

	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCBnullptr);
}

//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//--------------------------------------------------------------------------------------
ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
	{
		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}