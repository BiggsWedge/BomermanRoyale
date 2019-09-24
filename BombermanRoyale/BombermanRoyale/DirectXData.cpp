#include "DirectXData.h"
#include "Utilities.h"

DirectXData* g_d3dData = nullptr;


bool DirectXData::Initialize()
{

	GW::GRAPHICS::CreateGDirectX11Surface(g_pWindow, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, &d3dSurface);

	GW::GReturn gresult;

	gresult = d3dSurface->GetDevice((void**)&d3dDevice);
	if (G_FAIL(gresult))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to get the Device");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully retrieved the device");

	gresult = d3dSurface->GetSwapchain((void**)&d3dSwapChain);
	if (G_FAIL(gresult))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to get the swapchain");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully retrieved the Swapchain");

	gresult = d3dSurface->GetContext((void**)&d3dContext);
	if (G_FAIL(gresult))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to get the context");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully retrieved the context");

	DXGI_SWAP_CHAIN_DESC d3dSwapChainDesc;
	if (FAILED(d3dSwapChain->GetDesc(&d3dSwapChainDesc)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to get the swap chain description");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully retrieved the swap chain description");


#pragma region Viewport Creation

	ZeroMemory(&d3dViewport, sizeof(d3dViewport));

	d3dViewport.Height = (FLOAT)d3dSwapChainDesc.BufferDesc.Height;
	d3dViewport.Width = (FLOAT)d3dSwapChainDesc.BufferDesc.Width;
	d3dViewport.TopLeftX = d3dViewport.TopLeftY = 0;
	d3dViewport.MaxDepth = 1;
	d3dViewport.MinDepth = 0;

	windowWidthHeight.x = d3dSwapChainDesc.BufferDesc.Width;
	windowWidthHeight.y = d3dSwapChainDesc.BufferDesc.Height;

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

	if (FAILED(d3dDevice->CreateRasterizerState(&d3dRasterDesc, &d3dRasterizerState[RASTERIZER_STATE::DEFAULT])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the rasterizer state");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created rasterizer state");

	d3dRasterDesc.FrontCounterClockwise = false;

	if (FAILED(d3dDevice->CreateRasterizerState(&d3dRasterDesc, &d3dRasterizerState[RASTERIZER_STATE::SKYBOX])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the rasterizer state");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created rasterizer state");



#pragma endregion

#pragma region CollisionMatrix

	for (int i = 0; i < COLLISION_LAYERS::COUNT; ++i)
	{
		for (int j = 0; j < COLLISION_LAYERS::COUNT; ++j)
			collisionMatrix[i][j] = false;
	}
	for (int i = 0; i < COLLISION_LAYERS::COUNT; ++i)
	{
		switch (i)
		{
		case COLLISION_LAYERS::PLAYER:
		{
			for (int j = 0; j < COLLISION_LAYERS::COUNT; ++j)
			{
				collisionMatrix[i][j] = (j == COLLISION_LAYERS::FLOOR) ? false : true;
				collisionMatrix[j][i] = (j == COLLISION_LAYERS::FLOOR) ? false : true;
			}
			break;
		}
		case COLLISION_LAYERS::BOMB:
		{
			for (int j = 0; j < COLLISION_LAYERS::COUNT; ++j)
			{
				collisionMatrix[i][j] = (j == COLLISION_LAYERS::PLAYER || j == COLLISION_LAYERS::WALL || j == COLLISION_LAYERS::DESTROYABLE || j == COLLISION_LAYERS::BOMB) ? true : false;
				collisionMatrix[j][i] = (j == COLLISION_LAYERS::PLAYER || j == COLLISION_LAYERS::WALL || j == COLLISION_LAYERS::DESTROYABLE || j == COLLISION_LAYERS::BOMB) ? true : false;

				//collisionMatrix[i][j] = (j == COLLISION_LAYERS::WALL) ? true : false;
				//collisionMatrix[j][i] = (j == COLLISION_LAYERS::WALL) ? true : false;

				//collisionMatrix[i][j] = (j == COLLISION_LAYERS::DESTROYABLE) ? true : false;
				//collisionMatrix[j][i] = (j == COLLISION_LAYERS::DESTROYABLE) ? true : false;

				//collisionMatrix[i][j] = (j == COLLISION_LAYERS::BOMB) ? true : false;
				//collisionMatrix[j][i] = (j == COLLISION_LAYERS::BOMB) ? true : false;
			}
			break;
		}
		case COLLISION_LAYERS::EXPLOSION:
		{
			for (int j = 0; j < COLLISION_LAYERS::COUNT; ++j)
			{
				collisionMatrix[i][j] = (j == COLLISION_LAYERS::PLAYER || j == COLLISION_LAYERS::DESTROYABLE || j == COLLISION_LAYERS::BOMB) ? true : false;
				collisionMatrix[j][i] = (j == COLLISION_LAYERS::PLAYER || j == COLLISION_LAYERS::DESTROYABLE || j == COLLISION_LAYERS::BOMB) ? true : false;
			}
			break;
		}
		default:
		{
			break;
		}
		}
	}

#pragma endregion



#pragma region Shader Creations

#pragma region Vertex

	if (FAILED(d3dDevice->CreateVertexShader(BasicVertex, sizeof(BasicVertex), nullptr, &d3dVertexShader[VERTEX_SHADER::BASIC])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the basic vertex shader");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the basic vertex shader");

	if (FAILED(d3dDevice->CreateVertexShader(LineVertex, sizeof(LineVertex), nullptr, &d3dVertexShader[VERTEX_SHADER::LINE])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the line vertex shader");

	if (FAILED(d3dDevice->CreateVertexShader(AnimVertex, sizeof(AnimVertex), nullptr, &d3dVertexShader[VERTEX_SHADER::ANIM])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the animation vertex shader");


	/*
	if (FAILED(d3dDevice->CreateVertexShader(SkyVertex, sizeof(SkyVertex), nullptr, &d3dVertexShader[VERTEX_SHADER::SKY])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the skybox vertex shader");
	*/

	if (FAILED(d3dDevice->CreateVertexShader(BombShader, sizeof(BombShader), nullptr, &d3dVertexShader[VERTEX_SHADER::BOMB])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the animation vertex shader");

#pragma endregion

#pragma region Pixel

	if (FAILED(d3dDevice->CreatePixelShader(BasicPixel, sizeof(BasicPixel), nullptr, &d3dPixelShader[PIXEL_SHADER::BASIC])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the basic pixel shader");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the basic pixel shader");

	if (FAILED(d3dDevice->CreatePixelShader(LinePixel, sizeof(LinePixel), nullptr, &d3dPixelShader[PIXEL_SHADER::LINE])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the line pixel shader");

	if (FAILED(d3dDevice->CreatePixelShader(AnimPixel, sizeof(AnimPixel), nullptr, &d3dPixelShader[PIXEL_SHADER::ANIM])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the animation pixel shader");

	/*
	if (FAILED(d3dDevice->CreatePixelShader(SkyPixel, sizeof(SkyPixel), nullptr, &d3dPixelShader[PIXEL_SHADER::SKY])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the skybox pixel shader");
	*/

	if (FAILED(d3dDevice->CreatePixelShader(BombPShader, sizeof(BombPShader), nullptr, &d3dPixelShader[PIXEL_SHADER::BOMB])))

	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the bomb pixel shader");

#pragma endregion

#pragma region Geometry
	if (FAILED(d3dDevice->CreateGeometryShader(GeometryShader, sizeof(GeometryShader), nullptr, &d3dGeometryShader[GEOMETRY_SHADER::PARTICLE])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the geometry shader");
#pragma endregion

#pragma region Compute
	if (FAILED(d3dDevice->CreateComputeShader(ComputeShader, sizeof(ComputeShader), nullptr, &d3dComputeShader[COMPUTE_SHADER::PARTICLE])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the compute shader");
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
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the basic vertex input layout");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the basic vertex input layout");

	std::vector<D3D11_INPUT_ELEMENT_DESC> d3dLineShaderDesc = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(d3dDevice->CreateInputLayout(d3dLineShaderDesc.data(), d3dLineShaderDesc.size(), LineVertex, sizeof(LineVertex), &d3dInputLayout[INPUT_LAYOUT::LINE])))
	{
		//log failure
		return false;
	}

	/*
	std::vector<D3D11_INPUT_ELEMENT_DESC> d3dSkyShaderDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(d3dDevice->CreateInputLayout(d3dSkyShaderDesc.data(), d3dSkyShaderDesc.size(), SkyVertex, sizeof(SkyVertex), &d3dInputLayout[INPUT_LAYOUT::SKY])))
	{
		//log failure
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the skybox input layout");

	*/
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
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the sampler state");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the sampler state");


	// Create the skybox sample state
	D3D11_SAMPLER_DESC skyboxSampDesc = {};
	skyboxSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	skyboxSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	skyboxSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	skyboxSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	skyboxSampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	skyboxSampDesc.MinLOD = 0;
	skyboxSampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(d3dDevice->CreateSamplerState(&skyboxSampDesc, &JungleSampler)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the skybox sampler state");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the skybox sampler state");

	//CreateDDSTextureFromFile(d3dDevice, L"JungleCubeMap.dds", (ID3D11Resource**)&Jungle, &JungleSRV);

#pragma endregion

#pragma region Camera Creation

	debugCamPos = { 0.0f, 25.0f, -22.0f };
	camPos = { 0.0f, 45.0f, -25.0f };
	DirectX::XMFLOAT3 at = { 0.0f, 0.0f, 3.0f };
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };

	camMat = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&camPos), DirectX::XMLoadFloat3(&at), DirectX::XMLoadFloat3(&up));

	debugCamMat = DirectX::XMMatrixTranslation(debugCamPos.x, debugCamPos.y, debugCamPos.z) * DirectX::XMMatrixIdentity();
	debugCamMat = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(30)) * debugCamMat;

	viewMat = camMat;// DirectX::XMMatrixInverse(nullptr, camMat);
	viewMatSky = viewMat;
	projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), static_cast<float>(d3dSwapChainDesc.BufferDesc.Width) / static_cast<float>(d3dSwapChainDesc.BufferDesc.Height), 0.1f, 100.0f);

#pragma endregion

#pragma region Blend State Creation

	D3D11_BLEND_DESC blendDesc;

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	d3dDevice->CreateBlendState(&blendDesc, &d3dBlendState[BLEND_STATE::UI]);

	blendDesc.RenderTarget[0].BlendEnable = false;

	d3dDevice->CreateBlendState(&blendDesc, &d3dBlendState[BLEND_STATE::DEFAULT]);

#pragma endregion

#pragma region Depth Stencil State Creation

	D3D11_DEPTH_STENCIL_DESC dssDesc;

	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dssDesc.StencilEnable = true;
	dssDesc.StencilReadMask = 0xFF;
	dssDesc.StencilWriteMask = 0xFF;

	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	d3dDevice->CreateDepthStencilState(&dssDesc, &d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT]);

	dssDesc.DepthEnable = false;

	d3dDevice->CreateDepthStencilState(&dssDesc, &d3dDepthStencilState[DEPTH_STENCIL_STATE::TWO_D]);

#pragma endregion

#pragma region Constant Buffer Creation

	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA bdsd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TLineVertex) * get_line_vert_capacity();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	bdsd.pSysMem = get_line_verts();

	d3dDevice->CreateBuffer(&bd, NULL, &d3dConstBuffers[CONSTANT_BUFFER::V_LINE]);

	D3D11_BUFFER_DESC BasicVConstBuffDesc;
	ZeroMemory(&BasicVConstBuffDesc, sizeof(BasicVConstBuffDesc));

	BasicVConstBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BasicVConstBuffDesc.ByteWidth = sizeof(TBasicVertexConstBuff);
	BasicVConstBuffDesc.CPUAccessFlags = 0;
	BasicVConstBuffDesc.MiscFlags = 0;
	BasicVConstBuffDesc.StructureByteStride = 0;
	BasicVConstBuffDesc.Usage = D3D11_USAGE_DEFAULT;


	if (FAILED(d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::V_BASIC])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the basic vertex constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the basic vertex constant buffer");

	BasicVConstBuffDesc.ByteWidth = sizeof(LightBuffer);

	if (FAILED(d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::LIGHTS])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the light constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the light constant buffer");

	BasicVConstBuffDesc.ByteWidth = sizeof(MatBuffer);

	if (FAILED(d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::MATERIAL])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the material constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the material constant buffer");

	BasicVConstBuffDesc.ByteWidth = sizeof(MVP_t);

	if (FAILED(d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::MVP_t])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the MVP_t constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the MVP_t constant buffer");

	BasicVConstBuffDesc.ByteWidth = sizeof(jointCB);

	if (FAILED(d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::JOINTS])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the joints constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the joints constant buffer");

	BasicVConstBuffDesc.ByteWidth = sizeof(bombconstbuffer);

	if (FAILED(d3dDevice->CreateBuffer(&BasicVConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the bomb vertex constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the bomb vertex constant buffer");

	D3D11_BUFFER_DESC BasicPConstBuffDesc;
	ZeroMemory(&BasicPConstBuffDesc, sizeof(BasicPConstBuffDesc));

	BasicPConstBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BasicPConstBuffDesc.ByteWidth = sizeof(TBasicPixelConstBuff);
	BasicPConstBuffDesc.CPUAccessFlags = 0;
	BasicPConstBuffDesc.MiscFlags = 0;
	BasicPConstBuffDesc.StructureByteStride = 0;
	BasicPConstBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(d3dDevice->CreateBuffer(&BasicPConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::P_BASIC])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the basic pixel constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the basic pixel constant buffer");

	BasicPConstBuffDesc.ByteWidth = sizeof(bombpixelbuffer);

	if (FAILED(d3dDevice->CreateBuffer(&BasicPConstBuffDesc, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::BOMB_P_CONST])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the bomb pixel constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the bomb pixel constant buffer");

	d3dSpriteBatch = new DirectX::SpriteBatch(d3dContext);
	d3dSpriteFont = new DirectX::SpriteFont(d3dDevice, L".//Assets//winkDeep.spritefont");



	//Jungle Constant Buffer
	D3D11_BUFFER_DESC JungleBuffer = {};
	JungleBuffer.Usage = D3D11_USAGE_DYNAMIC;
	JungleBuffer.ByteWidth = sizeof(TBasicVertexConstBuff);
	JungleBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	JungleBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(d3dDevice->CreateBuffer(&JungleBuffer, nullptr, &d3dConstBuffers[CONSTANT_BUFFER::SKY])))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create the Skybox constant buffer");
		return false;
	}
	else
		g_pLogger->LogCatergorized("SUCCESS", "Successfully created the Skybox constant buffer");

#pragma endregion


#pragma region Index Buffer Creation

	//Sky Index Buffer

#pragma endregion



	return true;
}

DirectXData::DirectXData()
{

}

DirectXData::~DirectXData()
{
}

void DirectXData::Cleanup()
{
#pragma region Release Skybox Stuff

	SAFE_RELEASE(JungleSampler);
	SAFE_RELEASE(Jungle);
	SAFE_RELEASE(JungleSRV);
	SAFE_RELEASE(JungleVertexBuffer);
	SAFE_RELEASE(JungleIndexBuffer);

#pragma endregion

#pragma region Release Texture Stuff

	for (int i = 0; i < RASTERIZER_STATE::COUNT; ++i)
		SAFE_RELEASE(d3dRasterizerState[i]);
	SAFE_RELEASE(d3dSamplerState);
	for (int i = 0; i < DIFFUSE_TEXTURES::COUNT; ++i)
		SAFE_RELEASE(d3dDiffuseTextures[i]);
	for (int i = 0; i < BLEND_STATE::COUNT; ++i)
		SAFE_RELEASE(d3dBlendState[i]);

#pragma endregion

#pragma region Release ShaderStuff

	for (int i = 0; i < INPUT_LAYOUT::COUNT; ++i)
		SAFE_RELEASE(d3dInputLayout[i]);
	for (int i = 0; i < VERTEX_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dVertexShader[i]);
	for (int i = 0; i < PIXEL_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dPixelShader[i]);
	for (int i = 0; i < GEOMETRY_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dGeometryShader[i]);
	for (int i = 0; i < COMPUTE_SHADER::COUNT; ++i)
		SAFE_RELEASE(d3dComputeShader[i]);
	for (int i = 0; i < CONSTANT_BUFFER::COUNT; ++i)
		SAFE_RELEASE(d3dConstBuffers[i]);

#pragma endregion

#pragma region Release SpriteStuff

	delete d3dSpriteFont;
	delete d3dSpriteBatch;

#pragma endregion

#pragma region Release Essentials

	SAFE_RELEASE(d3dRenderTargetView);
	SAFE_RELEASE(d3dDepthStencilView);
	for (int i = 0; i < DEPTH_STENCIL_STATE::COUNT; ++i)
		SAFE_RELEASE(d3dDepthStencilState[i]);
	SAFE_RELEASE(d3dSwapChain);
	SAFE_RELEASE(d3dContext);
	SAFE_RELEASE(d3dDevice);
	GW_SAFE_RELEASE(d3dSurface);

#pragma endregion
}

void DirectXData::updateCameras()
{
	if (debugCamDelta.x != 0 || debugCamDelta.y != 0 || debugCursorRot.x != 0 || debugCursorRot.y != 0)
	{
		debugCamMat = DirectX::XMMatrixTranslation(debugCamDelta.x, 0.0f, debugCamDelta.y) * debugCamMat;

		DirectX::XMFLOAT4 debugPos;
		DirectX::XMStoreFloat4(&debugPos, debugCamMat.r[3]);
		debugCamMat.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		debugCamMat = DirectX::XMMatrixRotationX(debugCursorRot.y) * debugCamMat;
		debugCamMat = debugCamMat * DirectX::XMMatrixRotationY(debugCursorRot.x);

		debugCamMat.r[3] = DirectX::XMLoadFloat4(&debugPos);

		debugCursorRot = { 0.0f, 0.0f };
		DirectX::XMStoreFloat3(&debugCamPos, debugCamMat.r[3]);
	}
}

DirectX::XMMATRIX DirectXData::screenShake() {
	float radius = 0.5f;
	float randomAngle = rand() % 360;
	float2 offset;

	offset.x = sin(randomAngle) * radius * 0.25;
	offset.y = cos(randomAngle) * radius * 0.25;
	randomAngle += (150 + rand() % 60);

	DirectX::XMMATRIX viewPos = viewMat;
	viewMat = DirectX::XMMatrixTranslation(offset.x, 0.0f, offset.y) * viewMat;

	return viewPos;
}

void DirectXData::resetCamera() {
	viewMat = tempCamera;
}