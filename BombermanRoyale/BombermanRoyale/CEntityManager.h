#pragma once

#include "CObject.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;

	ID3D11VertexShader* usedVertex;
	ID3D11PixelShader* usedPixel;
	ID3D11GeometryShader* usedGeo;
	ID3D11InputLayout* usedInput;

	ID3D11ShaderResourceView* usedDiffuse;

};


class CEntityManager
{
private:


public:

	CObject CreateOBJFromTemplate(OBJLoadInfo loadInfo);


};

