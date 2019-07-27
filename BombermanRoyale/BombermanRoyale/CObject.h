#pragma once
#include "Component.h"


class CObject
{

	std::vector<TComponent*>			v_tComponents;
	ID3D11DeviceContext*			d3dDeferredContext;

public:
	CObject();
	inline std::vector<TComponent*> GetComponenets() { return v_tComponents; }
	inline void AddComponent(TComponent* _toAdd) { v_tComponents.push_back(_toAdd); }
	void Draw();
};




