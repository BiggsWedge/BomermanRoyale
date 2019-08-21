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

	g_d3dData->d3dContext->ClearRenderTargetView(g_d3dData->d3dRenderTargetView, bg_green);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState2);

	/*********DRAW OTHER STUFF HERE************/
	if (gamestate == 3)
		v_tMeshTemplates[MODELS::BATTLEMAGE].render(g_d3dData->d3dContext, timepassed);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState);
	for (CObject* c : rendereableObjects)
		c->Draw(timepassed);


	/*
	g_d3dData->d3dSpriteBatch->Begin();
	g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Barnyard Blast", { 100, 100 , 0, 0 }, DirectX::Colors::NavajoWhite);
	g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Barnyard Blast", { 100, 150 , 0, 0 }, DirectX::Colors::Peru);
	g_d3dData->d3dSpriteBatch->End();
	*/

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
