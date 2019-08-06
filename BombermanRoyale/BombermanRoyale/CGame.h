#pragma once

#include "CRendererManager.h"
#include "CEntityManager.h"
#include "pools.h"
#include "Utilities.h"

enum GameState
{
	MAIN_MENU = 0,
	ARCADE_MENU,
	BATTLE_MENU,
	ARCADE_GAME,
	BATTLE_GAME,
	WIN_SCREEN,
	CREDIT_SCREEN

};

class CGame
{

	CRendererManager* p_cRendererManager;
	CEntityManager* p_cEntityManager;
	std::vector<CObject*> objects;
	CObject* p2, *p1;
	CObject* p1B = nullptr, *p2B = nullptr;
	CObject* p1Ex = nullptr, *p1Ez = nullptr,*p2Ex = nullptr, *p2Ez = nullptr;
	float p1BTimer = 0.0f, p2BTimer = 0.0f;
	float p1ETimer = 0.0f, p2ETimer = 0.0f;
	int p1BIndex, p2BIndex;
	bool p1Move = true;
	bool p2Move = true;


public:
	bool FullScreen = false;
	unsigned int curGameState = 0;
	unsigned int width;
	unsigned int height;

	bool Initialize();
	void Run();
	void InitFreeParticles(emitter& emitter, pool_t<particle, 1024>& freePool, float deltaTime);
	void InitSortedParticles(sorted_pool_t<particle, 1000>& sortedPool, float deltaTime);
	
	void LoadObject();


	CGame();

	~CGame();
	void WindowResize();
	void GamePlayLoop();
};

