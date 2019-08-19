#include "CGame.h"
#include <DirectXCollision.h>
#include <iostream>
#include "pools.h"

#define PLAYER_SPEED 5.0f

const char* backgroundMusicFilePath = ".//Assets//Music//Level_Music1.wav";
const char* placeHolderSFX = ".//Assets//Music//snd_15186.wav";

struct key
{
	bool prevState = false;
	bool currState = false;

	inline bool pressed() { return currState && !prevState; }
	inline bool held() { return currState && prevState; }
	inline bool idle() { return !currState && !prevState; }
	inline bool released() { return !currState && prevState; }
};

struct CONTROL_KEYS
{
	enum { UP = 0, DOWN, LEFT, RIGHT, BOMB, COUNT };
};

struct KeyboardInput
{
	std::vector<key> controls;
	std::vector<int> keycodes;

	void updateKeys()
	{
		for (int i = 0; i < controls.size(); ++i)
		{
			controls[i].prevState = controls[i].currState;
			controls[i].currState = GetAsyncKeyState(keycodes[i]);
		}
	}

	key At(int index)
	{
		return controls[index];
	}
};

static std::vector<KeyboardInput> keyboardInputs;

struct KEYS
{
	enum { UP = 0, DOWN, LEFT, RIGHT, ZERO, RMB, SPACE, HELP_MENU, GAME_STATE, FULLSCREEN, PAUSE, COUNT };
};
static std::vector<key> keys(KEYS::COUNT);

static std::vector<int> keycodes =
{
	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,
	VK_NUMPAD0,
	VK_RBUTTON,
	VK_SPACE,
	VK_F1,
	'G',
	'F',
	VK_ESCAPE
};

bool ControlScreenToggle = false;
bool Controller1Alive = false;
bool Controller2Alive = false;
bool isPaused = false;

static double timePassed = 0.0f;
double tempTime = 0.0f;
double mapTime = 0.0f;
double offMapTimer = 0;
int boxDropped;
int currLayer = 0;
int passes = 0;
float fMinX = -15;
float fMaxX = 15;
float fMinZ = -10;
float fMaxZ = 15;
float bCollisionIgnore = 0.5f;

float isLDPADPressed = 0.0f;
float isRDPADPressed = 0.0f;
float isUDPADPressed = 0.0f;
float isDDPADPressed = 0.0f;
float isSouthButtonPressed = 0.0f;

float isP1StartButtonPressed = 0.0f;
float isP1SelectButtonPressed = 0.0f;
float isP2StartButtonPressed = 0.0f;
float isP2SelectButtonPressed = 0.0f;

float isP1LDPADPressed = 0.0f;
float isP1RDPADPressed = 0.0f;
float isP1UDPADPressed = 0.0f;
float isP1DDPADPressed = 0.0f;
float isP1SouthButtonPressed = 0.0f;

bool CGame::Initialize()
{
	keyboardInputs.resize(2);
	keyboardInputs[0].controls.resize(5);
	keyboardInputs[0].keycodes.resize(5);
	keyboardInputs[0].keycodes = { 'I', 'K', 'J', 'L', 'O' };

	keyboardInputs[1].controls.resize(5);
	keyboardInputs[1].keycodes.resize(5);
	keyboardInputs[1].keycodes = { VK_NUMPAD5, VK_NUMPAD2, VK_NUMPAD1, VK_NUMPAD3, VK_NUMPAD6 };

	v_cBombs.resize(maxNumBombs);
	p_cRendererManager = new CRendererManager();
	return true;
}

