#include "Skybox.h"
#include "SkyVertex.csh"
#include "SkyPixel.csh"


Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Cleanup()
{
	SAFE_RELEASE(d3dVertexBuffer);
	SAFE_RELEASE(d3dIndexBuffer);
	SAFE_RELEASE(d3dConstantBuffer);
	SAFE_RELEASE(d3dCommandList);
	SAFE_RELEASE(d3dInputLayout);
	SAFE_RELEASE(d3dPixelShader);
	SAFE_RELEASE(d3dVertexShader);
	p_d3dUsedSampler = nullptr;
	p_d3dUsedTexture = nullptr;

}

void Skybox::Initialize()
{
	KeyVertex verts[] =
	{
		//-z plane
		{DirectX::XMFLOAT3(-1,-1,-1), DirectX::XMFLOAT2(0,1)},
		{DirectX::XMFLOAT3(-1,1,-1), DirectX::XMFLOAT2(0,0)},
		{DirectX::XMFLOAT3(1,-1,-1), DirectX::XMFLOAT2(1,1)},
		{DirectX::XMFLOAT3(1,1,-1), DirectX::XMFLOAT2(1,0)},

		//+z plane
		{DirectX::XMFLOAT3(-1,-1,1), DirectX::XMFLOAT2(0,1)},
		{DirectX::XMFLOAT3(-1,1,1), DirectX::XMFLOAT2(0,0)},
		{DirectX::XMFLOAT3(1,-1,1), DirectX::XMFLOAT2(1,1)},
		{DirectX::XMFLOAT3(1,1,1), DirectX::XMFLOAT2(1,0)},

		//-y plane
		{DirectX::XMFLOAT3(-1,-1,-1) , DirectX::XMFLOAT2(0,1)},
		{DirectX::XMFLOAT3(-1,-1,1),  DirectX::XMFLOAT2(0,0)},
		{DirectX::XMFLOAT3(1,-1,-1),  DirectX::XMFLOAT2(1,1)},
		{DirectX::XMFLOAT3(1,-1,1),  DirectX::XMFLOAT2(1,0)},

		//+y plane
		{DirectX::XMFLOAT3(-1,1,-1)	, DirectX::XMFLOAT2(0,1)},
		{DirectX::XMFLOAT3(-1,1,1),	 DirectX::XMFLOAT2(0,0)},
		{DirectX::XMFLOAT3(1,1,-1),	 DirectX::XMFLOAT2(1,1)},
		{DirectX::XMFLOAT3(1,1,1), 	DirectX::XMFLOAT2(1,0)},

		//-x plane
		{DirectX::XMFLOAT3(-1,-1,-1) , DirectX::XMFLOAT2(0,1)},
		{DirectX::XMFLOAT3(-1,1,-1),  DirectX::XMFLOAT2(0,0)},
		{DirectX::XMFLOAT3(-1,-1,1),  DirectX::XMFLOAT2(1,1)},
		{DirectX::XMFLOAT3(-1,1,1),  DirectX::XMFLOAT2(1,0)},

		//+x plane
		{DirectX::XMFLOAT3(1,-1,-1), DirectX::XMFLOAT2(0,1)},
		{DirectX::XMFLOAT3(1,1,-1), DirectX::XMFLOAT2(0,0)},
		{DirectX::XMFLOAT3(1,-1,1), DirectX::XMFLOAT2(1,1)},
		{DirectX::XMFLOAT3(1,1,1), DirectX::XMFLOAT2(1,0)},

	};

	TMeshTemplate* thisTemplate = &v_tMeshTemplates[MODELS::CUBE];
	HRESULT res;
	uNumIndices = thisTemplate->v_iIndices.size();

	D3D11_BUFFER_DESC vBuff;
	vBuff.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuff.ByteWidth = sizeof(KeyVertex) * thisTemplate->v_tVertices.size();
	vBuff.CPUAccessFlags = 0;
	vBuff.MiscFlags = 0;
	vBuff.StructureByteStride = 0;
	vBuff.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = thisTemplate->v_tVertices.data();

	res = g_d3dData->d3dDevice->CreateBuffer(&vBuff, &subData, &d3dVertexBuffer);

	D3D11_BUFFER_DESC iBuff;
	iBuff.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuff.ByteWidth = sizeof(unsigned int) * thisTemplate->v_iIndices.size();
	iBuff.CPUAccessFlags = 0;
	iBuff.MiscFlags = 0;
	iBuff.StructureByteStride = 0;
	iBuff.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA iSubData;
	iSubData.pSysMem = thisTemplate->v_iIndices.data();

	res = g_d3dData->d3dDevice->CreateBuffer(&iBuff, &iSubData, &d3dIndexBuffer);

	D3D11_BUFFER_DESC cBuff;
	cBuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBuff.ByteWidth = sizeof(TBasicVertexConstBuff);
	cBuff.CPUAccessFlags = 0;
	cBuff.MiscFlags = 0;
	cBuff.StructureByteStride = 0;
	cBuff.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA cSubData;
	cSubData.pSysMem = &tConstantBuffer;

	res = g_d3dData->d3dDevice->CreateBuffer(&cBuff, &cSubData, &d3dConstantBuffer);

	res = g_d3dData->d3dDevice->CreateVertexShader(SkyVertex, sizeof(SkyVertex), nullptr, &d3dVertexShader);
	res = g_d3dData->d3dDevice->CreatePixelShader(SkyPixel, sizeof(SkyPixel), nullptr, &d3dPixelShader);

	std::vector<D3D11_INPUT_ELEMENT_DESC> d3dSkyShaderDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	g_d3dData->d3dSurface->GetRenderTarget((void**)&g_d3dData->d3dRenderTargetView);


	res = g_d3dData->d3dDevice->CreateInputLayout(d3dSkyShaderDesc.data(), (UINT)d3dSkyShaderDesc.size(), SkyVertex, sizeof(SkyVertex), &d3dInputLayout);

	p_d3dUsedSampler = &g_d3dData->d3dSamplerState;
	p_d3dUsedTexture = &g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::FIRE_TEX];

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };
	g_d3dData->d3dDeferredContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dDeferredContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);

	g_d3dData->d3dDeferredContext->RSSetState(g_d3dData->d3dRasterizerState);
	g_d3dData->d3dDeferredContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 0);

	g_d3dData->d3dDeferredContext->IASetInputLayout(d3dInputLayout);
	g_d3dData->d3dDeferredContext->IASetIndexBuffer(d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	UINT strides = sizeof(TSimpleVertex);
	UINT offsets = 0;

	g_d3dData->d3dDeferredContext->IASetVertexBuffers(0, 1, &d3dVertexBuffer, &strides, &offsets);

	g_d3dData->d3dDeferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dData->d3dDeferredContext->VSSetShader(d3dVertexShader, nullptr, 0);
	g_d3dData->d3dDeferredContext->VSSetConstantBuffers(0, 1, &d3dConstantBuffer);

	g_d3dData->d3dDeferredContext->PSSetShader(d3dPixelShader, nullptr, 0);
	g_d3dData->d3dDeferredContext->PSSetSamplers(0, 1, p_d3dUsedSampler);
	g_d3dData->d3dDeferredContext->PSSetShaderResources(0, 1, p_d3dUsedTexture);

	g_d3dData->d3dDeferredContext->DrawIndexed(uNumIndices, 0, 0);

	g_d3dData->d3dDeferredContext->FinishCommandList(false, &d3dCommandList);
	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
}

void Skybox::Update()
{
	tConstantBuffer.mModelMatrix = DirectX::XMMatrixTranspose(g_d3dData->camMat);
	tConstantBuffer.mViewMatrix = DirectX::XMMatrixTranspose(g_d3dData->viewMat);
	tConstantBuffer.mProjMatrix = DirectX::XMMatrixTranspose(g_d3dData->projMat);

	g_d3dData->d3dContext->UpdateSubresource(d3dConstantBuffer, 0, nullptr, &tConstantBuffer, 0, 0);
}

void Skybox::Render()
{
	g_d3dData->d3dContext->ExecuteCommandList(d3dCommandList, false);
}