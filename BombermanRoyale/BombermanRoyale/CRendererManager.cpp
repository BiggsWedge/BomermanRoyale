#include "CRendererManager.h"



bool CRendererManager::HasComponent(CObject _objToCheck, int _componentToCheck)
{
	for (auto& c : _objToCheck.GetComponenets())
		if (c->GetComponentType() == COMPONENT_TYPE::RENDERER)
			return true;
	return false;
}

bool CRendererManager::RenderObject(CObject _objToDraw)
{
	int index = g_d3dData->d3dCommandLists.size();
	g_d3dData->d3dCommandLists.resize(index + 1);

	if (HasComponent(_objToDraw, COMPONENT_TYPE::RENDERER))
	{
		g_d3dData->d3dCommandLists[index] = _objToDraw.Draw();
		return true;
	}


	return false;
}

bool CRendererManager::Draw()
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
		g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
	}
	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };

	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	//g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);

	const float bg_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

	g_d3dData->d3dContext->ClearRenderTargetView(g_d3dData->d3dRenderTargetView, bg_green);

	/*********DRAW OTHER STUFF HERE************/









	for (ID3D11CommandList* d3dList : g_d3dData->d3dCommandLists)
	{
		g_d3dData->d3dContext->ExecuteCommandList(d3dList, false);
	}

	g_d3dData->d3dSwapChain->Present(1, 0);

	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
	return true;
}

CRendererManager::CRendererManager()
{
}


CRendererManager::~CRendererManager()
{
}
