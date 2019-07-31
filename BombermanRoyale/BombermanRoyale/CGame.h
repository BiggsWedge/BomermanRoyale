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

