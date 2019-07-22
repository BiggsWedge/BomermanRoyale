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
	int componentType = COMPONENT_TYPE::RENDERER;
public:
	ID3D11VertexShader**			p_d3dUsedVertexShader;
	ID3D11PixelShader**				p_d3dUsedPixelShader;
	ID3D11GeometryShader**			p_d3dUsedGeometryShader;
	ID3D11InputLayout**				p_d3dUsedInputLayout;
	TRendererComponent();
	TRendererComponent(ID3D11VertexShader* _usedVertexShader, ID3D11PixelShader* _usedPixelShader, ID3D11GeometryShader* _usedGeometryShader, ID3D11InputLayout* _usedInputLayout);
	~TRendererComponent();
};

struct TTransformComponent : TComponent
{
private:
	int componentType = COMPONENT_TYPE::TRANSFORM;
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
	int componentType = COMPONENT_TYPE::TEXTURE;
public:
	ID3D11ShaderResourceView**			p_d3dUsedDiffuse;
	ID3D11ShaderResourceView**			p_d3dUsedSpecular;
	ID3D11ShaderResourceView**			p_d3dUsedEmissive;
	ID3D11ShaderResourceView**			p_d3dUsedNormal;

	TTextureComponent();
	TTextureComponent(ID3D11ShaderResourceView* d3dUsedDiffuse, ID3D11ShaderResourceView* d3dUsedSpecular, ID3D11ShaderResourceView* d3dUsedEmissive, ID3D11ShaderResourceView* d3dUsedNormal);
	~TTextureComponent();
};

struct TMeshComponent : TComponent
{
private:
	int componentType = COMPONENT_TYPE::MESH;
public:
	UINT indexCount;
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






