#pragma once
#include "CObject.h"

class CRendererManager
{
public:
	bool HasComponent(CObject _objToCheck, int _componentToCheck);
	bool RenderObject(CObject _objToDraw);
	bool Draw();
	CRendererManager();
	~CRendererManager();
};

