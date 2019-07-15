/****************************************************
*	Filename:			TImpl.h
*	Date:				07/11/2019
*	Mod. Date:			07/15/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the header file for
*						the Render Implementation
*						class and holds all of its
*						declerations and members
****************************************************/
#pragma once



#include "CRenderer.h"


struct CRenderer::TImpl
{

	GW::GRAPHICS::GDirectX11Surface*	p_d3dSurface = nullptr;
	ID3D11Device*						p_d3dDevice = nullptr;
	ID3D11DeviceContext*				p_d3dContext = nullptr;
	IDXGISwapChain*						p_d3dSwapChain = nullptr;

	ID3D11DepthStencilView*				p_d3dDepthStencilView[DEPTH_STENCIL_VIEW::COUNT]{};
	ID3D11DepthStencilState*			p_d3dDepthStencilState[DEPTH_STENCIL_STATE::COUNT]{};
	ID3D11RenderTargetView*				p_d3dRenderTargetView[RENDER_TARGET_VIEW::COUNT]{};
	ID3D11RasterizerState*				p_d3dRasterizerState[RASTERIZER_STATE::COUNT]{};

	D3D11_VIEWPORT						d3dViewport[VIEWPORT::COUNT];


	/*************************************************************************
	*	Initialize()
	*							Initializes the Renderer Implementation.
	*							Returns true if implementation was initialized
	*							successfully or false if not.
	*	Ins:
	*							GWindow* window_handle
	*	Outs:
	*							None
	*	Returns:
	*							Bool
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Intials:			D.S.
	*************************************************************************/
	bool Intialize(GW::SYSTEM::GWindow* window_handle);

	/*********************************************************
	*	Render()
	*							Renders the current frame
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							void
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	*********************************************************/
	bool Render();

	/********************************************************
	*	Cleanup()
	*							Cleans up the Renderer
	*							Implementation. Called
	*							before delete;
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							void
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	********************************************************/
	void Cleanup();

	/************************************************
	*	~TImpl()
	*							Destructor
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							None
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	************************************************/
	~TImpl();

	/***********************************************
	*	TImpl()
	*							Defaut Constructor
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							None
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	***********************************************/
	TImpl();
};

