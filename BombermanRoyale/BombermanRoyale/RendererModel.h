/*******************************************************
*	Filename:			RendererModel.h
*	Date:				07/16/2019
*	Mod. Date:			07/16/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contains the Renderer
*						model and all of it sub-classes
*						and structures.
*******************************************************/
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
struct TResource
{
};

struct TRendererVertexShaderSet
{
	ID3D11VertexShader**		p_d3d11VertexShader;
	ID3D11InputLayout**			p_d3d11InputLayout;

	TRendererVertexShaderSet();
	TRendererVertexShaderSet(ID3D11VertexShader* p_d3d11VertexShaderToAdd, ID3D11InputLayout* p_d3d11InputLayoutToAdd);
	~TRendererVertexShaderSet();
};

struct TRendererShaderSet
{

private:

	TRendererVertexShaderSet	tRendererVertexShaderSet;
	ID3D11PixelShader**			p_d3d11PixelShader;
	ID3D11GeometryShader**		p_d3d11GeometryShader;

public:

	TRendererShaderSet();
	TRendererShaderSet(
		ID3D11VertexShader* p_d3d11VertexShaderToAdd,
		ID3D11InputLayout* p_d3d11InputLayoutToAdd,
		ID3D11PixelShader* p_d3d11PixelShaderToAdd,
		ID3D11GeometryShader* p_d3d11GeometryShaderToAdd = nullptr);
	~TRendererShaderSet();


	/*****************************************
	*	Accessors
	*****************************************/

	inline ID3D11VertexShader* GetVertexShader() { return *(tRendererVertexShaderSet.p_d3d11VertexShader); }
	inline ID3D11InputLayout* GetInputLayout() { return*(tRendererVertexShaderSet.p_d3d11InputLayout); }
	inline ID3D11PixelShader* GetPixelShader() { return *p_d3d11PixelShader; }
	inline ID3D11GeometryShader* GetGeometryShader() { return *p_d3d11GeometryShader; }

	/*****************************************
	*	Mutators
	*****************************************/

	inline void SetVertexShaderSet(ID3D11VertexShader* p_d3d11VertexShaderToAdd, ID3D11InputLayout* p_d3d11InputLayoutToAdd) {
		tRendererVertexShaderSet.p_d3d11VertexShader = &p_d3d11VertexShaderToAdd;
		tRendererVertexShaderSet.p_d3d11InputLayout = &p_d3d11InputLayoutToAdd;
	}
	inline void SetPixelShader(ID3D11PixelShader* p_d3d11PixelShaderToAdd) { p_d3d11PixelShader = &p_d3d11PixelShaderToAdd; }
	inline void SetGeometryShader(ID3D11GeometryShader* p_d3d11GeometryShaderToAdd) { p_d3d11GeometryShader = &p_d3d11GeometryShaderToAdd; }

};

struct TRendererTextureSet
{
public:

	struct RESOURCE
	{
		enum
		{
			DIFFUSE = 0,
			NORMAL,
			EMISSIVE,
			SPECULAR,
			COUNT
		};
	};

private:

	ID3D11ShaderResourceView* p_d3d11ShaderResources[RESOURCE::COUNT];

public:

	/***************************************
	*	Accessors
	***************************************/
	inline ID3D11ShaderResourceView* GetResourceAt(int index) { return p_d3d11ShaderResources[index]; }
	/***************************************
	*	Mutators
	***************************************/
	inline void SetResourceAt(ID3D11ShaderResourceView* p_d3dShaderResourceToAdd, int index) { p_d3d11ShaderResources[index] = p_d3dShaderResourceToAdd; }

};

struct TRendererMesh
{

	ID3D11Buffer* p_d3dVertexBuffer = nullptr;
	ID3D11Buffer* p_d3dIndexBuffer;
	unsigned int numIndices;


};



struct TRenderer3DModel
{

	TRendererMesh tRendererMesh;
	TRendererShaderSet tRendererShaderSet;
	TRendererTextureSet tRendererTextureSet;
	ID3D11DeviceContext* p_d3dDeferredContext;
	ID3D11SamplerState* p_d3dSamplerState;

	struct
	{
		bool textureFlags[4];
	} tPixelConstBuffer;

	ID3D11Buffer* p_d3dVertexConstBuffer = nullptr;
	ID3D11Buffer* p_d3dPixelConstBuffer;


	DirectX::XMFLOAT3 d3dPosition;

	bool bRendereable;
	DirectX::XMMATRIX d3dModelMatrix;

	void initialize();


};













