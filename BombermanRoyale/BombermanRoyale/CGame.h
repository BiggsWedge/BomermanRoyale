#pragma once

#include "CRendererManager.h"
#include "CEntityManager.h"

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
	float p1BTimer = 0.0f, p2BTimer = 0.0f;
	int p1BIndex, p2BIndex;


public:
	bool FullScreen = false;
	unsigned int curGameState = 0;
	unsigned int width;
	unsigned int height;

	bool Initialize();
	void Run();

	
	void LoadObject();


	CGame();

	~CGame();
	void WindowResize();
	void GamePlayLoop();
};