void CGame::Run()
{
	POINT currCursor, prevCursor;
	GetCursorPos(&currCursor);

#pragma region Audio

	if (G_SUCCESS(g_pAudioHolder->CreateMusicStream(backgroundMusicFilePath, &g_pMusicStream)))
	{
		if (G_SUCCESS(g_pMusicStream->SetVolume(0.5f)))
		{
			g_pMusicStream->StreamStart(true);
		}
	}

#pragma endregion

	float errorCode = 0;
	if (G_FAIL(g_pAudioHolder->CreateSound(placeHolderSFX, &g_pSoundPlayer)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}

	GW::SYSTEM::GWindowInputEvents gLastEvent;
	while (G_SUCCESS(g_pWindow->GetLastEvent(gLastEvent)) && gLastEvent != GW::SYSTEM::GWindowInputEvents::DESTROY)
	{
		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;

		static ULONGLONG currFrame = GetTickCount64();
		static ULONGLONG prevFrame = GetTickCount64();

		prevFrame = currFrame;
		currFrame = GetTickCount64();
		timePassed = (currFrame - prevFrame) / 1000.0;
		mapTime += timePassed;

		for (int i = 0; i < keycodes.size(); ++i)
		{
			keys[i].prevState = keys[i].currState;
			keys[i].currState = GetAsyncKeyState(keycodes[i]) & 0x8000;
		}
		for (int i = 0; i < CONTROL_KEYS::COUNT; ++i)
		{
			keyboardInputs[0].controls[i].prevState = keyboardInputs[0].controls[i].currState;
			keyboardInputs[1].controls[i].prevState = keyboardInputs[1].controls[i].currState;

			keyboardInputs[0].controls[i].currState = GetAsyncKeyState(keyboardInputs[0].keycodes[i]);
			keyboardInputs[1].controls[i].currState = GetAsyncKeyState(keyboardInputs[1].keycodes[i]);
		}
		if (keys[KEYS::FULLSCREEN].pressed())
		{
			FullScreen = !FullScreen;
			this->WindowResize();
		}
		if (keys[KEYS::HELP_MENU].pressed()) {
			ControlScreenToggle = !ControlScreenToggle;
			isPaused = !isPaused;
			if (isPaused == false)
			{
				g_pAudioHolder->ResumeAll();
				g_pMusicStream->ResumeStream();
				timePassed = tempTime;
			}
		}
		if (keys[KEYS::GAME_STATE].pressed())
		{
			if (curGameState == GAME_STATE::MAIN_MENU)
			{
				setGameState(GAME_STATE::ARCADE_GAME);
			}
			else if (curGameState == GAME_STATE::ARCADE_GAME)
			{
				setGameState(GAME_STATE::MAIN_MENU);
			}
			else if (curGameState == GAME_STATE::WIN_SCREEN)
			{
				setGameState(GAME_STATE::MAIN_MENU);
			}
		}
		if (keys[KEYS::PAUSE].pressed())
		{
			isPaused = !isPaused;
			ControlScreenToggle = !ControlScreenToggle;
			if (isPaused == false)
			{
				g_pAudioHolder->ResumeAll();
				g_pMusicStream->ResumeStream();
				timePassed = tempTime;
			}
		}
		if (isPaused == true)
		{
			g_pMusicStream->PauseStream();
			g_pAudioHolder->PauseAll();
			tempTime = timePassed;
			timePassed = 0;
		}
		if (curGameState == GAME_STATE::ARCADE_GAME)
		{
			updateBombs(timePassed);
			prevCursor = currCursor;
			GetCursorPos(&currCursor);

			g_d3dData->debugCamDelta = { 0.0f, 0.0f };
			if (keys[KEYS::ZERO].pressed())
				g_d3dData->ToggleUseDebugCamera();
			if (keys[KEYS::UP].held())
				g_d3dData->debugCamDelta.y += 0.1f;
			if (keys[KEYS::DOWN].held())
				g_d3dData->debugCamDelta.y += -0.1f;
			if (keys[KEYS::LEFT].held())
				g_d3dData->debugCamDelta.x += -0.1f;
			if (keys[KEYS::RIGHT].held())
				g_d3dData->debugCamDelta.x += 0.1f;

			if (keys[KEYS::RMB].held())
			{
				g_d3dData->debugCursorRot.y = 0.002f*(currCursor.y - prevCursor.y);
				g_d3dData->debugCursorRot.x = 0.002f*(currCursor.x - prevCursor.x);
			}

			static int numPlayersAlive = 0;
			static int prevNumPlayersAlive = 0;
			prevNumPlayersAlive = numPlayersAlive;
			numPlayersAlive = 0;
			for (int i = 0; i < v_cPlayers.size(); ++i)
			{
				if (v_cPlayers[i] && v_cPlayers[i]->isAlive())
					numPlayersAlive++;
			}

			if (prevNumPlayersAlive > 1 && numPlayersAlive <= 1)
			{
				setGameState(GAME_STATE::WIN_SCREEN);
			}

			this->GamePlayLoop(timePassed);
		}
		//RenderMenus
		for (CObject* menu : menuObjects)
		{
			TComponent* cRenderer;
			if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
				continue;
			TRendererComponent* renderer = (TRendererComponent*)cRenderer;
			if (renderer->iUsedLoadState == curGameState || (ControlScreenToggle && renderer->iUsedLoadState == GAME_STATE::CONTROLS_SCREEN))
				p_cRendererManager->RenderObject(menu);

		}

		//RenderObjects
		if (mapTime >= 55) {
			for (int i = 0; i < objects.size(); ++i) {
				if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::TRANSFORM)) {
					TComponent* cRenderer = nullptr;
					TComponent* texture = nullptr;
					TTransformComponent* renderer = nullptr;
					TTextureComponent* newTexture = nullptr;

					if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer)) {
						renderer = (TTransformComponent*)cRenderer;

						if (renderer->fPosition.x == fMinX || renderer->fPosition.z == fMinZ || renderer->fPosition.x == fMaxX || renderer->fPosition.z == fMaxZ) {
							objects[i]->GetComponent(COMPONENT_TYPE::TEXTURE, texture);
							newTexture = (TTextureComponent*)texture;
							newTexture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::FIRE_TEX;
						}
					}
				}
			}

			if (mapTime >= 60) {
				for (int passes = 0; passes < 6; passes++) {
					for (int i = 0; i < objects.size(); ++i) {
						if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::TRANSFORM)) {
							TComponent* cRenderer = nullptr;
							TTransformComponent* renderer = nullptr;

							if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer)) {
								renderer = (TTransformComponent*)cRenderer;

								if (renderer->fPosition.x == fMinX || renderer->fPosition.z == fMinZ || renderer->fPosition.x == fMaxX || renderer->fPosition.z == fMaxZ) {
									objects[i] = nullptr;

									for (CPlayer* player : v_cPlayers) {
										if (!player || !player->isAlive())
											continue;

										TComponent* _prenderer = nullptr;

										if (player->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
											TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

											if (pRenderer->fPosition.x == renderer->fPosition.x && pRenderer->fPosition.z == renderer->fPosition.z)
												player->setAlive(false);
										}
									}

									objects.erase(objects.begin() + i);
								}
							}
						}
					}
				}

				if (fMinX < -7.5) {
					fMinX += 2.5;
					fMinZ += 2.5;
					fMaxX -= 2.5;
					fMaxZ -= 2.5;
				}

				mapTime = 0;
			}
		}

		for (int i = 0; i < objects.size(); ++i) {
			if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER)) {
				TComponent* cRenderer = nullptr;
				TComponent* fRenderer = nullptr;
				TColliderComponent* floorRender = nullptr;
				TRendererComponent* renderer = nullptr;

				if (objects[i]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer)) {
					renderer = (TRendererComponent*)cRenderer;

					if (renderer->iUsedLoadState == curGameState)
						p_cRendererManager->RenderObject(objects[i]);
				}

				if (ControlScreenToggle == true) {
					if (objects[i]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer)) {
						renderer = (TRendererComponent*)cRenderer;

						if (renderer->iUsedLoadState == GAME_STATE::CONTROLS_SCREEN)
							p_cRendererManager->RenderObject(objects[i]);
					}
				}
			}
		}

		//RenderBombs
		for (CBomb* bomb : v_cBombs) {
			if (!bomb || !bomb->isAlive())
				continue;

			TComponent* renderer = nullptr;

			if (bomb->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
				TRendererComponent* tRenderer = (TRendererComponent*)renderer;

				if (tRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject((CObject*)bomb);
			}
		}

		//RenderExplosions
		for (int i = 0; i < explosionTimers.size(); ++i) {
			TComponent* renderer = nullptr;

			if (Xexplosions[i]->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
				TRendererComponent* tRenderer = (TRendererComponent*)renderer;
				if (tRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(Xexplosions[i]);
			}

			if (Zexplosions[i]->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
				TRendererComponent* tRenderer = (TRendererComponent*)renderer;
				if (tRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(Zexplosions[i]);
			}
		}

		//Render Item
		for (int i = 0; i < items.size(); i++) {
			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;

			if (items[i]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer)) {
				renderer = (TRendererComponent*)cRenderer;

				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(items[i]);
			}
		}

		//Render Players
		for (CPlayer* player : v_cPlayers) {
			if (!player || !player->isAlive())
				continue;

			TComponent* renderer = nullptr;

			if (player->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
				TRendererComponent* pRenderer = (TRendererComponent*)renderer;

				if (pRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject((CObject*)player);
			}
		}

		g_d3dData->updateCameras();

#pragma region Input
		if (g_pInputRecord->GetState(G_KEY_SPACE, errorCode) == 1) {
			std::cout << "SPACE WAS PRESSED, G INPUT STYLE";
		}

		if (keys[KEYS::SPACE].pressed()) {
			g_pSoundPlayer->Play();
		}

#pragma endregion
		if (GetAsyncKeyState('A')) {
			LoadLines();
		}

		if (!p_cRendererManager->Draw()) {
			g_pLogger->LogCatergorized("FAILURE", "Failed to draw");
		}
	}
}

void CGame::LoadObject() {
	OBJLoadInfo loadInfo;
	float minX, maxX, minZ, maxZ;
	minX = -12.5;
	maxX = 15;
	minZ = -7.5;
	maxZ = 15;

	for (float z = minZ; z < maxZ; z += 2.5f) {
		for (float x = minX; x < maxX; x += 2.5f) {
			loadInfo.position = { x, -2.5f, z };
			loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
			loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLUE_TEX;
			loadInfo.usedGeo = -1;
			loadInfo.usedVertex = VERTEX_SHADER::BASIC;
			loadInfo.usedPixel = PIXEL_SHADER::BASIC;
			loadInfo.usedInput = INPUT_LAYOUT::BASIC;
			loadInfo.meshID = MODELS::CUBE;
			loadInfo.LoadState = 3;
			loadInfo.floor = true;
			loadInfo.hasCollider = true;
			loadInfo.destroyable = false;
			loadInfo.collisionLayer = COLLISION_LAYERS::FLOOR;
			loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	for (float x = -7.5; x <= 7.5; x += 2.5f) {
		loadInfo.position = { x, 0.0f, 10 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.collisionLayer = COLLISION_LAYERS::DESTROYABLE;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, -5 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, 12.5 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, -7.5 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}
	
	for (float z = -2.5; z <= 7.5; z += 2.5f) {
		loadInfo.position = { -12.5, 0.0f, z };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.collisionLayer = COLLISION_LAYERS::DESTROYABLE;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { -10, 0.0f, z };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { 10, 0.0f, z };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.collisionLayer = COLLISION_LAYERS::DESTROYABLE;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.item = true;
		loadInfo.destroyable = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { 12.5, 0.0f, z };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.collisionLayer = COLLISION_LAYERS::DESTROYABLE;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = -2.5f; z < 10.0f; z += 2.5f) {
		for (float x = -7.5f; x < 10.0f; x += 2.5f) {
			if (rand() % 4 >= 1) {
				loadInfo.position = { x, 0.0f, z };
				loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
				loadInfo.meshID = MODELS::CUBE;
				loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
				loadInfo.usedVertex = VERTEX_SHADER::BASIC;
				loadInfo.usedPixel = PIXEL_SHADER::BASIC;
				loadInfo.usedInput = INPUT_LAYOUT::BASIC;
				loadInfo.usedGeo = -1;
				loadInfo.LoadState = 3;
				loadInfo.destroyable = true;
				loadInfo.collisionLayer = COLLISION_LAYERS::DESTROYABLE;
				loadInfo.item = true;
				loadInfo.floor = false;
				loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
				objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
			}
		}
	}

	for (float x = -15; x <= 15; x += 2.5f) {
		loadInfo.position = { x, 0, -10 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
		loadInfo.meshID = MODELS::CUBE;
		loadInfo.LoadState = 3;
		loadInfo.item = false;
		loadInfo.floor = false;
		loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
		loadInfo.destroyable = false;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0, 15 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
		loadInfo.meshID = MODELS::CUBE;
		loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = -7.5; z <= 12.5; z += 2.5f) {
		loadInfo.position = { -15, 0, z };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
		loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
		loadInfo.meshID = MODELS::CUBE;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { 15, 0, z };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
		loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
		loadInfo.meshID = MODELS::CUBE;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	loadInfo.position = { 5, 0, -5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5, 0, -5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0, 0, -5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 10, 0, 0 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -10, 0, 0 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 10, 0, 5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -10, 0, 5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0, 0, 2.5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 5, 0, 10 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5, 0, 10 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0, 0, 10 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

CGame::CGame() {
}

CGame::~CGame() {
	delete p_cRendererManager;
}


void CGame::WindowResize() {
	g_pWindow->GetClientWidth(width);
	g_pWindow->GetClientHeight(height);
	g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	GW::SYSTEM::GWindowStyle style = (FullScreen) ? GW::SYSTEM::GWindowStyle::FULLSCREENBORDERLESS : GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED;
	g_pWindow->ChangeWindowStyle(style);
	g_pWindow->GetClientWidth(width);
	g_pWindow->GetClientHeight(height);
	g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	g_d3dData->d3dViewport.Height = static_cast<float>(height);
	g_d3dData->d3dViewport.Width = static_cast<float>(width);
}

void CGame::GamePlayLoop(double timePassed) {
	for (CPlayer* currPlayer : v_cPlayers) {
		if (!currPlayer)
			continue;

		float isUpPressed = 0.0f, isDownPressed = 0.0f, isLeftPressed = 0.0f, isRightPressed = 0.0f, isSouthPressed = 0.0f;
		bool controllerConnected;

		g_pControllerInput->IsConnected(currPlayer->GetControllerIndex(), controllerConnected);

		if (controllerConnected) {
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_UP_BTN, isUpPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_DOWN_BTN, isDownPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_LEFT_BTN, isLeftPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_RIGHT_BTN, isRightPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_SOUTH_BTN, isSouthPressed);
		}

		bool canMove = true;
		float deltaX = 0.0f, deltaZ = 0.0f;

		TComponent* pComponent;
		currPlayer->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
		TTransformComponent* pTransform = (TTransformComponent*)pComponent;

		if (!currPlayer || !currPlayer->isAlive())
			continue;

		TComponent* col = nullptr;

		if (currPlayer->GetComponent(COMPONENT_TYPE::COLLIDER, col)) {
			TColliderComponent* tCol = (TColliderComponent*)col;
			DirectX::XMStoreFloat4(&tCol->d3dCollider.Orientation, DirectX::XMQuaternionRotationMatrix(pTransform->mObjMatrix));
		}

		if (isUpPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::UP).held())
			deltaZ += timePassed * PLAYER_SPEED;

		if (isDownPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::DOWN).held())
			deltaZ -= timePassed * PLAYER_SPEED;

		if (isLeftPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::LEFT).held())
			deltaX -= timePassed * PLAYER_SPEED;

		if (isRightPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::RIGHT).held())
			deltaX += timePassed * PLAYER_SPEED;

		if (deltaX != 0.0f || deltaZ != 0.0f)
			currPlayer->Move(deltaX, deltaZ);

		for (CObject* cObj : objects) {
			if (currPlayer->Collides(cObj))
				PlayerCollision(currPlayer, cObj);
		}

		for (CBomb* bomb : v_cBombs) {
			if (bomb && bomb->isAlive())
				if (currPlayer->Collides(bomb))
					PlayerBombCollision(currPlayer, bomb);
		}

		for (int i = 0; i < items.size(); i++) {
			if (currPlayer->Collides((CObject*)items[i])) {
				currPlayer->SetBombType(items[i]->GetItemType());

				if (currPlayer->GetNumBombs() <= 6) {
					currPlayer->incNumBombs();
				}

				items.erase(items.begin() + i);
				--i;
			}
		}

		TComponent* _prenderer = nullptr;

		if (currPlayer->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
			TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

			if (pRenderer->fPosition.x < fMinX - 1.5 || pRenderer->fPosition.x > fMaxX + 1.5 || pRenderer->fPosition.z < fMinZ - 1.5 || pRenderer->fPosition.z > fMaxZ + 1.5) {
				offMapTimer += timePassed;

				if (offMapTimer >= 0.5) {
					offMapTimer = 0;
					currPlayer->setAlive(false);
				}
			}
		}

		if (isSouthPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::BOMB).pressed()) {
			if (currPlayer->hasAvailableBombSlot()) {
				for (int i = 0; i < maxNumBombs; ++i) {
					if (v_cBombs[i] == nullptr || !v_cBombs[i]->isAlive()) {
						currPlayer->AddBombIndex(i);
						switch (currPlayer->GetBombType()) {
						case 4:
							if (v_cBombs[i])
								*v_cBombs[i] = *p_cEntityManager->DropBomb0(currPlayer);
							else
								v_cBombs[i] = p_cEntityManager->DropBomb0(currPlayer);
							break;
						case 1:
							if (v_cBombs[i])
								*v_cBombs[i] = *p_cEntityManager->DropBomb1(currPlayer);
							else
								v_cBombs[i] = p_cEntityManager->DropBomb1(currPlayer);
							break;
						case 2:
							if (v_cBombs[i])
								*v_cBombs[i] = *p_cEntityManager->DropBomb2(currPlayer);
							else
								v_cBombs[i] = p_cEntityManager->DropBomb2(currPlayer);
							break;
						case 3:
							if (v_cBombs[i])
								*v_cBombs[i] = *p_cEntityManager->DropBomb3(currPlayer);
							else
								v_cBombs[i] = p_cEntityManager->DropBomb3(currPlayer);
							break;
						case 0:
							if (v_cBombs[i])
								*v_cBombs[i] = *p_cEntityManager->DropBomb(currPlayer);
							else
								v_cBombs[i] = p_cEntityManager->DropBomb(currPlayer);
							break;
						}

						break;
					}
				}
			}
		}
	}
}

void CGame::setGameState(int _gameState) {
	switch (_gameState) {
	case GAME_STATE::MAIN_MENU:
		ClearPlayersAndBombs();
		break;
	case GAME_STATE::ARCADE_GAME:
		curGameState = 3;
		LoadObject();
		v_cPlayers[0] = p_cEntityManager->InstantiatePlayer(1, DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(-10.0f, 0.0f, 10.0f));
		v_cPlayers[1] = p_cEntityManager->InstantiatePlayer(2, DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(10.0f, 0.0f, -5.0f));
		fMinX = -15;
		fMaxX = 15;
		fMinZ = -10;
		fMaxZ = 15;
		mapTime = 0;
		break;
	case GAME_STATE::WIN_SCREEN:
		ClearPlayersAndBombs();
		break;
	}

	curGameState = _gameState;
}

void CGame::ClearPlayersAndBombs() {
	for (int i = 0; i < v_cPlayers.size(); ++i) {
		if (v_cPlayers[i]) {
			delete v_cPlayers[i];
			v_cPlayers[i] = nullptr;
		}
	}

	v_cBombs.clear();
	v_cBombs.resize(maxNumBombs);
	Xexplosions.clear();
	Zexplosions.clear();
	explosionTimers.clear();
	objects.clear();
	items.clear();
}

void CGame::updateBombs(double timePassed) {
	for (int i = 0; i < explosionTimers.size(); ++i) {
		if (explosionTimers[i] >= 0.5f) {
			Xexplosions.erase(Xexplosions.begin() + i);
			Zexplosions.erase(Zexplosions.begin() + i);
			explosionTimers.erase(explosionTimers.begin() + i);
			--i;
			continue;
		}

		for (int j = 0; j < objects.size(); j++) {
			TComponent* obj = nullptr;
			TTransformComponent* objTrans;
			objTrans = nullptr;

			if (objects[j]->GetComponent(COMPONENT_TYPE::TRANSFORM, obj)) {
				objTrans = (TTransformComponent*)obj;
			}

			if (Xexplosions[i]->Collides(objects[j]) || Zexplosions[i]->Collides(objects[j])) {
				if (objTrans->item) {
					switch (rand() % 4) {
					case 0:
						items.push_back(p_cEntityManager->ItemDrop(objects[j], 1));
						objects.erase(objects.begin() + j);
						--j;
						break;
					case 1:
						items.push_back(p_cEntityManager->ItemDrop(objects[j], 2));
						objects.erase(objects.begin() + j);
						--j;
						break;
					case 2:
						items.push_back(p_cEntityManager->ItemDrop(objects[j], 3));
						objects.erase(objects.begin() + j);
						--j;
						break;
					case 3:
						items.push_back(p_cEntityManager->ItemDrop(objects[j], 4));
						objects.erase(objects.begin() + j);
						--j;
						break;
					}
				}
			}
		}

		for (CPlayer* player : v_cPlayers) {
			if (player) {
				if (Xexplosions[i]->Collides((CObject*)player) || Zexplosions[i]->Collides((CObject*)player))
					player->setAlive(false);
			}

		}

		for (CBomb* bomb : v_cBombs) {
			if (bomb && bomb->isAlive()) {
				if (Xexplosions[i]->Collides((CObject*)bomb) || Zexplosions[i]->Collides((CObject*)bomb)) {
					bomb->Explode();
				}
			}
		}

		explosionTimers[i] += timePassed;
	}

	for (int i = 0; i < v_cBombs.size(); ++i) {
		if (!v_cBombs[i])
			continue;

		if (v_cBombs[i]->isAlive()) {
			if (v_cBombs[i]->shouldExplode()) {
				v_cBombs[i]->Explode();
				CPlayer* parent = v_cBombs[i]->getParent();
				for (int j = 0; j < parent->getBombIndices().size(); ++j) {
					if (parent->getBombIndices()[j] == i)
						parent->deleteBomb(j);
				}

				explosionTimers.push_back(0.0f);
				Xexplosions.push_back(p_cEntityManager->BombExplosionX(v_cBombs[i]));
				Zexplosions.push_back(p_cEntityManager->BombExplosionZ(v_cBombs[i]));
				g_pSoundPlayer->Play();
				v_cBombs[i]->SetAlive(false);
			}

			v_cBombs[i]->updateBomb(timePassed);
		}
	}
}

void CGame::loadMap(int index) {
}

bool CGame::loadTempMenus() {
	OBJLoadInfo loadInfo;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;

	loadInfo.position = { 0.0f, 2.5f, 20.0f };
	loadInfo.forwardVec = { 0.0f, 1.1f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::NAMES_HUD;
	loadInfo.hasCollider = false;
	loadInfo.scale = DirectX::XMFLOAT3(2.4f, 0.25f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::ARCADE_GAME;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 5.4f, -5.0f };
	loadInfo.forwardVec = { 0.0f, 1.1f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::MAIN_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(2.6f, 2.1f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::MAIN_MENU;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 5.4f, -5.0f };
	loadInfo.forwardVec = { 0.0f, 1.1f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::WIN_SCREEN;
	loadInfo.scale = DirectX::XMFLOAT3(2.6f, 2.1f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::WIN_SCREEN;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 12.0f, -10.5f };
	loadInfo.forwardVec = { 0.0f, 1.1f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HELP_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(0.8f, 1.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU2;
	loadInfo.LoadState = GAME_STATE::CONTROLS_SCREEN;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	return true;
}

void CGame::PlayerCollision(CPlayer * playerToCheck, CObject* cObj) {
	TComponent* comp = nullptr;
	TColliderComponent* pCollider;
	playerToCheck->GetComponent(COMPONENT_TYPE::COLLIDER, comp);
	pCollider = (TColliderComponent*)comp;

	if (!cObj->GetComponent(COMPONENT_TYPE::COLLIDER, comp))
		return;

	TColliderComponent* objCollider = (TColliderComponent*)comp;

	float xD, zD, mX, mZ;
	int leftRight, upDown;

	if (pCollider->d3dCollider.Center.x > objCollider->d3dCollider.Center.x)
		leftRight = 1;
	else
		leftRight = -1;

	if (pCollider->d3dCollider.Center.z > objCollider->d3dCollider.Center.z)
		upDown = 1;
	else
		upDown = -1;

	xD = abs(pCollider->d3dCollider.Center.x - objCollider->d3dCollider.Center.x);
	zD = abs(pCollider->d3dCollider.Center.z - objCollider->d3dCollider.Center.z);
	mX = pCollider->d3dCollider.Extents.x + objCollider->d3dCollider.Extents.x;
	mZ = pCollider->d3dCollider.Extents.z + objCollider->d3dCollider.Extents.z;

	if (zD < mZ && zD > mZ - 0.25) {
		playerToCheck->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	} else if (xD < mX && xD > mX - 0.25) {
		playerToCheck->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
	}
}

void CGame::PlayerBombCollision(CPlayer * playerToCheck, CBomb* cBomb) {
	TComponent* comp = nullptr;
	TColliderComponent* pCollider;
	playerToCheck->GetComponent(COMPONENT_TYPE::COLLIDER, comp);
	pCollider = (TColliderComponent*)comp;

	if (!cBomb->GetComponent(COMPONENT_TYPE::COLLIDER, comp))
		return;

	TColliderComponent* objCollider = (TColliderComponent*)comp;
	TTransformComponent* bombTransform = (TTransformComponent*)comp;

	float xD, zD, mX, mZ;
	int leftRight, upDown;

	if (pCollider->d3dCollider.Center.x < objCollider->d3dCollider.Center.x)
		leftRight = 1;
	else
		leftRight = -1;

	if (pCollider->d3dCollider.Center.z < objCollider->d3dCollider.Center.z)
		upDown = 1;
	else
		upDown = -1;

	xD = abs(pCollider->d3dCollider.Center.x - objCollider->d3dCollider.Center.x);
	zD = abs(pCollider->d3dCollider.Center.z - objCollider->d3dCollider.Center.z);
	mX = pCollider->d3dCollider.Extents.x + objCollider->d3dCollider.Extents.x;
	mZ = pCollider->d3dCollider.Extents.z + objCollider->d3dCollider.Extents.z;

	if (zD < mZ && zD > mZ - 0.25) {
		if ()
		{

		}
		cBomb->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	} else if (xD < mX && xD > mX - 0.25) {
		cBomb->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
	}
}

void CGame::BombCollision(CObject* objectToCheck, CBomb* cBomb) {
	TComponent* comp = nullptr;
	TColliderComponent* pCollider;
	objectToCheck->GetComponent(COMPONENT_TYPE::COLLIDER, comp);
	pCollider = (TColliderComponent*)comp;

	if (!cBomb->GetComponent(COMPONENT_TYPE::COLLIDER, comp))
		return;

	TColliderComponent* objCollider = (TColliderComponent*)comp;
	TTransformComponent* bombTransform = (TTransformComponent*)comp;

	float xD, zD, mX, mZ;
	int leftRight, upDown;

	if (pCollider->d3dCollider.Center.x < objCollider->d3dCollider.Center.x)
		leftRight = 1;
	else
		leftRight = -1;

	if (pCollider->d3dCollider.Center.z < objCollider->d3dCollider.Center.z)
		upDown = 1;
	else
		upDown = -1;

	xD = abs(pCollider->d3dCollider.Center.x - objCollider->d3dCollider.Center.x);
	zD = abs(pCollider->d3dCollider.Center.z - objCollider->d3dCollider.Center.z);
	mX = pCollider->d3dCollider.Extents.x + objCollider->d3dCollider.Extents.x;
	mZ = pCollider->d3dCollider.Extents.z + objCollider->d3dCollider.Extents.z;

	if (zD < mZ && zD > mZ - 0.25) {
		cBomb->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	}
	else if (xD < mX && xD > mX - 0.25) {
		cBomb->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
	}
}