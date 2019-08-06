#pragma once
#include "Component.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::XMFLOAT3 scale;
	TCollider collider;
	int usedVertex, usedPixel, usedGeo, usedInput, usedDiffuse, LoadState;
	bool floor;
};

class CObject
{

	std::vector<TComponent*>			v_tComponents;
	ID3D11DeviceContext*			d3dDeferredContext;

public:
	CObject();
	inline std::vector<TComponent*> GetComponenets() { return v_tComponents; }
	inline void AddComponent(TComponent* _toAdd) { v_tComponents.push_back(_toAdd); }
	bool GetComponent(int componentType, TComponent* & component);
	void Draw();
	bool Move(float _x, float _z);

};




