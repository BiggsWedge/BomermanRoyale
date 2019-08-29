#pragma once

#include "CRendererManager.h"
#include "CEntityManager.h"
#include "CObject.h"
#include "pools.h"
#include "XTime.h"
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

struct particle
{
	float timer;
	float3 pos;
	float3 prev_pos;
	float3 speed;
	float4 color;
};

struct emitter
{
	float3 spawn_pos;
	float4 spawn_color;
	// indices into the shared_pool 
	end::sorted_pool_t<int16_t, 256> indices;
};

class CGame
{
	float temp1 = 0;
	float temp2 = 0;
	float particleSpeed = 0.5f;
	float particleGravity = 9.8f;
	end::sorted_pool_t<particle, 1000> sortedParticles;
	end::pool_t<particle, 1000> freeParticles;
	end::pool_t<particle, 1024> shared_pool;
	emitter firstEmit;
	emitter secondEmit;
	emitter thirdEmit;
	XTime timer;

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




	//int maxNumBombs = 24;
	//CPlayer *p1 = nullptr, *p2 = nullptr;

	int maxNumBombs = 48;
	DirectX::XMMATRIX viewPos;
	std::vector<CBomb*> v_cBombs;
	CPlayer* menuBomb = nullptr;
	int menuIndex = 0;

	double mouseIdleTimer;
	double menucontroltimer = 0.0;
	bool prevShowMouse = true;
	bool showMouse = true;
	bool bombExploded = false;


public:
	void Cleanup();
	bool FullScreen = false;
	unsigned int curGameState = 0;
	unsigned int width;
	unsigned int height;

	bool Initialize();
	void Run();

	void InitFreeParticles(emitter& emitter, end::pool_t<particle, 1024>& freePool, float deltaTime);
	void InitSortedParticles(end::sorted_pool_t<particle, 1000>& sortedPool, float deltaTime);
	void LoadAnim();
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
};

