#pragma once

#include "Utilities.h"

struct COMPONENT_TYPE { enum { RENDERER = 0, MESH, TRANSFORM, TEXTURE, MATERIAL, ANIM, COLLIDER }; };

struct TComponent
{
protected:
	int componentType;
public:
	inline int GetComponentType() { return componentType; }
	virtual ~TComponent();
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
	int nLayer;

	TTransformComponent();
	TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector, DirectX::XMFLOAT3 scale, bool floor, bool ndestroyable, bool nitem, int nitemType, int layer);
	~TTransformComponent();
	void ResetMatrix();
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
	std::string mName;
	TMeshComponent();
	TMeshComponent(TMeshTemplate _template);
	~TMeshComponent();
};

struct TMaterialComponent : TComponent
{
private:
public:
	std::vector<file_path_t> filepaths;
	std::vector<material_t> mats;
	TMaterial _mat;
	enum TEXTURES { DIFFUSE = 0, EMISSIVE, SPECULAR, COUNT };
	ID3D11ShaderResourceView*	_srv[TEXTURES::COUNT];
	ID3D11Resource*				_textures[TEXTURES::COUNT];
	ID3D11SamplerState*			_samState;
	TMaterialComponent();
	TMaterialComponent(TMeshTemplate _template);
	~TMaterialComponent();
};

struct TAnimComponent : TComponent
{
private:
public:
	std::vector<AnimationClip*> animations;
	AnimationClip* currentAnimation;
	int currentFrameIndex;
	double _time;
	TAnimComponent();
	TAnimComponent(TMeshTemplate _template);
	~TAnimComponent();
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

