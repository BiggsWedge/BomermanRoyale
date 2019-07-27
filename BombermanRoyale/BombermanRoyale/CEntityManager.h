#pragma once

#include "CObject.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::XMFLOAT3 scale;

	int usedVertex, usedPixel, usedGeo, usedInput, usedDiffuse;

};

class CEntityManager
{
private:

public:
	CObject* CreateOBJFromTemplate(OBJLoadInfo loadInfo);

};

