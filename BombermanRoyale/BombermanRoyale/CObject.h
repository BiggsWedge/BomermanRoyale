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
	int layer;
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
	float explosiontime = 0.0f;
	float totalTime;
	float movetimer = 0.0f;
	bool moving = false;

	CObject();
	virtual ~CObject();
	inline std::vector<TComponent*> GetComponenets() { return v_tComponents; }
	inline void AddComponent(TComponent* _toAdd) { v_tComponents.push_back(_toAdd); }
	bool GetComponent(int componentType, TComponent* & component);
	virtual void Draw(double timepassed);

	bool TurnPlayerTo(float _x, float _z);
	bool Move(float _x, float _z, bool rotation = true);
	bool CrouchRoll(float _x, float _z, float _y, bool rotation = true);
	bool Collides(CObject* _other);
	bool MoveOverTime(float start_x, float end_x, float start_z, float end_z, float timepassed, bool rotation = true);
	virtual void Cleanup();

};




