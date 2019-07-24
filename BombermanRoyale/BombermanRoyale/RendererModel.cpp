/*******************************************************
*	Filename:			RendererModel.cpp
*	Date:				07/16/2019
*	Mod. Date:			07/16/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contains the
*						implementation for the Renderer
*						model and all of it sub-classes
*						and structures.
*******************************************************/
#include "RendererModel.h"

TRendererVertexShaderSet::TRendererVertexShaderSet()
{
	p_d3d11InputLayout = nullptr;
	p_d3d11VertexShader = nullptr;
}

TRendererVertexShaderSet::TRendererVertexShaderSet(ID3D11VertexShader* p_d3d11VertexShaderToAdd, ID3D11InputLayout* p_d3d11InputLayoutToAdd)
{
	p_d3d11InputLayout = &p_d3d11InputLayoutToAdd;
	p_d3d11VertexShader = &p_d3d11VertexShaderToAdd;
}

TRendererVertexShaderSet::~TRendererVertexShaderSet()
{
	p_d3d11InputLayout = nullptr;
	p_d3d11VertexShader = nullptr;
}

TRendererShaderSet::TRendererShaderSet()
{
	tRendererVertexShaderSet.p_d3d11InputLayout = nullptr;
	tRendererVertexShaderSet.p_d3d11VertexShader = nullptr;
	p_d3d11PixelShader = nullptr;
	p_d3d11GeometryShader = nullptr;
}

TRendererShaderSet::TRendererShaderSet(
	ID3D11VertexShader* p_d3d11VertexShaderToAdd,
	ID3D11InputLayout* p_d3d11InputLayoutToAdd,
	ID3D11PixelShader* p_d3d11PixelShaderToAdd,
	ID3D11GeometryShader* p_d3d11GeometryShaderToAdd)
{
	tRendererVertexShaderSet.p_d3d11VertexShader = &p_d3d11VertexShaderToAdd;
	tRendererVertexShaderSet.p_d3d11InputLayout = &p_d3d11InputLayoutToAdd;
	p_d3d11PixelShader = &p_d3d11PixelShaderToAdd;
	p_d3d11GeometryShader = &p_d3d11GeometryShaderToAdd;
}

TRendererShaderSet::~TRendererShaderSet()
{
	tRendererVertexShaderSet.p_d3d11InputLayout = nullptr;
	tRendererVertexShaderSet.p_d3d11VertexShader = nullptr;
	p_d3d11PixelShader = nullptr;
	p_d3d11GeometryShader = nullptr;
}



