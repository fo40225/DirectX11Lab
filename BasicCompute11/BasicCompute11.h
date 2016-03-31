#pragma once

const UINT NUM_ELEMENTS = 1024;

HRESULT CreateComputeDevice(
	_Outptr_ ID3D11Device** ppDeviceOut,
	_Outptr_ ID3D11DeviceContext** ppContextOut
	);
HRESULT CreateComputeShader(
	_In_z_ LPCWSTR pSrcFile,
	_In_z_ LPCSTR pFunctionName,
	_In_ ID3D11Device* pDevice,
	_Outptr_ ID3D11ComputeShader** ppShaderOut
	);
HRESULT CreateStructuredBuffer(
	_In_ ID3D11Device* pDevice,
	_In_ UINT uElementSize,
	_In_ UINT uCount,
	_In_reads_(uElementSize*uCount) void* pInitData,
	_Outptr_ ID3D11Buffer** ppBufOut
	);
HRESULT CreateBufferSRV(
	ID3D11Device* pDevice,
	_In_ ID3D11Buffer* pBuffer,
	_Outptr_ ID3D11ShaderResourceView** ppSRVOut
	);
HRESULT CreateBufferUAV(
	_In_ ID3D11Device* pDevice,
	_In_ ID3D11Buffer* pBuffer,
	_Outptr_ ID3D11UnorderedAccessView** pUAVOut
	);
void RunComputeShader(
	_In_ ID3D11DeviceContext* pd3dImmediateContext,
	_In_ ID3D11ComputeShader* pComputeShader,
	_In_ UINT nNumViews, _In_reads_(nNumViews) ID3D11ShaderResourceView** pShaderResourceViews,
	_In_opt_ ID3D11Buffer* pCBCS, _In_reads_opt_(dwNumDataBytes) void* pCSData, _In_ DWORD dwNumDataBytes,
	_In_ ID3D11UnorderedAccessView* pUnorderedAccessView,
	_In_ UINT X, _In_ UINT Y, _In_ UINT Z
	);
ID3D11Buffer* CreateAndCopyToDebugBuf(
	_In_ ID3D11Device* pDevice,
	_In_ ID3D11DeviceContext* pd3dImmediateContext,
	_In_ ID3D11Buffer* pBuffer
	);

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3D11Device*               g_pDevice = nullptr;
ID3D11DeviceContext*        g_pContext = nullptr;
ID3D11ComputeShader*        g_pCS = nullptr;

ID3D11Buffer*               g_pBuf0 = nullptr;
ID3D11Buffer*               g_pBuf1 = nullptr;
ID3D11Buffer*               g_pBufResult = nullptr;

ID3D11ShaderResourceView*   g_pBuf0SRV = nullptr;
ID3D11ShaderResourceView*   g_pBuf1SRV = nullptr;
ID3D11UnorderedAccessView*  g_pBufResultUAV = nullptr;

struct BufType
{
	int i;
};
BufType g_vBuf0[NUM_ELEMENTS];
BufType g_vBuf1[NUM_ELEMENTS];