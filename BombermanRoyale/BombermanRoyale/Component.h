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
	bool item;
	int itemType;

	TTransformComponent();
	TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector, DirectX::XMFLOAT3 scale, bool floor, bool ndestroyable, bool nitem, int nitemType);
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
	TCollider mCollider;
	bool hasCollider;
	TMeshComponent();
	TMeshComponent(TMeshTemplate _template, TCollider tCollider, bool noCollider);
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
	DirectX::BoundingOrientedBox d3dCollider;
	bool isStatic;
	DirectX::XMFLOAT3 TFL, TFR, TBL, TBR, BFL, BFR, BBL, BBR;
	int collisionLayer;
	TColliderComponent();
	TColliderComponent(TMeshTemplate mTemplate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 position, int layer);

	inline DirectX::BoundingOrientedBox GetCollider() { return d3dCollider; }
};








