#pragma once

#include "CObject.h"
#include <DirectXCollision.h>

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::BoundingBox Collider;
	int usedVertex, usedPixel, usedGeo, usedInput, usedDiffuse;

};

class CEntityManager
{
private:

public:
	CObject* CreateOBJFromTemplate(OBJLoadInfo loadInfo);

};

