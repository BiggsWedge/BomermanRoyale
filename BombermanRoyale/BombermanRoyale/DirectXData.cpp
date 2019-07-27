#include "DirectXData.h"
#include "Utilities.h"

DirectXData* g_d3dData = nullptr;


bool DirectXData::Initialize()
{
	GW::GRAPHICS::CreateGDirectX11Surface(g_pWindow, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, &d3dSurface);

	d3dSurface->GetDevice((void**)&d3dDevice);
	d3dSurface->GetSwapchain((void**)&d3dSwapChain);
	d3dSurface->GetContext((void**)&d3dContext);

	DXGI_SWAP_CHAIN_DESC d3dSwapChainDesc;
	d3dSwapChain->GetDesc(&d3dSwapChainDesc);

#pragma region Viewport Creation

	ZeroMemory(&d3dViewport, sizeof(d3dViewport));

	d3dViewport.Height = d3dSwapChainDesc.BufferDesc.Height;
	d3dViewport.Width = d3dSwapChainDesc.BufferDesc.Width;
	d3dViewport.TopLeftX = d3dViewport.TopLeftY = 0;
	d3dViewport.MaxDepth = 1;
	d3dViewport.MinDepth = 0;

#pragma endregion

#pragma region Rasterizer State Creation

	D3D11_RASTERIZER_DESC d3dRasterDesc;
	ZeroMemory(&d3dRasterDesc, sizeof(d3dRasterDesc));
	d3dRasterDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterDesc.FrontCounterClockwise = true;
	d3dRasterDesc.DepthBias = false;
	d3dRasterDesc.DepthBiasClamp = 1.0f;
	d3dRasterDesc.SlopeScaledDepthBias = 0;
	d3dRasterDesc.DepthClipEnable = true;
	d3dRasterDesc.ScissorEnable = false;
	d3dRasterDesc.MultisampleEnable = true;
	d3dRasterDesc.AntialiasedLineEnable = true;

	if (FAILED(d3dDevice->CreateRasterizerState(&d3dRasterDesc, &d3dRasterizerState)))
	{
		//Log Failure
		return false;
	}

#pragma endregion

#pragma region Shader Creations

#pragma region Vertex

	if (FAILED(d3dDevice->CreateVertexShader(BasicVertex, sizeof(BasicVertex), nullptr, &d3dVertexShader[VERTEX_SHADER::BASIC])))
	{
		//log failure
		return false;
	}

#pragma endregion

#pragma region Pixel

	if (FAILED(d3dDevice->CreatePixelShader(BasicPixel, sizeof(BasicPixel), nullptr, &d3dPixelShader[PIXEL_SHADER::BASIC])))
	{
		//log failure
		return false;
	}

#pragma endregion

#pragma region Geometry

#pragma endregion

#pragma endregion

#pragma region Input Layout Creation

	std::vector<D3D11_INPUT_ELEMENT_DESC> d3dBasicShaderDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JOINTS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(d3dBasicShaderDesc.data(), d3dBasicShaderDesc.size(), BasicVertex, sizeof(BasicVertex), &d3dInputLayout[INPUT_LAYOUT::BASIC])))
	{
		//log failure
		return false;
	}

#pragma endregion

#pragma region Sampler State Creation

	D3D11_SAMPLER_DESC d3dSamStateDesc;
	ZeroMemory(&d3dSamStateDesc, sizeof(d3dSamStateDesc));
	d3dSamStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamStateDesc.AddressU = d3dSamStateDesc.AddressV = d3dSamStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamStateDesc.MinLOD = 0;
	d3dSamStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(d3dDevice->CreateSamplerState(&d3dSamStateDesc, &d3dSamplerState)))
	{
		//log failure
		return false;
	}

#pragma endregion

#pragma region Camera Creation

	camPos = { 0.0f, 15.0f, -15.0f };
	DirectX::XMFLOAT3 at = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };

	camMat = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&camPos), DirectX::XMLoadFloat3(&at), DirectX::XMLoadFloat3(&up));

	viewMat = camMat;// DirectX::XMMatrixInverse(nullptr, camMat);
	projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(d3dSwapChainDesc.BufferDesc.Width) / static_cast<float>(d3dSwapChainDesc.BufferDesc.Height), 0.1f, 50.0f);

#pragma endregion

#pragma region Constant Buffer Creation

	D3D11_BUFFER_DESC BasicVConstBuffDesc;
	ZeroMemory(&BasicVConstBuffDesc, sizeof(BasicVConstBuffDesc));

	BasicVConstBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BasicVConstBuffDesc.ByteWidth = sizeof(TBasicVertexConstBuff);
	BasicVConstBuffDesc.CPUAccessFlags = 0;
	BasicVConstBuffDesc.MiscFlags = 0;
	BasicVConstBuffDesc.StructureByteStride = 0;
	BasicVConstBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::V_BASIC]);

	D3D11_BUFFER_DESC BasicPConstBuffDesc;
	ZeroMemory(&BasicPConstBuffDesc, sizeof(BasicPConstBuffDesc));

	BasicPConstBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BasicPConstBuffDesc.ByteWidth = sizeof(TBasicPixelConstBuff);
	BasicPConstBuffDesc.CPUAccessFlags = 0;
	BasicPConstBuffDesc.MiscFlags = 0;
	BasicPConstBuffDesc.StructureByteStride = 0;
	BasicPConstBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	d3dDevice->CreateBuffer(&BasicPConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::P_BASIC]);

#pragma endregion

	return true;
}

DirectXData::DirectXData()
{

}

DirectXData::~DirectXData()
{
	Cleanup();
}

void DirectXData::Cleanup()
{
	ID3D11Debug* _debugger;
	d3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&_debugger));


	SAFE_RELEASE(d3dSamplerState);
	SAFE_RELEASE(d3dRasterizerState);
	SAFE_RELEASE(d3dDepthStencilView);
	SAFE_RELEASE(d3dRenderTargetView);

	for (int i = 0; i < INPUT_LAYOUT::COUNT; ++i)
		SAFE_RELEASE(d3dInputLayout[i]);
	for (int i = 0; i < CONSTANT_BUFFER::COUNT; ++i)
		SAFE_RELEASE(d3dConstBuffers[i]);
	for (int i = 0; i < GEOMETRY_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dGeometryShader[i]);
	for (int i = 0; i < PIXEL_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dPixelShader[i]);
	for (int i = 0; i < VERTEX_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dVertexShader[i]);
	for (int i = 0; i < DIFFUSE_TEXTURES::COUNT; ++i)
		SAFE_RELEASE(d3dDiffuseTextures[i]);

	SAFE_RELEASE(d3dSwapChain);
	SAFE_RELEASE(d3dContext);
	SAFE_RELEASE(d3dDevice);

	if (d3dSurface)
	{
		d3dSurface->DecrementCount();
		d3dSurface = nullptr;
	}

	SAFE_RELEASE(_debugger);
}
