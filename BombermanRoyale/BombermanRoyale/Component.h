#pragma once

#include "Utilities.h"

struct COMPONENT_TYPE { enum { RENDERER = 0, MESH, TRANSFORM, TEXTURE, MATERIAL, ANIM }; };

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
	TCollider mCollider;
	bool hasCollider;
	std::string mName;
	TMeshComponent();
	TMeshComponent(TMeshTemplate _template, TCollider tCollider, bool noCollider);
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
	std::vector<joint> _bindPose;
	AnimationClip _anim;
	TAnimComponent();
	TAnimComponent(TMeshTemplate _template);
	~TAnimComponent();
};








