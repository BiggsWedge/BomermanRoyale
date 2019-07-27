#pragma once

#include "CRendererManager.h";
#include "CEntityManager.h"

class CGame
{

	CRendererManager* p_cRendererManager;
	CEntityManager* p_cEntityManager;
	std::vector<CObject*> objects;


public:
	bool Initialize();
	void Run();


	void LoadObject();


	CGame();

	~CGame();
};

