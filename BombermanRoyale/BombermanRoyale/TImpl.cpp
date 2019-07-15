/****************************************************
*	Filename:			TImpl.cpp
*	Date:				07/11/2019
*	Mod. Date:			07/15/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the impelmentation
*						file that is the companion
*						to the TImpl.h file;
****************************************************/
#include "TImpl.h"

CRenderer::TImpl::TImpl()
{

}

bool CRenderer::TImpl::Intialize(GW::SYSTEM::GWindow* window_handle)
{
	if (!G_SUCCESS(GW::GRAPHICS::CreateGDirectX11Surface(window_handle, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, &p_d3dSurface)))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to create the DirectX Surface");
		return false;
	}

	p_d3dSurface->GetDevice((void**)&p_d3dDevice);
	p_d3dSurface->GetSwapchain((void**)&p_d3dSwapChain);
	p_d3dSurface->GetContext((void**)&p_d3dContext);

	DXGI_SWAP_CHAIN_DESC d3dSwapChainDesc;
	p_d3dSwapChain->GetDesc(&d3dSwapChainDesc);

	ID3D11Texture2D* p_d3dDepthStencilTexture;
	D3D11_TEXTURE2D_DESC d3dDepthStencilTextureDesc;
	ZeroMemory(&d3dDepthStencilTextureDesc, sizeof(d3dDepthStencilTextureDesc));

	d3dDepthStencilTextureDesc.Width = d3dSwapChainDesc.BufferDesc.Width;
	d3dDepthStencilTextureDesc.Height = d3dSwapChainDesc.BufferDesc.Height;
	d3dDepthStencilTextureDesc.MipLevels = 1;
	d3dDepthStencilTextureDesc.ArraySize = 1;
	d3dDepthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilTextureDesc.SampleDesc.Count = 1;
	d3dDepthStencilTextureDesc.SampleDesc.Quality = 0;
	d3dDepthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilTextureDesc.CPUAccessFlags = 0;
	d3dDepthStencilTextureDesc.MiscFlags = 0;

	if (FAILED(p_d3dDevice->CreateTexture2D(&d3dDepthStencilTextureDesc, nullptr, &p_d3dDepthStencilTexture)))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to create the Depth Stencil Texture");
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	ZeroMemory(&d3dDepthStencilDesc, sizeof(d3dDepthStencilDesc));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	d3dDepthStencilDesc.StencilEnable = TRUE;
	d3dDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	d3dDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(p_d3dDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &p_d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT])))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to create the Depth Stencil State.");
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(d3dDepthStencilViewDesc));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(p_d3dDevice->CreateDepthStencilView(p_d3dDepthStencilTexture, &d3dDepthStencilViewDesc, &p_d3dDepthStencilView[DEPTH_STENCIL_VIEW::DEFAULT])))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to create the Depth Stencil View");
		return false;
	}

	ZeroMemory(&d3dViewport[VIEWPORT::DEFAULT], sizeof(d3dViewport[VIEWPORT::DEFAULT]));

	d3dViewport[VIEWPORT::DEFAULT].Height = d3dSwapChainDesc.BufferDesc.Height;
	d3dViewport[VIEWPORT::DEFAULT].Width = d3dSwapChainDesc.BufferDesc.Width;
	d3dViewport[VIEWPORT::DEFAULT].TopLeftX = d3dViewport[VIEWPORT::DEFAULT].TopLeftY = 0;
	d3dViewport[VIEWPORT::DEFAULT].MaxDepth = 1;
	d3dViewport[VIEWPORT::DEFAULT].MinDepth = 0;

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

	if (FAILED(p_d3dDevice->CreateRasterizerState(&d3dRasterDesc, &p_d3dRasterizerState[RASTERIZER_STATE::DEFAULT])))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to create Rasterizer State");
		return false;
	}

	SAFE_RELEASE(p_d3dDepthStencilTexture);

	return true;
}

bool CRenderer::TImpl::Render()
{
	if (p_d3dRenderTargetView[RENDER_TARGET_VIEW::DEFAULT])
		SAFE_RELEASE(p_d3dRenderTargetView[RENDER_TARGET_VIEW::DEFAULT]);

	if (!p_d3dSurface)
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to render. No surface available.");
		return false;
	}

	if (!G_SUCCESS(p_d3dSurface->GetRenderTarget((void**)&p_d3dRenderTargetView[RENDER_TARGET_VIEW::DEFAULT])))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to get the Render Target.");
		return false;
	}

	p_d3dContext->ClearDepthStencilView(p_d3dDepthStencilView[DEPTH_STENCIL_VIEW::DEFAULT], D3D11_CLEAR_DEPTH, 1.0f, 0);

	p_d3dContext->OMSetRenderTargets(1, p_d3dRenderTargetView, p_d3dDepthStencilView[DEPTH_STENCIL_VIEW::DEFAULT]);
	p_d3dContext->RSSetViewports(1, &d3dViewport[VIEWPORT::DEFAULT]);

	const float bg_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

	p_d3dContext->ClearRenderTargetView(p_d3dRenderTargetView[RENDER_TARGET_VIEW::DEFAULT], bg_green);

	p_d3dSwapChain->Present(1, 0);

}

void CRenderer::TImpl::Cleanup()
{
	for (int i = 0; i < RENDER_TARGET_VIEW::COUNT; ++i)
		//SAFE_RELEASE(p_d3dRenderTargetView[i]);
		p_d3dRenderTargetView[i]->Release();
	for (int i = 0; i < DEPTH_STENCIL_STATE::COUNT; ++i)
		//SAFE_RELEASE(p_d3dDepthStencilState[i]);
		p_d3dDepthStencilState[i]->Release();
	for (int i = 0; i < DEPTH_STENCIL_VIEW::COUNT; ++i)
		//SAFE_RELEASE(p_d3dDepthStencilView[i]);
		p_d3dDepthStencilView[i]->Release();
	for (int i = 0; i < RASTERIZER_STATE::COUNT; ++i)
		SAFE_RELEASE(p_d3dRasterizerState[i]);

	SAFE_RELEASE(p_d3dSwapChain);
	SAFE_RELEASE(p_d3dContext);
	/*
	ID3D11Debug* p_d3dDebugger = nullptr;
	p_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&p_d3dDebugger));
	p_d3dDebugger->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	SAFE_RELEASE(p_d3dDebugger);
	*/

	SAFE_RELEASE(p_d3dDevice);
	p_d3dSurface->DecrementCount();
}

CRenderer::TImpl::~TImpl()
{
	Cleanup();
}