#include "CRendererManager.h"
#include "CGame.h"


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

bool CRendererManager::Draw(double timepassed, int gamestate, CGame* parentGame)
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
	{
		g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
	}

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };


	g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 0);
	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	//g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	const float bg_green[] = { 0.2f, 0.5f, 0.2f, 1.0f };


	g_d3dData->d3dContext->ClearRenderTargetView(g_d3dData->d3dRenderTargetView, DirectX::Colors::Wheat);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState2);
	/*

	if (gamestate == GAME_STATE::ARCADE_GAME)
	{
		theSkybox->Update();
		theSkybox->Render();
		SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
		if (G_SUCCESS(g_d3dData->d3dSurface->GetDepthStencilView((void**)&g_d3dData->d3dDepthStencilView)))
		{
			g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
		}
	}
	*/


	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState[RASTERIZER_STATE::DEFAULT]);
	/*********DRAW OTHER STUFF HERE************/
	//if(gamestate == 3)
	//	v_tMeshTemplates[MODELS::BATTLEMAGE].render(g_d3dData->d3dContext, timepassed);
	for (CObject* c : rendereableObjects)
		c->Draw(timepassed);


	/*
	if (gamestate == 3)
	{
		UINT stride[] = { sizeof(KeyVertex) };
		UINT offset[] = { 0 };

		//Skybox
		g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerStateSKYBOX);
		//ID3D11ShaderResourceView* texViews[] = { g_d3dData->JungleSRV };
		//Sky Vertex Buffer Update
		ID3D11Buffer* tempVB[] = { g_d3dData->JungleVertexBuffer };
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
		cb.mProjMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, (float)g_d3dData->windowWidthHeight.x / (float)g_d3dData->windowWidthHeight.y, 0.01f, 1100.0f));

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
	*/

	g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::TWO_D], 0);

	g_d3dData->d3dSpriteBatch->Begin();
	float aspectRatio = (float)g_d3dData->windowWidthHeight.x / (float)g_d3dData->windowWidthHeight.y;
	float invAspect = (float)g_d3dData->windowWidthHeight.y / (float)g_d3dData->windowWidthHeight.x;
	DirectX::GXMVECTOR scale = { 0.1f * aspectRatio, 0.1f * aspectRatio, 0.1f * aspectRatio, 1.0f };

	DirectX::XMVECTOR measurement = g_d3dData->d3dSpriteFont->MeasureString("Arcade Mode");

	if (gamestate == GAME_STATE::ARCADE_GAME)
	{
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Arcade Mode", DirectX::XMVECTOR{ (0.5f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.01f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::SaddleBrown, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}

	float x = (0.5f - (0.6f * ((float)g_d3dData->windowWidthHeight.y / (float)g_d3dData->windowWidthHeight.x)));
	if (parentGame->GetPlayer(0))
	{
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 1:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x , (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.15f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(0)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(0)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(1))
	{
		x += 0.14f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 2:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::Blue, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(1)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(1)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(2))
	{
		x += 0.14*invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 3:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::FXMVECTOR{ 0.059f, 0.231f, 0.047f, 1.0f }, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(2)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(2)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(3) || parentGame->GetAI(0))
	{
		x += 0.14f * invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		if (parentGame->GetPlayer(3))
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetPlayer(3)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(3)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		else if (parentGame->GetAI(0))
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(0)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetAI(0)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);


	}

	g_d3dData->d3dSpriteBatch->End();

	g_d3dData->d3dSwapChain->Present(1, 0);

	rendereableObjects.clear();

	SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
	return true;
}

CRendererManager::CRendererManager()
{
	//theSkybox = new Skybox();
	//theSkybox->Initialize();
}


CRendererManager::~CRendererManager()
{
}
