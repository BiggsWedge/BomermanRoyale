#include "Component.h"


#pragma region TRenderer

TRendererComponent::TRendererComponent()
{
	componentType = COMPONENT_TYPE::RENDERER;
}

TRendererComponent::TRendererComponent(int _usedVertexShader, int _usedPixelShader, int _usedInputLayout, int _usedGeometryShader, int _usedLoadState)
{
	componentType = COMPONENT_TYPE::RENDERER;
	iUsedVertexShaderIndex = _usedVertexShader;
	iUsedPixelShaderIndex = _usedPixelShader;
	iUsedGeometryShaderIndex = _usedGeometryShader;
	iUsedInputLayout = _usedInputLayout;
	iUsedLoadState = _usedLoadState;
}

TRendererComponent::~TRendererComponent()
{

}

#pragma endregion

#pragma region TTransform

TTransformComponent::TTransformComponent()
{
	componentType = COMPONENT_TYPE::TRANSFORM;

}

TTransformComponent::TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector, DirectX::XMFLOAT3 scale, bool floor, bool ndestroyable, bool nitem, int nitemType, int layer)
{
	componentType = COMPONENT_TYPE::TRANSFORM;
	fPosition = spawnPosition;
	fScale = scale;
	fForwardVector = forwardVector;
	fScale = scale;
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR at = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&fForwardVector)));
	DirectX::XMVECTOR origin = { 0.0f, 0.0f, 0.0f };
	nFloor = floor;
	destroyable = ndestroyable;
	item = nitem;
	itemType = nitemType;
	nLayer = layer;
	mObjMatrix = DirectX::XMMatrixLookAtLH(origin, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forwardVector)), DirectX::XMLoadFloat3(&up));
	mObjMatrix = mObjMatrix * DirectX::XMMatrixTranslation(spawnPosition.x, spawnPosition.y, spawnPosition.z);
	//mObjMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * mObjMatrix;
}

TTransformComponent::~TTransformComponent()
{

}


void TTransformComponent::ResetMatrix()
{
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR at = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&fForwardVector)));
	DirectX::XMVECTOR origin = { 0.0f, 0.0f, 0.0f };

	mObjMatrix = DirectX::XMMatrixLookAtLH(origin, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&fForwardVector)), DirectX::XMLoadFloat3(&up));
	mObjMatrix = mObjMatrix * DirectX::XMMatrixTranslation(fPosition.x, fPosition.y, fPosition.z);
	mObjMatrix = DirectX::XMMatrixScaling(fScale.x, fScale.y, fScale.z) * mObjMatrix;

}

#pragma endregion

#pragma region TTexture

TTextureComponent::TTextureComponent()
{
	componentType = COMPONENT_TYPE::TEXTURE;

}

TTextureComponent::TTextureComponent(int _usedDiffuse, int _usedNormal)
{
	componentType = COMPONENT_TYPE::TEXTURE;
	iUsedDiffuseIndex = _usedDiffuse;
	iUsedNormalIndex = _usedNormal;
}

TTextureComponent::~TTextureComponent()
{

}

#pragma endregion

#pragma region TMesh

TMeshComponent::TMeshComponent()
{
	componentType = COMPONENT_TYPE::MESH;
}

TMeshComponent::TMeshComponent(TMeshTemplate _template)
{
	componentType = COMPONENT_TYPE::MESH;
	mName = _template.sName;

	indexCount = _template.v_iIndices.size();
	vertexCount = _template.v_tVertices.size();

	D3D11_BUFFER_DESC vBuffDesc;
	D3D11_SUBRESOURCE_DATA vBuffSub;
	ZeroMemory(&vBuffDesc, sizeof(vBuffDesc));
	ZeroMemory(&vBuffSub, sizeof(vBuffSub));

	vBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuffDesc.ByteWidth = sizeof(TSimpleVertex) * vertexCount;
	vBuffDesc.CPUAccessFlags = 0;
	vBuffDesc.MiscFlags = 0;
	vBuffDesc.StructureByteStride = 0;
	vBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	vBuffSub.pSysMem = _template.v_tVertices.data();
	HRESULT result;

	result = g_d3dData->d3dDevice->CreateBuffer(&vBuffDesc, &vBuffSub, &d3dVertexBuffer);

	D3D11_BUFFER_DESC iBuffDesc;
	D3D11_SUBRESOURCE_DATA iBuffSub;
	ZeroMemory(&iBuffDesc, sizeof(iBuffDesc));
	ZeroMemory(&iBuffSub, sizeof(iBuffSub));

	iBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuffDesc.ByteWidth = sizeof(int) * indexCount;
	iBuffDesc.CPUAccessFlags = 0;
	iBuffDesc.MiscFlags = 0;
	iBuffDesc.StructureByteStride = 0;
	iBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	iBuffSub.pSysMem = _template.v_iIndices.data();

	result = g_d3dData->d3dDevice->CreateBuffer(&iBuffDesc, &iBuffSub, &d3dIndexBuffer);
}

