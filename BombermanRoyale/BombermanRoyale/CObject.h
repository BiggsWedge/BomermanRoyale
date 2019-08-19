#pragma once
#include "Component.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::XMFLOAT3 scale;
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

	int currKeyFrame = 0;
	float frameTime = 0;
	float currFrameIndex = 0;
	float animTime = 0.0f;
	float totalTime;

	CObject();
	virtual ~CObject();
	inline std::vector<TComponent*> GetComponenets() { return v_tComponents; }
	inline void AddComponent(TComponent* _toAdd) { v_tComponents.push_back(_toAdd); }
	bool GetComponent(int componentType, TComponent* & component);
	virtual void Draw(double timepassed);
	
	bool Move(float _x, float _z);
	bool Collides(CObject* _other);
	virtual void Cleanup();
};




