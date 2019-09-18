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
		CHARACTER_SCREEN,
		LOAD_SCREEN,
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
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 prev_pos;
	DirectX::XMFLOAT3 speed;
	DirectX::XMFLOAT4 color;
};

struct emitter
{
	DirectX::XMFLOAT3 spawn_pos;
	DirectX::XMFLOAT4 spawn_color;
	// indices into the shared_pool 
	end::sorted_pool_t<int16_t, 256> indices;
};

class CGame
{
	float particleLife = 5.0f;
	float particleSpeed = 1.2f;
	float particleGravity = 9.8f;
	end::sorted_pool_t<particle, 500> sortedParticles;
	end::pool_t<particle, 500> freeParticles;
	end::pool_t<particle, 1024> shared_pool;
	emitter firstEmit;
	emitter secondEmit;
	emitter thirdEmit;
	emitter fourthEmit;
	emitter freeEmit;
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
	std::vector<CPlayer*> PlayersInCustom = { nullptr, nullptr, nullptr, nullptr };
	std::vector<CPlayer*> v_cAI = { nullptr, nullptr };
	std::vector<CPlayer*> AiInCustom = { nullptr, nullptr};



	int maxNumBombs = 48;
	DirectX::XMMATRIX viewPos;
	DirectX::XMFLOAT3 bombPos;
	std::vector<CBomb*> v_cBombs;
	CPlayer* menuBomb = nullptr;
	int menuIndex = 0;

	double mouseIdleTimer;
	double menucontroltimer = 0.0;
	double AItime = 0.0;
	bool prevShowMouse = true;
	bool showMouse = true;
	bool bombExploded = false;
	bool SprinklersOn = false;
	float CSx = -11.35f;


public:
	void Cleanup();
	bool FullScreen = false;
	unsigned int curGameState = 0;
	int mapsize = 2;
	int numAI = 0;
	int numPLAYERS = 2;
	int playermodel[4] = { 4, 4, 4, 4 };
	int AImodel[2] = { 4, 4 };

	bool Initialize();
	void Run();

	void InitSortedParticles(end::sorted_pool_t<particle, 500>& sortedPool, double deltaTime, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color);
	void InitFreeParticles(emitter& emitter, end::pool_t<particle, 1024>& freePool, double deltaTime);
	void InitFreeParticles(emitter& emitter, end::pool_t<particle, 1024>& freePool, double deltaTime, CObject* obj);
	void SpawnParticles(CObject* obj, double time, double timePassed);

	void LoadAnim();
	void LoadObjectBattle();
	void LoadObjectSmall();
	void LoadObjectMedium();
	void LoadObjectLarge();

	CGame();
	~CGame();

	void WindowResize();
	void GamePlayLoop(double timePassed);
	void setGameState(int _gameState);
	void ClearPlayersAndBombs();
	void updateBombs(double timePassed);
	void loadMap(int index);
	bool loadTempMenus();
	void PlayerCollision(CPlayer* playerToCheck, CObject* cObj, float dx, float dz);
	void PlayerBombCollision(CPlayer * playerToCheck, CBomb* cBomb);
	void BombCollision(CObject* objectToCheck, CBomb* cBomb, CPlayer * playerToCheck);
	void AI_Method(double timepassed, double action_time);
	void CustomMeshUpdate();

	inline CPlayer* GetPlayer(int index) { if (index >= v_cPlayers.size())return nullptr; else return v_cPlayers.at(index); }
	inline CPlayer* GetAI(int index) { if (index >= v_cAI.size()) return nullptr; else return v_cAI.at(index); }
};

