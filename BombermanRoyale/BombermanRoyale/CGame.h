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
		GP_SPLASH,
		FS_SPLASH,
		DEV_LOGO,
		BATTLE_GAME,
		WIN_SCREEN,
		CONTROLS_SCREEN,
		PAUSE_MENU,
		CREDIT_SCREEN,
		OPTIONS_SCREEN
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
	std::vector<CObject*> deleteLayer;
	std::vector<CObject*> menuObjects;
	std::vector<CObject*> particleObjects;
	std::vector<CExplosion*> Xexplosions;
	std::vector<CExplosion*> Zexplosions;
	std::vector<CItem*> items;
	std::vector<double> explosionTimers;
	std::vector<CPlayer*> v_cPlayers = { nullptr, nullptr, nullptr, nullptr };
	std::vector<CPlayer*> PlayersInCustom = { nullptr, nullptr, nullptr, nullptr };
	std::vector<CPlayer*> v_cAI = { nullptr, nullptr };
	std::vector<CPlayer*> AiInCustom = { nullptr, nullptr };



	int maxNumBombs = 100;
	DirectX::XMMATRIX viewPos;
	DirectX::XMFLOAT3 bombPos;
	std::vector<CBomb*> v_cBombs;
	//CPlayer* menuBomb = nullptr;
	CharacterController menuController;
	int menuIndex = 0;
	int mapPasses = 4;

	double mouseIdleTimer;
	double menucontroltimer = 0.0;
	double AItime = 0.0;
	bool prevShowMouse = true;
	bool showMouse = true;
	bool bombExploded = false;
	bool SprinklersOn = false;
	bool isPaused = false;
	float CSx = -11.35f;


public:
	void Cleanup();
	bool FullScreen = false;
	unsigned int curGameState = 0;
	float BattleDuration = 300.0f;
	int HighScore = 0;
	int highscoreINDEX = 0;
	bool initialLoad = true;
	int mapsize = 2;
	int numAI = 0;
	int numPLAYERS = 2;
	int playermodel[4] = { 0,0,0,0 };
	int playertextures[4] = { DIFFUSE_TEXTURES::CHICKEN1, DIFFUSE_TEXTURES::CHICKEN2, DIFFUSE_TEXTURES::CHICKEN3, DIFFUSE_TEXTURES::CHICKEN4 };
	int AImodel[2] = { 4, 4 };
	int PlayerDiscIndex = 0;
	bool playerdisconnect = false;
	bool PlayerDisconnectToggle = false;
	bool AI_1_Moving = false;
	bool AI_1_STARTMoving = false;
	int prevGameState = 0;
	float gameStart = 5.0f;
	float menuTime = 0.0f;

	bool Initialize();
	void Run();

	void InitSortedParticles(end::sorted_pool_t<particle, 500>& sortedPool, double deltaTime, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color);
	void InitSortedParticles_vs_2(end::sorted_pool_t<particle, 500>& sortedPool, double deltaTime, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, DirectX::XMFLOAT4 direction);
	void InitFreeParticles(emitter& emitter, end::pool_t<particle, 1024>& freePool, double deltaTime);

	void LoadAnim();
	void LoadObjectBattle(int gmae_state);
	void LoadObjectSmall(int gmae_state);
	void LoadObjectMedium(int gmae_state);
	void LoadObjectLarge(int gmae_state);

	CGame();
	~CGame();

	void WindowResize();
	void GamePlayLoop(double timePassed);
	void setGameState(int _gameState);
	void ClearPlayersAndBombs();
	void updateBombs(double timePassed);
	bool loadTempMenus();
	void PlayerCollision(CPlayer* playerToCheck, CObject* cObj, float dx, float dz);
	void PlayerBombCollision(CPlayer* playerToCheck, CBomb* cBomb);
	void BombCollision(CObject* objectToCheck, CBomb* cBomb, CPlayer* playerToCheck);
	void AI_Method(double timepassed, double action_time);
	void CustomMeshUpdate(float timepassed);
	void moveAI(CPlayer* player, int direction, float deltaX, float deltaZ);
	void WallDrop(CObject* objectToCheck);
	void WallFlames(CObject* wall, float duration, int frames);
	void DeathTimerforRespawnUpdate(float timepassed);
	void MenuAnimation(int startDiffuseTexture, float duration, int frames, int loadstate);
	inline std::vector<CObject*> getMenuObjects() { return menuObjects; }
	inline bool GetisPaused() { return isPaused; }

	inline CPlayer* GetPlayer(int index) { if (index >= v_cPlayers.size())return nullptr; else return v_cPlayers.at(index); }
	inline CPlayer* GetAI(int index) { if (index >= v_cAI.size()) return nullptr; else return v_cAI.at(index); }
};

