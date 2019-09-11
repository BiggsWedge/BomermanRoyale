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
	TMeshTemplate* thisTemplate = &v_tMeshTemplates[MODELS::CUBE];
	HRESULT res;
	uNumIndices = thisTemplate->v_iIndices.size();
	std::vector<TSimpleVertex> tempVerts;
	tempVerts.resize(thisTemplate->v_tVertices.size());
	for (int i = 0; i < tempVerts.size(); ++i)
	{
		tempVerts[i].fPosition.x = (thisTemplate->v_tVertices[i].fPosition.x / 50.0f);
		tempVerts[i].fPosition.y = (thisTemplate->v_tVertices[i].fPosition.y / 50.0f) - 1.0f;
		tempVerts[i].fPosition.z = (thisTemplate->v_tVertices[i].fPosition.z / 50.0f);
	}

	D3D11_BUFFER_DESC vBuff;
	vBuff.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuff.ByteWidth = sizeof(TSimpleVertex) * tempVerts.size();
	vBuff.CPUAccessFlags = 0;
	vBuff.MiscFlags = 0;
	vBuff.StructureByteStride = 0;
	vBuff.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = tempVerts.data();

	res = g_d3dData->d3dDevice->CreateBuffer(&vBuff, &subData, &d3dVertexBuffer);

	D3D11_BUFFER_DESC iBuff;
	iBuff.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuff.ByteWidth = sizeof(int) * thisTemplate->v_iIndices.size();
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

	res = g_d3dData->d3dDevice->CreateInputLayout(d3dSkyShaderDesc.data(), (UINT)d3dSkyShaderDesc.size(), SkyVertex, sizeof(SkyVertex), &d3dInputLayout);

	p_d3dUsedSampler = &g_d3dData->d3dSamplerState;
	p_d3dUsedTexture = &g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::FIRE_TEX];

	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
}

void Skybox::Update()
{
	tConstantBuffer.mModelMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(g_d3dData->camPos.x, g_d3dData->camPos.y, g_d3dData->camPos.z));
	//tConstantBuffer.mModelMatrix = DirectX::XMMatrixTranspose(g_d3dData->camMat);
	tConstantBuffer.mViewMatrix = DirectX::XMMatrixTranspose(g_d3dData->viewMat);
	tConstantBuffer.mProjMatrix = DirectX::XMMatrixTranspose(g_d3dData->projMat);

	g_d3dData->d3dContext->UpdateSubresource(d3dConstantBuffer, 0, nullptr, &tConstantBuffer, 0, 0);
}

void Skybox::Render()
{
	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };
	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);

	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState[RASTERIZER_STATE::SKYBOX]);
	g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 0);

	g_d3dData->d3dContext->IASetInputLayout(d3dInputLayout);
	g_d3dData->d3dContext->IASetIndexBuffer(d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	UINT strides = sizeof(TSimpleVertex);
	UINT offsets = 0;

	g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &d3dVertexBuffer, &strides, &offsets);

	g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dData->d3dContext->VSSetShader(d3dVertexShader, nullptr, 0);
	g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &d3dConstantBuffer);

	g_d3dData->d3dContext->PSSetShader(d3dPixelShader, nullptr, 0);
	g_d3dData->d3dContext->PSSetSamplers(0, 1, p_d3dUsedSampler);
	g_d3dData->d3dContext->PSSetShaderResources(0, 1, p_d3dUsedTexture);

	g_d3dData->d3dContext->DrawIndexed(uNumIndices, 0, 0);
}