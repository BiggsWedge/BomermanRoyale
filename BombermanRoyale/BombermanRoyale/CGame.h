#pragma once

#include "CRendererManager.h"
#include "CEntityManager.h"
#include "CObject.h"
#include "pools.h"
#include "Utilities.h"
#include "CPlayer.h"
#include "CItem.h"


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
		PAUSE_MENU,
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
	std::vector<CItem*> items;
	std::vector<double> explosionTimers;
	std::vector<CPlayer*> v_cPlayers = { nullptr, nullptr, nullptr, nullptr };
	std::vector<CPlayer*> v_cAI = { nullptr, nullptr, nullptr, nullptr };



//<<<<<<< HEAD
//	int maxNumBombs = 24;
//=======
	//CPlayer *p1 = nullptr, *p2 = nullptr;

	int maxNumBombs = 48;
//>>>>>>> origin/Month2-UI
	std::vector<CBomb*> v_cBombs;
	CPlayer* menuBomb = nullptr;
	int menuIndex = 0;

	double mouseIdleTimer;
	double menucontroltimer = 0.0;
	bool prevShowMouse = true;
	bool showMouse = true;


public:
	void Cleanup();
	bool FullScreen = false;
	unsigned int curGameState = 0;
	unsigned int width;
	unsigned int height;

	bool Initialize();
	void Run();
//<<<<<<< HEAD
//=======
	//void InitFreeParticles(emitter& emitter, pool_t<particle, 1024>& freePool, float deltaTime);
	//void InitSortedParticles(sorted_pool_t<particle, 1000>& sortedPool, float deltaTime);
	void LoadAnim();
//>>>>>>> origin/Month2-UI
	void LoadObject();

	CGame();
	~CGame();

	void WindowResize();
	void GamePlayLoop(double timePassed);
	void setGameState(int _gameState);
	void ClearPlayersAndBombs();
	void updateBombs(double timePassed);
	void loadMap(int index);
	bool loadTempMenus();
	void PlayerCollision(CPlayer* playerToCheck, CObject* cObj);
	void PlayerBombCollision(CPlayer * playerToCheck, CBomb* cBomb);
	void BombCollision(CObject* objectToCheck, CBomb* cBomb, CPlayer * playerToCheck);
	void BombBombCollision(CBomb* cBomb, CBomb* cBomb2);
};

