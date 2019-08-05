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

TTransformComponent::TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector, DirectX::XMFLOAT3 scale, bool floor)
{
	componentType = COMPONENT_TYPE::TRANSFORM;
	fPosition = spawnPosition;
	fForwardVector = forwardVector;
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR at = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&fForwardVector)));
	DirectX::XMVECTOR origin = { 0.0f, 0.0f, 0.0f };
	nFloor = floor;

	mObjMatrix = DirectX::XMMatrixLookAtLH(origin, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forwardVector)), DirectX::XMLoadFloat3(&up));
	mObjMatrix = mObjMatrix * DirectX::XMMatrixTranslation(spawnPosition.x, spawnPosition.y, spawnPosition.z);
	mObjMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * mObjMatrix;
}

TTransformComponent::~TTransformComponent()
{

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

}

TMaterialComponent::~TMaterialComponent()
{

}

#pragma endregion

