#include "CRendererManager.h"



bool CRendererManager::HasComponent(CObject _objToCheck, int _componentToCheck)
{
	for (TComponent* c : _objToCheck.GetComponenets())
		if (c->GetComponentType() == COMPONENT_TYPE::RENDERER)
			return true;
	return false;
}


void CRendererManager::RenderObject(CObject _objToDraw)
{
	rendereableObjects.push_back(_objToDraw);
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
		g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };

	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	//g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);

	const float bg_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

	g_d3dData->d3dContext->ClearRenderTargetView(g_d3dData->d3dRenderTargetView, bg_green);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState);

	/*********DRAW OTHER STUFF HERE************/

	for (CObject c : rendereableObjects)
		c.Draw();

	UINT strides[] = { sizeof(TLineVertex) };
	UINT offsets[] = { 0 };

	g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dVertexBuffers[VERTEX_BUFFER::LINE], 0, nullptr, get_line_verts(), 0, 0);
	g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &g_d3dData->d3dVertexBuffers[VERTEX_BUFFER::LINE], strides, offsets);
	g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[INPUT_LAYOUT::LINE]);
	g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[VERTEX_SHADER::LINE], 0, 0);

	g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[PIXEL_SHADER::LINE], nullptr, 0);
	g_d3dData->d3dContext->Draw(get_line_vert_count(), 0);
	clear_lines();

	g_d3dData->d3dSwapChain->Present(1, 0);

	rendereableObjects.clear();

	SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
	SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
	return true;
}

CRendererManager::CRendererManager()
{
}


CRendererManager::~CRendererManager()
{
}
