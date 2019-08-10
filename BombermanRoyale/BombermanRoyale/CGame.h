#pragma once

#include "CRendererManager.h"
#include "CEntityManager.h"
#include "CObject.h"
#include "pools.h"
#include "Utilities.h"
#include "CPlayer.h"


struct GAME_STATE
{
	enum
	{
		MAIN_MENU = 0,
		ARCADE_MENU,
		BATTLE_MENU,
		ARCADE_GAME,
		BATTLE_GAME,
		WIN_SCREEN,
		CONTROLS_SCREEN,
		CREDIT_SCREEN
	};
};



class CGame
{

	CRendererManager* p_cRendererManager;
	CEntityManager* p_cEntityManager;
	std::vector<CObject*> objects;
	std::vector<CObject*> menuObjects;
	std::vector<CObject*> Xexplosions;
	std::vector<CObject*> Zexplosions;
	std::vector<double> explosionTimers;
	std::vector<CPlayer*> v_cPlayers = { nullptr, nullptr, nullptr, nullptr };

	//CPlayer *p1 = nullptr, *p2 = nullptr;

	int maxNumBombs = 24;
	std::vector<CBomb*> v_cBombs;

	double mouseIdleTimer;
	bool prevShowMouse = true;
	bool showMouse = true;


public:
	bool FullScreen = false;
	unsigned int curGameState = 0;
	unsigned int width;
	unsigned int height;

	bool Initialize();
	void Run();
	//void InitFreeParticles(emitter& emitter, pool_t<particle, 1024>& freePool, float deltaTime);
	//void InitSortedParticles(sorted_pool_t<particle, 1000>& sortedPool, float deltaTime);

	void LoadObject();
	void SpawnObject(int i, std::vector<CObject*> objects, CRendererManager* p_cRendererManager, CEntityManager* p_cEntityManager);

	CGame();

	~CGame();
	void WindowResize();
	void GamePlayLoop(double timePassed);
	//void ExplodeBomb(int bombToExplodeIndex);
	void setGameState(int _gameState);
	void ClearPlayersAndBombs();
	void updateBombs(double timePassed);
	void loadMap(int index);
	bool loadTempMenus();
};

