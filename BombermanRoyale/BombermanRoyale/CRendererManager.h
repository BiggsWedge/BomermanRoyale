#pragma once
#include "CPlayer.h"

class CRendererManager
{
private:
	std::vector<CObject*> rendereableObjects;
public:
	bool HasComponent(CObject _objToCheck, int _componentToCheck);
	void RenderObject(CObject* _objToDraw);
	bool Draw();
	CRendererManager();
	~CRendererManager();
};

