#pragma once
#include "Component.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::XMFLOAT3 scale;
	TCollider collider;
	bool hasCollider;
	int usedVertex, usedPixel, usedGeo, usedInput, usedDiffuse, LoadState;
	bool floor;
	bool destroyable;
	bool item;
	int collisionLayer;
	int itemType;
};

class CObject
{
protected:
	std::vector<TComponent*>			v_tComponents;
	ID3D11DeviceContext*			d3dDeferredContext;

public:
	CObject();
	inline std::vector<TComponent*> GetComponenets() { return v_tComponents; }
	inline void AddComponent(TComponent* _toAdd) { v_tComponents.push_back(_toAdd); }
	bool GetComponent(int componentType, TComponent* & component);
	void Draw();
	bool Move(float _x, float _z);
	bool Collides(CObject* _other);

};




