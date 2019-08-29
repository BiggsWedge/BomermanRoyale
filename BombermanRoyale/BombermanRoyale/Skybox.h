#pragma once
#include "Utilities.h"

class Skybox
{
private:

	ID3D11Buffer*							d3dVertexBuffer;
	ID3D11Buffer*							d3dIndexBuffer;
	ID3D11Buffer*							d3dConstantBuffer;
	ID3D11VertexShader*						d3dVertexShader;
	ID3D11PixelShader*						d3dPixelShader;
	ID3D11InputLayout*						d3dInputLayout;
	unsigned int							uNumIndices;
	ID3D11ShaderResourceView**				p_d3dUsedTexture;
	ID3D11SamplerState**					p_d3dUsedSampler;
	DirectX::XMFLOAT3						fPosition;
	ID3D11CommandList*						d3dCommandList;
	TBasicVertexConstBuff					tConstantBuffer;

public:
	Skybox();
	~Skybox();
	void Cleanup();
	void Initialize();
	void Update();
	void Render();
};

