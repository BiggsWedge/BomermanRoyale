#include "CRendererManager.h"



bool CRendererManager::HasComponent(CObject _objToCheck, int _componentToCheck)
{
	for (TComponent* c : _objToCheck.GetComponenets())
		if (c->GetComponentType() == COMPONENT_TYPE::RENDERER)
			return true;
	return false;
}

void CRendererManager::RenderObject(CObject* _objToDraw)
{
	rendereableObjects.push_back(_objToDraw);
}

void CRendererManager::RenderObjectCollider(CObject * _objToDraw)
{
	TComponent* tCollider = nullptr;
	if (_objToDraw->GetComponent(COMPONENT_TYPE::COLLIDER, tCollider))
	{
		TColliderComponent* collider = (TColliderComponent*)tCollider;
	}
}

bool CRendererManager::Draw(double timepassed, int gamestate)
{

	if (!g_d3dData->d3dSurface)
	{
		//Log Error
		return false;
	}

	if (!G_SUCCESS(g_d3dData->d3dSurface->GetRenderTarget((void**)&g_d3dData->d3dRenderTargetView)))
	{
		//Log Failure
		return false;
	}

	if (G_SUCCESS(g_d3dData->d3dSurface->GetDepthStencilView((void**)&g_d3dData->d3dDepthStencilView)))
		g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };

	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	//g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);

	const float bg_green[] = { 0.2f, 0.5f, 0.2f, 1.0f };

	g_d3dData->d3dContext->ClearRenderTargetView(g_d3dData->d3dRenderTargetView,  bg_green);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState2);

	/*********DRAW OTHER STUFF HERE************/
	//if(gamestate == 3)
	//	v_tMeshTemplates[MODELS::BATTLEMAGE].render(g_d3dData->d3dContext, timepassed);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState);
	for (CObject* c : rendereableObjects)
		c->Draw(timepassed);

	if (gamestate == 3)
	{
		UINT strides[] = { sizeof(TLineVertex) };
		UINT offsets[] = { 0 };

		//Skybox
		g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerStateSKYBOX);
		ID3D11ShaderResourceView* texViews[] = { g_d3dData->JungleSRV };
		//Sky Vertex Buffer Update
		ID3D11Buffer* tempVB[] = { g_d3dData->JungleVertexBuffer };
		UINT stride[] = { sizeof(KeyVertex) };
		UINT offset[] = { 0 };
		g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &tempVB[0], stride, offset);
		g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[INPUT_LAYOUT::SKY]);
		//Sky Index Buffer
		g_d3dData->d3dContext->IASetIndexBuffer(g_d3dData->JungleIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectX::XMFLOAT3 CameraPos = g_d3dData->debugCamPos;
		CameraPos.y = 5.0f;
		DirectX::XMVECTOR FocalPoint = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR UpDirectionOfCamera = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&g_d3dData->debugCamPos), FocalPoint, UpDirectionOfCamera);

		//Sky Constant Buffer
		DirectX::XMMATRIX ScalingMatrix = DirectX::XMMatrixScaling(45.0f, 60.0f, 50.0f);
		TBasicVertexConstBuff cb;
		ScalingMatrix.r[3] = g_d3dData->basicConstBuff.mViewMatrix.r[3];
		cb.mModelMatrix = DirectX::XMMatrixTranspose(ScalingMatrix);
		//cb.mViewMatrix = DirectX::XMMatrixInverse(nullptr, View);
		cb.mViewMatrix = g_d3dData->basicConstBuff.mViewMatrix;
		cb.mProjMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1920.0f / 1080.0f, 0.01f, 1100.0f));

		D3D11_MAPPED_SUBRESOURCE mapsr;
		ZeroMemory(&mapsr, sizeof(mapsr));

		g_d3dData->d3dContext->Map(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::SKY], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapsr);
		std::memcpy(mapsr.pData, &cb, sizeof(TBasicVertexConstBuff));
		g_d3dData->d3dContext->Unmap(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::SKY], 0);


		//Sky shaders Set Up
		g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[VERTEX_SHADER::SKY], nullptr, 0);
		g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::SKY]);
		g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[PIXEL_SHADER::SKY], nullptr, 0);
		g_d3dData->d3dContext->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::SKY]);
		g_d3dData->d3dContext->PSSetShaderResources(0, 1, &g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::FIRE_TEX]);
		//g_d3dData->d3dContext->PSSetSamplers(0, 1, &g_d3dData->JungleSampler);

		//Draw
		g_d3dData->d3dContext->DrawIndexed(36, 0, 0);

	}

	g_d3dData->d3dSwapChain->Present(1, 0);

	rendereableObjects.clear();

	SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
	return true;
}

CRendererManager::CRendererManager()
{
}


CRendererManager::~CRendererManager()
{
}
