#pragma once

#include "Utilities.h"

struct COMPONENT_TYPE { enum { RENDERER = 0, MESH, TRANSFORM, TEXTURE, MATERIAL, COLLIDER }; };

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
	int iUsedVertexShaderIndex, iUsedPixelShaderIndex, iUsedGeometryShaderIndex, iUsedInputLayout, iUsedLoadState;
	TRendererComponent();
	TRendererComponent(int _usedVertexShader, int _usedPixelShader, int _usedInputLayout, int _usedGeometryShader, int _usedLoadState);
	~TRendererComponent();
};

struct TTransformComponent : TComponent
{
private:
public:
	DirectX::XMFLOAT3				fPosition;
	DirectX::XMFLOAT3				fForwardVector;
	DirectX::XMMATRIX				mObjMatrix;
	DirectX::XMFLOAT3				fScale;
	bool nFloor;
	bool destroyable;

	TTransformComponent();
	TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector, DirectX::XMFLOAT3 scale, bool floor, bool ndestroyable);
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
	bool hasCollider;
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

struct TColliderComponent : TComponent
{
private:
public:
	DirectX::BoundingBox d3dCollider;
	bool isStatic;
	int collisionLayer;
	TColliderComponent();
	TColliderComponent(TMeshTemplate mTemplate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 position, int layer);

	inline DirectX::BoundingBox GetCollider() { return d3dCollider; }
};








