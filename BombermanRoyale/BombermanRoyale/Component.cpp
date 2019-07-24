#include "Component.h"


#pragma region TRenderer

TRendererComponent::TRendererComponent()
{
}

TRendererComponent::TRendererComponent(ID3D11VertexShader * _usedVertexShader, ID3D11PixelShader * _usedPixelShader, ID3D11GeometryShader * _usedGeometryShader, ID3D11InputLayout * _usedInputLayout)
{
	if (_usedVertexShader)
		p_d3dUsedVertexShader = &_usedVertexShader;
	if (_usedPixelShader)
		p_d3dUsedPixelShader = &_usedPixelShader;
	if (_usedGeometryShader)
		p_d3dUsedGeometryShader = &_usedGeometryShader;
	if (_usedInputLayout)
		p_d3dUsedInputLayout = &_usedInputLayout;
}

TRendererComponent::~TRendererComponent()
{
	p_d3dUsedVertexShader = nullptr;
	p_d3dUsedPixelShader = nullptr;
	p_d3dUsedGeometryShader = nullptr;
	p_d3dUsedInputLayout = nullptr;
}

#pragma endregion

#pragma region TTransform

TTransformComponent::TTransformComponent()
{

}

TTransformComponent::TTransformComponent(DirectX::XMFLOAT3 spawnPosition, DirectX::XMFLOAT3 forwardVector)
{
	fPosition = spawnPosition;
	fForwardVector = forwardVector;
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
	mObjMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&fPosition), DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&fForwardVector))), DirectX::XMLoadFloat3(&up));
}

TTransformComponent::~TTransformComponent()
{

}

#pragma endregion

#pragma region TTexture

TTextureComponent::TTextureComponent()
{

}

TTextureComponent::TTextureComponent(ID3D11ShaderResourceView* d3dUsedDiffuse, ID3D11ShaderResourceView* d3dUsedSpecular, ID3D11ShaderResourceView* d3dUsedEmissive, ID3D11ShaderResourceView* d3dUsedNormal)
{
	p_d3dUsedDiffuse = &d3dUsedDiffuse;
	p_d3dUsedSpecular = &d3dUsedSpecular;
	p_d3dUsedEmissive = &d3dUsedEmissive;
	p_d3dUsedNormal = &d3dUsedNormal;
}

TTextureComponent::~TTextureComponent()
{
	p_d3dUsedDiffuse = nullptr;
	p_d3dUsedEmissive = nullptr;
	p_d3dUsedNormal = nullptr;
	p_d3dUsedSpecular = nullptr;
}

#pragma endregion

#pragma region TMesh

TMeshComponent::TMeshComponent()
{

}

TMeshComponent::TMeshComponent(TMeshTemplate _template)
{
	indexCount = _template.v_iIndices.size();
	D3D11_BUFFER_DESC vBuffDesc;
	D3D11_SUBRESOURCE_DATA vBuffSub;
	ZeroMemory(&vBuffDesc, sizeof(vBuffDesc));
	ZeroMemory(&vBuffSub, sizeof(vBuffSub));

	vBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuffDesc.ByteWidth = sizeof(TSimpleVertex);
	vBuffDesc.CPUAccessFlags = 0;
	vBuffDesc.MiscFlags = 0;
	vBuffDesc.StructureByteStride = 0;
	vBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	vBuffSub.pSysMem = _template.v_tVertices.data();

	g_d3dData->d3dDevice->CreateBuffer(&vBuffDesc, &vBuffSub, &d3dVertexBuffer);

	D3D11_BUFFER_DESC iBuffDesc;
	D3D11_SUBRESOURCE_DATA iBuffSub;
	ZeroMemory(&iBuffDesc, sizeof(iBuffDesc));
	ZeroMemory(&iBuffSub, sizeof(iBuffSub));

	iBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuffDesc.ByteWidth = sizeof(int);
	iBuffDesc.CPUAccessFlags = 0;
	iBuffDesc.MiscFlags = 0;
	iBuffDesc.StructureByteStride = 0;
	iBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	iBuffSub.pSysMem = _template.v_iIndices.data();

	g_d3dData->d3dDevice->CreateBuffer(&iBuffDesc, &iBuffSub, &d3dIndexBuffer);

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