TMeshComponent::~TMeshComponent()
{
	SAFE_RELEASE(d3dVertexBuffer);
	SAFE_RELEASE(d3dIndexBuffer);
}

#pragma endregion

#pragma region TMaterial

TMaterialComponent::TMaterialComponent()
{
	componentType = COMPONENT_TYPE::MATERIAL;
}

TMaterialComponent::TMaterialComponent(TMeshTemplate _template)
{
	componentType = COMPONENT_TYPE::MATERIAL;
	filepaths = _template.filePaths;
	mats = _template.mats;
	_mat = _template._mat;
	_textures[TEXTURES::DIFFUSE] = _template._textures[TEXTURES::DIFFUSE];
	_textures[TEXTURES::EMISSIVE] = _template._textures[TEXTURES::EMISSIVE];
	_textures[TEXTURES::SPECULAR] = _template._textures[TEXTURES::SPECULAR];
	_samState = _template._samState;
	_srv[TEXTURES::DIFFUSE] = _template._srv[TEXTURES::DIFFUSE];
	_srv[TEXTURES::EMISSIVE] = _template._srv[TEXTURES::EMISSIVE];
	_srv[TEXTURES::SPECULAR] = _template._srv[TEXTURES::SPECULAR];
}



TMaterialComponent::~TMaterialComponent()
{

}

#pragma endregion


#pragma region TAnim

TAnimComponent::TAnimComponent()
{
	componentType = COMPONENT_TYPE::ANIM;
}

TAnimComponent::TAnimComponent(TMeshTemplate _template)
{

}

TAnimComponent::~TAnimComponent()
{

}

#pragma endregion


TColliderComponent::TColliderComponent()
{
	componentType = COMPONENT_TYPE::COLLIDER;
}

TColliderComponent::TColliderComponent(TMeshTemplate mtemplate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 position, int layer)
{
	componentType = COMPONENT_TYPE::COLLIDER;
	float top, bottom, left, right, front, back;
	collisionLayer = layer;

	top = bottom = mtemplate.v_tVertices[0].fPosition.y * scale.y;
	left = right = mtemplate.v_tVertices[0].fPosition.x * scale.y;
	front = back = mtemplate.v_tVertices[0].fPosition.z * scale.y;

	for (int i = 1; i < mtemplate.v_tVertices.size(); ++i)
	{
		left = (mtemplate.v_tVertices[i].fPosition.x * scale.x < left) ? mtemplate.v_tVertices[i].fPosition.x * scale.x : left;
		right = (mtemplate.v_tVertices[i].fPosition.x * scale.x > right) ? mtemplate.v_tVertices[i].fPosition.x * scale.x : right;
		top = (mtemplate.v_tVertices[i].fPosition.y * scale.y > top) ? mtemplate.v_tVertices[i].fPosition.y * scale.y : top;
		bottom = (mtemplate.v_tVertices[i].fPosition.y * scale.y < bottom) ? mtemplate.v_tVertices[i].fPosition.y * scale.y : bottom;
		front = (mtemplate.v_tVertices[i].fPosition.z * scale.z < front) ? mtemplate.v_tVertices[i].fPosition.z * scale.z : front;
		back = (mtemplate.v_tVertices[i].fPosition.z * scale.z > back) ? mtemplate.v_tVertices[i].fPosition.z* scale.z : back;
	}

	d3dCollider.Center = DirectX::XMFLOAT3(((left + right) / 2.0f) + position.x, ((top + bottom) / 2.0f) + position.y, ((front + back) / 2.0f) + position.z);
	//if (mtemplate.uID == MODELS::CHICKEN)
	//	d3dCollider.Extents = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	//else
	d3dCollider.Extents = DirectX::XMFLOAT3(abs((left + right) / 2.0f + left), abs((top + bottom) / 2.0f + top), abs((front + back) / 2.0f + front));
}

TComponent::~TComponent()
{

}
