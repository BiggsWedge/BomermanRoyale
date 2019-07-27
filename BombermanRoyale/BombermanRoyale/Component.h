#pragma once

#include "Utilities.h"

struct COMPONENT_TYPE { enum { RENDERER = 0, MESH, TRANSFORM, TEXTURE, MATERIAL }; };

struct TComponent
{
protected:
	int componentType;
public:
	inline int GetComponentType() { return componentType; }

};

struct TRendererComponent : TComponent
{
private:
public:
	int iUsedVertexShaderIndex, iUsedPixelShaderIndex, iUsedGeometryShaderIndex, iUsedInputLayout;
	TRendererComponent();
	TRendererComponent(int _usedVertexShader, int _usedPixelShader, int _usedInputLayout, int _usedGeometryShader = -1);
	~TRendererComponent();
};

struct TTransformComponent : TComponent
{
private:
public:
	DirectX::XMFLOAT3				fPosition;
	DirectX::XMFLOAT3				fForwardVector;
	DirectX::XMMATRIX				mObjMatrix;

	TTransformComponent();
	TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector);
	~TTransformComponent();
};

struct TTextureComponent : TComponent
{
private:
public:
	int iUsedDiffuseIndex, iUsedNormalIndex;

	TTextureComponent();
	TTextureComponent(int _usedDiffuse, int _usedNormal = -1);
	~TTextureComponent();
};

struct TMeshComponent : TComponent
{
private:
public:
	UINT indexCount;
	UINT vertexCount;
	ID3D11Buffer*		d3dVertexBuffer;
	ID3D11Buffer*		d3dIndexBuffer;
	TMeshComponent();
	TMeshComponent(TMeshTemplate _template);
	~TMeshComponent();
};

struct TMaterialComponent : TComponent
{
private:
	int componentType = COMPONENT_TYPE::MATERIAL;
public:
	TMaterialComponent();
	~TMaterialComponent();
};






