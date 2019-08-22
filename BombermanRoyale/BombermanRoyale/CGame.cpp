#include "CGame.h"
#include <DirectXCollision.h>
#include <iostream>
#include "pools.h"

#define PLAYER_SPEED 5.0f

const char* backgroundMusicFilePath = ".//Assets//Music//Level_Music1.wav";
const char* placeHolderSFX = ".//Assets//Music//snd_15186.wav";
const char* walkSFX = ".//Assets//Music//RD_UI_Scroll_Up.wav";
const char* bombPlaceSFX = ".//Assets//Music//RD_UI_Scroll_Down.wav";
const char* menuSFX = ".//Assets//Music//RD_UI_Select.wav";
const char* explosionSFX = ".//Assets//Music//RD_Bomb_Explode_02.wav";
const char* spawnSFX = ".//Assets//Music//RD_Upgrade_Pickup_01.wav";
const char* powerUpSFX = ".//Assets//Music//RD_Upgrade_Pickup_03.wav";

struct key {
	bool prevState = false;
	bool currState = false;

	inline bool pressed() { return currState && !prevState; }
	inline bool held() { return currState && prevState; }
	inline bool idle() { return !currState && !prevState; }
	inline bool released() { return !currState && prevState; }
};

Button p1Pause;
Button p1Help;
CObject* pauseMenuBomb;
float pauseMenuTimer;


struct CONTROL_KEYS
{
	enum { UP = 0, DOWN, LEFT, RIGHT, BOMB, COUNT };
};

struct KeyboardInput {
	std::vector<key> controls;
	std::vector<int> keycodes;

	void updateKeys() {
		for (int i = 0; i < controls.size(); ++i) {
			controls[i].prevState = controls[i].currState;
			controls[i].currState = GetAsyncKeyState(keycodes[i]);
		}
	}

	key At(int index) {
		return controls[index];
	}
};

static std::vector<KeyboardInput> keyboardInputs;
struct KEYS {
	enum { UP = 0, DOWN, LEFT, RIGHT, ZERO, RMB, SPACE, HELP_MENU, GAME_STATE, FULLSCREEN, PAUSE, COUNT };
};
static std::vector<key> keys(KEYS::COUNT);

static std::vector<int> keycodes = {
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
bool PauseMenuToggle = false;
bool Controller1Alive = false;
bool Controller2Alive = false;

bool soundplaying;
bool soundplaying2;
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
int numPlayers = 2;

float isLDPADPressed = 0.0f;
float isRDPADPressed = 0.0f;
float isUDPADPressed = 0.0f;
float isDDPADPressed = 0.0f;
float isSouthButtonPressed = 0.0f;

float isP1StartButtonPressed = 0.0f;
float isP1SelectButtonPressed = 0.0f;
float isP2StartButtonPressed = 0.0f;
float isP2SelectButtonPressed = 0.0f;

float walktimer = 0.0f;

float isP1LDPADPressed = 0.0f;
float isP1RDPADPressed = 0.0f;
float isP1UDPADPressed = 0.0f;
float isP1DDPADPressed = 0.0f;
float isP1SouthButtonPressed = 0.0f;

bool CGame::Initialize() {
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

	if (G_SUCCESS(g_pAudioHolder->CreateMusicStream(backgroundMusicFilePath, &g_pMusicStream))) {
		if (G_SUCCESS(g_pMusicStream->SetVolume(0.5f))) {
			g_pMusicStream->StreamStart(true);
		}
	}

#pragma endregion

	float errorCode = 0;

	if (G_FAIL(g_pAudioHolder->CreateSound(placeHolderSFX, &g_pSoundPlayer))) {
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}

	if (G_FAIL(g_pAudioHolder->CreateSound(walkSFX, &walkSound1)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}
	walkSound1->SetVolume(0.4f);

	if (G_FAIL(g_pAudioHolder->CreateSound(spawnSFX, &spawnSound1)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}
	spawnSound1->SetVolume(0.25f);

	if (G_FAIL(g_pAudioHolder->CreateSound(bombPlaceSFX, &bombPlaceSound1)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}
	bombPlaceSound1->SetVolume(0.8f);

	if (G_FAIL(g_pAudioHolder->CreateSound(spawnSFX, &spawnSound2)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}
	spawnSound2->SetVolume(0.25f);

	if (G_FAIL(g_pAudioHolder->CreateSound(bombPlaceSFX, &bombPlaceSound2)))
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}
	bombPlaceSound2->SetVolume(0.8f);
	//for (int i = 0; i < numPlayers; ++i)
	//{
	//    if (walkSound.size() != numPlayers)
	//    {
	//        walkSound.resize(2);
	//    }
	//    if (G_FAIL(g_pAudioHolder->CreateSound(walkSFX, &walkSound.at(i))))
	//    {
	//        g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	//    }
	//}

	for (int i = 0; i < 2; ++i)
	{
		if (MenuSounds.size() != 2)
		{
			MenuSounds.resize(2);
		}
		if (G_FAIL(g_pAudioHolder->CreateSound(menuSFX, &MenuSounds.at(i))))
		{
			g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
		}
	}

	for (int i = 0; i < 24; ++i)
	{
		if (explosionSound.size() != 24)
		{
			explosionSound.resize(24);
		}
		if (G_FAIL(g_pAudioHolder->CreateSound(explosionSFX, &explosionSound.at(i))))
		{
			g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		if (bombPlaceSound.size() != 4)
		{
			bombPlaceSound.resize(4);
		}
		if (G_FAIL(g_pAudioHolder->CreateSound(bombPlaceSFX, &bombPlaceSound.at(i))))
		{
			g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
		}
		bombPlaceSound.at(i)->SetVolume(0.8f);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (powerUpSound.size() != 4)
		{
			powerUpSound.resize(4);
		}
		if (G_FAIL(g_pAudioHolder->CreateSound(powerUpSFX, &powerUpSound.at(i))))
		{
			g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
		}
		powerUpSound.at(i)->SetVolume(0.2f);
	}


	p1Pause.SetButtonID(G_START_BTN);
	p1Help.SetButtonID(G_SELECT_BTN);

	setGameState(GAME_STATE::MAIN_MENU);

	p1Pause.Reset(false);
	GW::SYSTEM::GWindowInputEvents gLastEvent;

	while (G_SUCCESS(g_pWindow->GetLastEvent(gLastEvent)) && gLastEvent != GW::SYSTEM::GWindowInputEvents::DESTROY) {
		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;

		static ULONGLONG currFrame = GetTickCount64();
		static ULONGLONG prevFrame = GetTickCount64();

		prevFrame = currFrame;
		currFrame = GetTickCount64();
		timePassed = (currFrame - prevFrame) / 1000.0;
		mapTime += timePassed;

		for (int i = 0; i < keycodes.size(); ++i) {
			keys[i].prevState = keys[i].currState;
			keys[i].currState = GetAsyncKeyState(keycodes[i]) & 0x8000;
		}

		p1Pause.Update(0, curGameState != GAME_STATE::ARCADE_GAME);

		for (int i = 0; i < CONTROL_KEYS::COUNT; ++i)
		{
			keyboardInputs[0].controls[i].prevState = keyboardInputs[0].controls[i].currState;
			keyboardInputs[1].controls[i].prevState = keyboardInputs[1].controls[i].currState;

			keyboardInputs[0].controls[i].currState = GetAsyncKeyState(keyboardInputs[0].keycodes[i]);
			keyboardInputs[1].controls[i].currState = GetAsyncKeyState(keyboardInputs[1].keycodes[i]);
		}

		if (keys[KEYS::FULLSCREEN].pressed()) {
			FullScreen = !FullScreen;
			this->WindowResize();
		}
		if (keys[KEYS::HELP_MENU].pressed() || p1Help.Pressed()) {
			ControlScreenToggle = !ControlScreenToggle;
			isPaused = !isPaused;

		}

		if (keys[KEYS::GAME_STATE].pressed() || p1Pause.Released())
		{
			if (curGameState == GAME_STATE::MAIN_MENU)
			{
				setGameState(GAME_STATE::ARCADE_GAME);
			}
			else if (curGameState == GAME_STATE::ARCADE_GAME) {
				setGameState(GAME_STATE::MAIN_MENU);
			}
			else if (curGameState == GAME_STATE::WIN_SCREEN) {
				setGameState(GAME_STATE::MAIN_MENU);
			}
			else if (curGameState == GAME_STATE::CREDIT_SCREEN)
			{
				setGameState(GAME_STATE::MAIN_MENU);
			}

		}

		if (keys[KEYS::PAUSE].pressed()) {
			isPaused = !isPaused;
			ControlScreenToggle = !ControlScreenToggle;

			if (isPaused == false) {
				g_pAudioHolder->ResumeAll();
				g_pMusicStream->ResumeStream();
				timePassed = tempTime;
			}
		}

		if (isPaused == true) {
			g_pMusicStream->PauseStream();
			g_pAudioHolder->PauseAll();
			tempTime = timePassed;
			timePassed = 0;
		}

		if (curGameState == GAME_STATE::MAIN_MENU)
		{
			if (menuBomb)
			{
				menuBomb->GetInput();

				menucontroltimer += timePassed;
				if (menuBomb->GetCharacterController()->GetUpDown() > 0.0f && menucontroltimer > 0.2 && menuIndex > 0)
				{
					if (menuIndex != 3)
					{
						menucontroltimer = 0.0;
						menuBomb->Move(0.0f, 2.45f, false);
						menuIndex -= 1;
						for (int i = 0; i < MenuSounds.size(); ++i)
						{
							MenuSounds.at(i)->isSoundPlaying(soundplaying);
							if (!soundplaying)
							{
								MenuSounds.at(i)->Play();
								break;
							}
						}
					}
					else
					{
						menucontroltimer = 0.0;
						menuBomb->Move(0.0f, 4.9f + 2.45f, false);
						menuIndex -= 3;
						for (int i = 0; i < MenuSounds.size(); ++i)
						{
							MenuSounds.at(i)->isSoundPlaying(soundplaying);
							if (!soundplaying)
							{
								MenuSounds.at(i)->Play();
								break;
							}
						}
					}
				}
				if (menuBomb->GetCharacterController()->GetUpDown() < 0.0f && menucontroltimer > 0.2 && menuIndex < 3)
				{
					if (menuIndex != 0)
					{
						menucontroltimer = 0.0;
						menuBomb->Move(0.0f, -2.45f, false);
						menuIndex += 1;
						for (int i = 0; i < MenuSounds.size(); ++i)
						{
							MenuSounds.at(i)->isSoundPlaying(soundplaying);
							if (!soundplaying)
							{
								MenuSounds.at(i)->Play();
								break;
							}
						}
					}
					else
					{
						menucontroltimer = 0.0;
						menuBomb->Move(0.0f, -4.9f - 2.45f, false);
						menuIndex += 3;
						for (int i = 0; i < MenuSounds.size(); ++i)
						{
							MenuSounds.at(i)->isSoundPlaying(soundplaying);
							if (!soundplaying)
							{
								MenuSounds.at(i)->Play();
								break;
							}
						}
					}
				}
				if (menuBomb->GetCharacterController()->ButtonReleased(DEFAULT_CONTROLLER_BUTTONS::ACTION))
				{
					for (int i = 0; i < MenuSounds.size(); ++i)
					{
						MenuSounds.at(i)->isSoundPlaying(soundplaying);
						if (!soundplaying)
						{
							MenuSounds.at(i)->Play();
							break;
						}
					}
					switch (menuIndex) {
					case 0:
					{
						setGameState(GAME_STATE::ARCADE_GAME);
						break;
					}
					case 1:
					{
						break;
					}
					case 2:
					{
						break;
					}
					case 3:
					{
						HWND windowHandle;
						g_pWindow->GetWindowHandle(sizeof(HWND), (void**)& windowHandle);
						SendMessage(windowHandle, WM_CLOSE, 0, 0);
						break;
					}
					default:
					{
						break;
					}
					}

				}
			}
		}

		if (prevShowMouse && !showMouse)
			ShowCursor(false);
		if (!prevShowMouse && showMouse)
			ShowCursor(true);
		if (curGameState == GAME_STATE::ARCADE_GAME)
		{
			if (!isPaused)
			{
				g_pMusicStream->ResumeStream();
				if (tempTime != 0)
					timePassed = tempTime;
			}

			updateBombs(timePassed);
			prevCursor = currCursor;
			GetCursorPos(&currCursor);

			/*
			prevShowMouse = showMouse;
			if (prevCursor.x == currCursor.x && prevCursor.y == currCursor.y)
			{
				mouseIdleTimer += timePassed;
				if (mouseIdleTimer >= 3.0)
					showMouse = false;
			}
			else
			{
				showMouse = true;
				mouseIdleTimer = 0.0;
			}
			if (prevShowMouse && !showMouse)
				ShowCursor(false);
			if (!prevShowMouse && showMouse)
				ShowCursor(true);
				f*/

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

			if (keys[KEYS::RMB].held()) {
				g_d3dData->debugCursorRot.y = 0.002f*(currCursor.y - prevCursor.y);
				g_d3dData->debugCursorRot.x = 0.002f*(currCursor.x - prevCursor.x);
			}

			static int numPlayersAlive = 0;
			static int prevNumPlayersAlive = 0;

			prevNumPlayersAlive = numPlayersAlive;
			numPlayersAlive = 0;
			if (menuBomb && menuBomb->isAlive())
			{
				numPlayersAlive++;
			}
			for (int i = 0; i < v_cPlayers.size(); ++i)
			{
				if (v_cPlayers[i] && v_cPlayers[i]->isAlive())
					numPlayersAlive++;
			}
			for (int i = 0; i < v_cAI.size(); ++i)
			{
				if (v_cAI[i] && v_cAI[i]->isAlive())
					numPlayersAlive++;
			}

			if (prevNumPlayersAlive > 1 && numPlayersAlive <= 1) {
				int winner = 0;

				for (CObject* menu : menuObjects)
				{
					TComponent* cRenderer;
					TComponent* cTexture;
					if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
						continue;
					menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
					TRendererComponent* renderer = (TRendererComponent*)cRenderer;
					TTextureComponent* Texture = (TTextureComponent*)cTexture;
					if (renderer->iUsedLoadState == GAME_STATE::WIN_SCREEN)
					{
						for (CPlayer* player : v_cPlayers)
						{
							winner++;
							if (player->isAlive())
							{
								break;
							}
						}
						if (winner == 0)
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_1_WIN;
						else if (winner == 1)
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_2_WIN;
						else if (winner == 2)
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_3_WIN;
						else
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_4_WIN;
					}
				}
				setGameState(GAME_STATE::WIN_SCREEN);
			}

			this->GamePlayLoop(timePassed);
		}

		//RenderMenus
		for (CObject* menu : menuObjects) {
			TComponent* cRenderer;

			if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
				continue;

			TRendererComponent* renderer = (TRendererComponent*)cRenderer;
			if (renderer->iUsedLoadState == curGameState || (PauseMenuToggle && renderer->iUsedLoadState == GAME_STATE::PAUSE_MENU))
				p_cRendererManager->RenderObject(menu);
			if (renderer->iUsedLoadState == GAME_STATE::CONTROLS_SCREEN && ControlScreenToggle)
			{
				p_cRendererManager->RenderObject(menu);
			}

		}

		//RenderObjects
		if (mapTime >= 55) {
			for (int i = 0; i < objects.size(); ++i) {
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

			if (mapTime >= 60) {
				for (int passes = 0; passes < 6; passes++) {
					for (int i = 0; i < objects.size(); ++i) {
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
		for (CPlayer* AI : v_cAI)
		{
			if (!AI || !AI->isAlive())
				continue;
			TComponent* renderer = nullptr;
			if (AI->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
			{
				TRendererComponent* pRenderer = (TRendererComponent*)renderer;
				if (pRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject((CObject*)AI);
			}
		}
		if (menuBomb)
		{
			TComponent* renderer = nullptr;
			if (menuBomb->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
			{
				TRendererComponent* pRenderer = (TRendererComponent*)renderer;
				if (pRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject((CObject*)menuBomb);
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

		if (!p_cRendererManager->Draw(timePassed, curGameState))
		{
			g_pLogger->LogCatergorized("FAILURE", "Failed to draw");
		}
	}
}

void CGame::LoadAnim()
{
	OBJLoadInfo loadInfo;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.position = { 0.0f, 10.0f, -5.0f };
	loadInfo.forwardVec = { 0.0f, 1.6f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
	loadInfo.LoadState = GAME_STATE::CREDIT_SCREEN;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.usedVertex = VERTEX_SHADER::ANIM;
	loadInfo.usedPixel = PIXEL_SHADER::ANIM;
	loadInfo.collisionLayer = COLLISION_LAYERS::FLOOR;
	loadInfo.usedGeo = -1;
	loadInfo.floor = true;
	loadInfo.destroyable = false;
	loadInfo.meshID = MODELS::BATTLEMAGE;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

void CGame::LoadObject()
{
	OBJLoadInfo loadInfo;
	float minX, maxX, minZ, maxZ;

	minX = -12.5;
	maxX = 15;
	minZ = -7.5;
	maxZ = 15;

	//for (float z = minZ; z < maxZ; z += 2.5f) {
	//	for (float x = minX; x < maxX; x += 2.5f) {
	//		loadInfo.position = { x, -2.5f, z };
	//		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	//		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLUE_TEX;
	//		loadInfo.usedGeo = -1;
	//		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	//		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	//		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	//		loadInfo.meshID = MODELS::CUBE;
	//		loadInfo.LoadState = 3;
	//		loadInfo.floor = true;
	//		loadInfo.hasCollider = true;
	//		loadInfo.destroyable = false;
	//		loadInfo.collisionLayer = COLLISION_LAYERS::FLOOR;
	//		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	//		====== =
				//TCollider collider = GetCenter(v_tMeshTemplates[0]);

				//window data(width, height)
				//g_pWindow->GetClientWidth(width);
				//g_pWindow->GetClientHeight(height);

				/*
					loadInfo.destroyable = true;
				loadInfo.item = true;
					loadInfo.hasCollider = true;
				loadInfo.destroyable = true;

				collider.center.x = GetCenter(v_tMeshTemplates[0]).center.x + loadInfo.position.x;
				collider.center.y = GetCenter(v_tMeshTemplates[0]).center.y + loadInfo.position.y;
				collider.center.z = GetCenter(v_tMeshTemplates[0]).center.z + loadInfo.position.z;

				loadInfo.collider.center = collider.center;
				loadInfo.collider.extents = collider.extents;
				loadInfo.LoadState = 3;
				loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);



				//collider data
				collider = GetCenter(v_tMeshTemplates[1]);
				collider.center.x = GetCenter(v_tMeshTemplates[1]).center.x + loadInfo.position.x;
				collider.center.y = GetCenter(v_tMeshTemplates[1]).center.y + loadInfo.position.y;
				collider.center.z = GetCenter(v_tMeshTemplates[1]).center.z + loadInfo.position.z;

				loadInfo.collider.center = collider.center;
				loadInfo.collider.extents = collider.extents;


				collider.center.x = GetCenter(v_tMeshTemplates[1]).center.x + loadInfo.position.x;
				collider.center.y = GetCenter(v_tMeshTemplates[1]).center.y + loadInfo.position.y;
				collider.center.z = GetCenter(v_tMeshTemplates[1]).center.z + loadInfo.position.z;

				loadInfo.collider.center = collider.center;
				loadInfo.collider.extents = collider.extents;


					//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));



				loadInfo.position = { 0.0f, 0.0f, 2.5f };
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
				*/



	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLUE_TEX;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = GAME_STATE::ARCADE_GAME;
	loadInfo.floor = true;
	loadInfo.item = false;
	loadInfo.destroyable = false;
	loadInfo.collisionLayer = COLLISION_LAYERS::FLOOR;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);

	for (float z = -7.5f; z < 15.0f; z += 2.5f)
	{
		for (float x = -12.5f; x < 15.0f; x += 2.5f)
		{
			loadInfo.position = { x, -2.5f, z };
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	for (float z = -2.5f; z < 10.0f; z += 2.5f) {
		for (float x = -7.5f; x < 10.0f; x += 2.5f) {
			if (rand() % 5 >= 1) {
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

	for (float x = -7.5; x <= 7.5; x += 2.5f) {
		//====== =
		//	//HAY
		//	for (float x = -7.5; x <= 7.5; x += 2.5f)
		//	{
		//		>>>>>> > origin / Month2 - UI
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
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	loadInfo.position = { -10, 0, 0 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -10, 0, 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -10, 0, -5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5, 0, -5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	//loadInfo.position = { -5, 0, 0 };
	//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	//loadInfo.position = { -5, 0, 5 };
	//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -5, 0, 10 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0, 0, 10 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 0, 0, -5 };
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

	loadInfo.position = { 5, 0, -5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	//loadInfo.position = { 5, 0, 0 };
	//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	//loadInfo.position = { 5, 0, 5 };
	//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 5, 0, 10 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 10, 0, 10 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 10, 0, 0 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 10, 0, 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 10.0f, 0, -5.0f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -10.0f, 0, 10.0f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}


void CGame::Cleanup()
{
	delete p_cEntityManager;
	p_cEntityManager = nullptr;
	delete p_cRendererManager;
	p_cRendererManager = nullptr;
	if (menuBomb)
	{
		menuBomb->Cleanup();
		delete menuBomb;
		menuBomb = nullptr;
	}
	for (CObject* object : objects)
	{
		object->Cleanup();
		delete object;
		object = nullptr;
	}
	objects.clear();

	for (CObject* object : menuObjects)
	{
		object->Cleanup();
		delete object;
		object = nullptr;
	}
	menuObjects.clear();
}

CGame::CGame()
{

}

CGame::~CGame() {
	delete p_cRendererManager;
}


void CGame::WindowResize() {
	g_pWindow->GetClientWidth(width);
	g_pWindow->GetClientHeight(height);
	g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	GW::SYSTEM::GWindowStyle style = (FullScreen) ? GW::SYSTEM::GWindowStyle::FULLSCREENBORDERLESS : GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED;
	g_pWindow->ChangeWindowStyle(style);
	g_pWindow->GetClientWidth(width);
	g_pWindow->GetClientHeight(height);
	g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	g_d3dData->d3dViewport.Height = static_cast<float>(height);
	g_d3dData->d3dViewport.Width = static_cast<float>(width);
}

void CGame::GamePlayLoop(double timePassed)
{
	//std::vector<CPlayer*> AIs;
	//AIs.resize(v_cAI.size());
	//std::vector<float> largest;
	//largest.resize(v_cAI.size());
	//std::vector<int> pToMimic;
	//pToMimic.resize(v_cAI.size());
	//int i = 0;
	//for (CPlayer* currAI : v_cAI)
	//{
	//	AIs.at(i) = currAI;
	//	int j = 0;
	//	TComponent* aiComponent;
	//	currAI->GetComponent(COMPONENT_TYPE::TRANSFORM, aiComponent);
	//	TTransformComponent* aiTransform = (TTransformComponent*)aiComponent;
	//	for (CPlayer* currPlayer : v_cPlayers)
	//	{
	//		TComponent* pComponent;
	//		currPlayer->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
	//		TTransformComponent* pTransform = (TTransformComponent*)pComponent;
	//
	//		float deltaX = aiTransform->fPosition.x - pTransform->fPosition.x;
	//		float deltaY = aiTransform->fPosition.y - pTransform->fPosition.y;
	//		float distance = sqrt((deltaX *deltaX) + (deltaY *deltaY));
	//		if(!largest.at(i) || largest.at(i) < distance)
	//		{
	//			largest.at(i) = distance;
	//			pToMimic.at(i) = j;
	//		}
	//	}
	//	i++;
	//}

	for (CPlayer* currPlayer : v_cPlayers)
	{
		if (!currPlayer || !currPlayer->isAlive())
			continue;

		currPlayer->GetInput();

		CharacterController* cont = currPlayer->GetCharacterController();
		//if (currPlayer->GetCharacterController()->ButtonPressed(DEFAULT_CONTROLLER_BUTTONS::HELP))
		//{
		//	ControlScreenToggle = !ControlScreenToggle;
		//	isPaused = !isPaused;
		//}
		if (currPlayer->GetCharacterController()->ButtonReleased(DEFAULT_CONTROLLER_BUTTONS::PAUSE))
		{
			int previndex = menuIndex;
			menuIndex = 0;
			previndex = menuIndex - previndex;
			pauseMenuBomb->Move(0.0f, (-(float)previndex*1.2f), false);
			PauseMenuToggle = !PauseMenuToggle;
			isPaused = !isPaused;
		}


		/*

		float isUpPressed = 0.0f, isDownPressed = 0.0f, isLeftPressed = 0.0f, isRightPressed = 0.0f, isSouthPressed = 0.0f;
		float leftright = 0.0f, updown = 0.0f;
		bool controllerConnected;

		g_pControllerInput->IsConnected(currPlayer->GetControllerIndex(), controllerConnected);

		if (controllerConnected) {
		if (controllerConnected)
		{

			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_UP_BTN, isUpPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_DOWN_BTN, isDownPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_LEFT_BTN, isLeftPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_DPAD_RIGHT_BTN, isRightPressed);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_SOUTH_BTN, isSouthPressed);

			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_LX_AXIS, leftright);
			g_pControllerInput->GetState(currPlayer->GetControllerIndex(), G_LY_AXIS, updown);
		}


		bool canMove = true;
		*/


		float deltaX = 0.0f, deltaZ = 0.0f;

		TComponent* pComponent;
		currPlayer->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
		TTransformComponent* pTransform = (TTransformComponent*)pComponent;

		/*
		TComponent* col = nullptr;

		if (currPlayer->GetComponent(COMPONENT_TYPE::COLLIDER, col)) {
			TColliderComponent* tCol = (TColliderComponent*)col;
			DirectX::XMStoreFloat4(&tCol->d3dCollider.Orientation, DirectX::XMQuaternionRotationMatrix(pTransform->mObjMatrix));
		}
		*/

		deltaX = currPlayer->GetCharacterController()->GetLeftRight() * timePassed * PLAYER_SPEED;
		deltaZ = currPlayer->GetCharacterController()->GetUpDown() * timePassed * PLAYER_SPEED;


		/*
		if (isUpPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::UP).held())
			deltaZ += timePassed * PLAYER_SPEED;

		if (isDownPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::DOWN).held())
			deltaZ -= timePassed * PLAYER_SPEED;

		if (isLeftPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::LEFT).held())
			deltaX -= timePassed * PLAYER_SPEED;

		if (isRightPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::RIGHT).held())
			deltaX += timePassed * PLAYER_SPEED;

		if (deltaX != 0.0f || deltaZ != 0.0f) {
		*/
		if (deltaX != 0.0f || deltaZ != 0.0f)
		{
			currPlayer->Move(deltaX, deltaZ);
			bool walkplaying;
			walktimer += timePassed;
			walkSound1->isSoundPlaying(walkplaying);
			if (!walkplaying && walktimer > 0.3f)
			{
				walktimer = 0.0f;
				walkSound1->Play();
			}
		}


		for (CObject* cObj : objects) {
			if (currPlayer->Collides(cObj))
				PlayerCollision(currPlayer, cObj);
		}

		TComponent* _prenderer = nullptr;
		TComponent* _brenderer = nullptr;

		for (CBomb* bomb : v_cBombs) {
			if (bomb && bomb->isAlive()) {
				if (bomb->getTimer() >= 0.7f) {
					if (currPlayer->Collides(bomb)) {
						PlayerBombCollision(currPlayer, bomb);
					}
				}
			}
		}
		for (int i = 0; i < items.size(); i++)
		{
			if (currPlayer->Collides((CObject*)items[i]))
			{
				for (int i = 0; i < powerUpSound.size(); ++i)
				{
					powerUpSound.at(i)->isSoundPlaying(soundplaying);
					if (!soundplaying)
					{
						powerUpSound.at(i)->Play();
						break;
					}
				}
				currPlayer->SetBombType(items[i]->GetItemType());
				if (currPlayer->GetNumBombs() < 3)
				{
					currPlayer->incNumBombs();
				}

				items.erase(items.begin() + i);
				--i;
			}
		}


		if (currPlayer->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
			TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

			if (pRenderer->fPosition.x < fMinX - 1.3 || pRenderer->fPosition.x > fMaxX + 1.3 || pRenderer->fPosition.z < fMinZ - 1.3 || pRenderer->fPosition.z > fMaxZ + 1.3) {
				offMapTimer += timePassed;

				if (offMapTimer >= 0.25) {
					offMapTimer = 0;
					currPlayer->setAlive(false);
				}
			}
		}

		if (currPlayer->GetCharacterController()->ButtonPressed(DEFAULT_CONTROLLER_BUTTONS::ACTION) && !isPaused)//isSouthPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::BOMB).pressed())
		{
			if (currPlayer->hasAvailableBombSlot())
			{
				bool soundplaying;
				bombPlaceSound1->isSoundPlaying(soundplaying);
				if (!soundplaying)
				{
					bombPlaceSound1->Play();
				}
				else
				{
					bombPlaceSound2->Play();
				}
				for (int i = 0; i < maxNumBombs; ++i)
				{
					if (v_cBombs[i] == nullptr || !v_cBombs[i]->isAlive())
					{
						currPlayer->AddBombIndex(i);
						std::vector<CBomb*> bombs;

						switch (currPlayer->GetBombType()) {
							//case 4:
							//	if (v_cBombs[i]) {
							//		bombs = p_cEntityManager->DropBomb0(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}
							//	else {
							//		bombs = p_cEntityManager->DropBomb0(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}

							//	break;
							//case 1:
							//	if (v_cBombs[i]) {
							//		bombs = p_cEntityManager->DropBomb1(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}
							//	else {
							//		bombs = p_cEntityManager->DropBomb1(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}

							//	break;
							//case 2:
							//	if (v_cBombs[i]) {
							//		bombs = p_cEntityManager->DropBomb2(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}
							//	else {
							//		bombs = p_cEntityManager->DropBomb2(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}

							//	break;
							//case 3:
							//	if (v_cBombs[i]) {
							//		bombs = p_cEntityManager->DropBomb3(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}
							//	else {
							//		bombs = p_cEntityManager->DropBomb3(currPlayer);
							//		for (int j = 0; j < bombs.size(); j++) {
							//			v_cBombs[i + j] = bombs[j];
							//		}
							//	}

							//	break;
						default:
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

		if (PauseMenuToggle)
		{
			currPlayer->GetCharacterController()->GetUpDown();
			pauseMenuTimer += 0.1f;
			if (!ControlScreenToggle)
			{
				if (currPlayer->GetCharacterController()->GetUpDown() > 0.0f && pauseMenuTimer > 4.0f && menuIndex > 0)
				{

					pauseMenuTimer = 0.0f;
					pauseMenuBomb->Move(0.0f, 1.2f, false);
					menuIndex -= 1;
					for (int i = 0; i < MenuSounds.size(); ++i)
					{
						//MenuSounds.at(i)->isSoundPlaying(soundplaying);
						//if (!soundplaying)
						//{
						//	MenuSounds.at(i)->Play();
						//	break;
						//}
					}

				}
				if (currPlayer->GetCharacterController()->GetUpDown() < 0.0f && pauseMenuTimer > 4.0f && menuIndex < 2)
				{

					pauseMenuTimer = 0.0f;
					pauseMenuBomb->Move(0.0f, -1.2f, false);
					menuIndex += 1;
					for (int i = 0; i < MenuSounds.size(); ++i)
					{
						//MenuSounds.at(i)->isSoundPlaying(soundplaying);
						//if (!soundplaying)
						//{
						//	MenuSounds.at(i)->Play();
						//	break;
						//}
					}

				}
			}
			if (currPlayer->GetCharacterController()->ButtonReleased(DEFAULT_CONTROLLER_BUTTONS::ACTION))
			{
				//for (int i = 0; i < MenuSounds.size(); ++i)
				//{
				//	MenuSounds.at(i)->isSoundPlaying(soundplaying);
				//	if (!soundplaying)
				//	{
				//		MenuSounds.at(i)->Play();
				//		break;
				//	}
				//}
				switch (menuIndex) {
				case 0:
				{
					isPaused = !isPaused;
					PauseMenuToggle = !PauseMenuToggle;
					break;
				}
				case 1:
				{
					ControlScreenToggle = !ControlScreenToggle;
					break;
				}
				case 2:
				{
					setGameState(GAME_STATE::MAIN_MENU);
					int previndex = menuIndex;
					menuIndex = 0;
					previndex = menuIndex - previndex;
					pauseMenuBomb->Move(0.0f, (-(float)previndex*1.2f), false);
					PauseMenuToggle = !PauseMenuToggle;
					isPaused = !isPaused;
					break;
				}
				default:
				{
					break;
				}
				}

			}
		}

	}
}


//void InitSortedParticles(sorted_pool_t<particle, 1000>& sortedPool, float deltaTime) {
//	//Init number of particles as loop
//	for (size_t i = 0; i < 1; i++) {
//		int count = sortedPool.alloc();
//
//		if (count != -1) {
//			sortedPool[count].pos.x = 0;
//			sortedPool[count].pos.y = 0;
//			sortedPool[count].pos.z = 0;
//			sortedPool[count].speed.x = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX)) / 3;
//			sortedPool[count].speed.y = (0.0f + (2.0f - 0.0f) * ((float)rand() / (float)RAND_MAX)) / 2;
//			sortedPool[count].speed.z = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX)) / 3;
//			sortedPool[count].speed.y *= particleSpeed;
//			sortedPool[count].timer = 1.5f + (1.8f - 1.5f) * ((float)rand() / (float)RAND_MAX);
//			sortedPool[count].color = { 1,0,0,1 };
//		}
//	}
//
//	for (size_t i = 0; i < sortedPool.size(); i++) {
//		sortedPool[i].prev_pos = sortedPool[i].pos;
//		sortedPool[i].pos.x += (sortedPool[i].speed.x * deltaTime);
//		sortedPool[i].pos.y += (sortedPool[i].speed.y * deltaTime);
//		sortedPool[i].pos.z += (sortedPool[i].speed.z * deltaTime);
//		sortedPool[i].speed.y -= particleGravity * deltaTime;
//		sortedPool[i].timer -= deltaTime;
//		add_line(sortedPool[i].pos, sortedPool[i].prev_pos, sortedPool[i].color);
//
//		if (sortedPool[i].timer <= 0) {
//			sortedPool.free(i);
//			i--;
//		}
//	}
//}
//
//void InitFreeParticles(emitter& emitter, pool_t<particle, 1024>& freePool, float deltaTime) {
//	//init emitters
//	firstEmit.spawn_pos = { 5,0,5 };
//	secondEmit.spawn_pos = { 0,0,10 };
//	thirdEmit.spawn_pos = { 5,0,0 };
//	firstEmit.spawn_color = { 0,1,0,0 };
//	secondEmit.spawn_color = { 0,0,1,0 };
//	thirdEmit.spawn_color = { 0,1,1,0 };
//
//	//alloc space
//	int count = freePool.alloc();
//	int emitCount = emitter.indices.alloc();
//
//	emitter.indices[emitCount] = count;
//	freePool[count].pos = emitter.spawn_pos;
//	freePool[count].prev_pos = emitter.spawn_pos;
//	freePool[count].speed.x = (-5.0f + (5.0f - -5.0f) * ((float)rand() / (float)RAND_MAX)) / 3;
//	freePool[count].speed.y = (0.0f + (2.0f - 0.0f) * ((float)rand() / (float)RAND_MAX)) / 2;
//	freePool[count].speed.z = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX)) / 3;
//	freePool[count].speed.x *= particleSpeed;
//	freePool[count].timer = ((float)rand() / (float)RAND_MAX);
//	freePool[count].color = { 0,1,1,0 };
//
//	for (size_t i = 0; i < emitter.indices.size(); i++) {
//		int Ecount = emitter.indices[i];
//
//		if (Ecount <= -1 || Ecount > 1024) {
//			break;
//		}
//
//		if (Ecount != -1) {
//			freePool[Ecount].timer -= deltaTime;
//
//			if (freePool[Ecount].timer <= 0.0f) {
//				freePool.free(Ecount);
//				emitter.indices.free(i);
//				i--;
//				continue;
//			}
//
//			freePool[Ecount].prev_pos = freePool[Ecount].pos;
//			freePool[Ecount].pos.x += (freePool[Ecount].speed.x * deltaTime);
//			freePool[Ecount].pos.y += (freePool[Ecount].speed.y * deltaTime);
//			freePool[Ecount].pos.z += (freePool[Ecount].speed.z * deltaTime);
//			freePool[Ecount].speed.y -= particleGravity * deltaTime;
//			add_line(freePool[Ecount].pos, freePool[Ecount].prev_pos, freePool[Ecount].color);
//		}
//	}
//}
//void CGame::SpawnObject(int i, std::vector<CObject*> objects, CRendererManager* p_cRendererManager, CEntityManager* p_cEntityManager) {
//	item = p_cEntityManager->ItemDrop(objects[i]);
//	p_cRendererManager->RenderObject(item);
//}

/*
void CGame::ExplodeBomb(int bombToExplodeIndex)
{
	for (CPlayer* _currPlayer : v_cPlayers)
	{
		if (_currPlayer)
		{
			std::vector<int>* playerBombs = &_currPlayer->getBombIndices();

			for (int i = 0; i < playerBombs->size(); ++i)
			{
				if (playerBombs->at(i) > bombToExplodeIndex)
					playerBombs->at(i)--;
				else if (playerBombs->at(i) == bombToExplodeIndex)
				{
					playerBombs->erase(playerBombs->begin() + i);
					--i;
				}
			}
		}
	}
	v_cBombs[bombToExplodeIndex]->Explode();
	v_cBombs.erase(v_cBombs.begin() + bombToExplodeIndex);
}
*/


void CGame::setGameState(int _gameState)
{
	switch (_gameState)
	{
	case GAME_STATE::MAIN_MENU:
	{
		p1Pause.Reset(false);
		ClearPlayersAndBombs();
		menuBomb = p_cEntityManager->InstantiatePlayer(1, MODELS::BOMB, DIFFUSE_TEXTURES::BOMB4, DirectX::XMFLOAT3(-1.5f, 11.4f, -6.8f), 0, DirectX::XMFLOAT3(0.0f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f));
		break;
	}
	case GAME_STATE::ARCADE_GAME:
	{
		delete menuBomb;
		menuBomb = nullptr;
		LoadObject();
		v_cPlayers[0] = p_cEntityManager->InstantiatePlayer(1, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(-12.5f, 0.0f, 12.5f));
		v_cPlayers[1] = p_cEntityManager->InstantiatePlayer(2, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(12.5f, 0.0f, -7.5f));
		v_cPlayers[2] = p_cEntityManager->InstantiatePlayer(3, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));
		v_cPlayers[3] = p_cEntityManager->InstantiatePlayer(4, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(-12.5f, 0.0f, -7.5f));

		fMinX = -15;
		fMaxX = 15;
		fMinZ = -10;
		fMaxZ = 15;
		mapTime = 0;
		//v_cAI[0] = p_cEntityManager->InstantiatePlayer(1, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));
		//v_cAI[1] = p_cEntityManager->InstantiatePlayer(2, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(-12.5f, 0.0f, -7.5f));

		spawnSound1->isSoundPlaying(soundplaying);
		if (!soundplaying)
			spawnSound1->Play();

		spawnSound2->isSoundPlaying(soundplaying);
		if (!soundplaying)
			spawnSound2->Play();

		break;
	}
	case GAME_STATE::WIN_SCREEN:
	{
		ClearPlayersAndBombs();
		break;
	}
	//case GAME_STATE::CREDIT_SCREEN:
	//{
	//	LoadAnim();
	//	break;
	//}
	default:
	{
		break;
	}
	}
	curGameState = _gameState;
}


void CGame::ClearPlayersAndBombs()
{
	for (int i = 0; i < v_cPlayers.size(); ++i)
		if (v_cPlayers[i])
		{
			v_cPlayers[i]->Cleanup();
			delete v_cPlayers[i];
			v_cPlayers[i] = nullptr;
		}
	for (int i = 0; i < v_cAI.size(); ++i)
		if (v_cAI[i])
		{
			v_cAI[i]->Cleanup();
			delete v_cAI[i];
			v_cAI[i] = nullptr;
		}

	v_cBombs.clear();
	v_cBombs.resize(maxNumBombs);
	Xexplosions.clear();
	Zexplosions.clear();
	explosionTimers.clear();

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Cleanup();
	}
	objects.clear();
	items.clear();
}

void CGame::updateBombs(double timePassed)
{
	for (int i = 0; i < explosionTimers.size(); ++i)
	{
		if (explosionTimers[i] >= 0.3f)
		{
			Xexplosions.erase(Xexplosions.begin() + i);
			Zexplosions.erase(Zexplosions.begin() + i);
			explosionTimers.erase(explosionTimers.begin() + i);
			--i;
			continue;
		}

		for (int j = 0; j < objects.size(); j++)
		{
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

		for (CPlayer* player : v_cPlayers)
		{
			if (player)
			{

				if (Xexplosions[i]->Collides((CObject*)player) || Zexplosions[i]->Collides((CObject*)player))
				{
					int playerIndex = player->GetControllerIndex();
					for (int i = 0; i < v_cPlayers.size(); i++)
					{
						if (player == v_cPlayers.at(i))
						{
							playerIndex = i;
						}
					}
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::WIN_SCREEN)
						{
							if (playerIndex == 0)
								Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_2_WIN;
							else
								Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_1_WIN;
						}


					}
					player->setAlive(false);
				}
			}

		}
		for (int k = 0; k < v_cBombs.size(); k++)
		{
			if (v_cBombs[k] && v_cBombs[k]->isAlive())
			{
				if (Xexplosions[i]->Collides((CObject*)v_cBombs[k]) || Zexplosions[i]->Collides((CObject*)v_cBombs[k]))
				{
					/*
					v_cBombs[k]->Explode();
					CPlayer* parent = v_cBombs[k]->getParent();
					for (int j = 0; j < parent->getBombIndices().size(); ++j) {
						if (parent->getBombIndices()[j] == k)
							parent->deleteBomb(j);
					}
					*/
					v_cBombs[k]->SetToExplode();

				}
			}
		}

		explosionTimers[i] += timePassed;
	}

	for (int i = 0; i < v_cBombs.size(); ++i)
	{
		if (!v_cBombs[i])
			continue;

		if (v_cBombs[i]->isAlive())
		{
			if (v_cBombs[i]->shouldExplode())
			{
				v_cBombs[i]->Explode();
				if (v_cPlayers.at(0)->isAlive())
					g_pControllerInput->StartVibration(0, 0.25f, 1, 0);
				if (v_cPlayers.at(1)->isAlive())
					g_pControllerInput->StartVibration(0, 0.25f, 1, 1);

				CPlayer* parent = v_cBombs[i]->getParent();
				for (int j = 0; j < parent->getBombIndices().size(); ++j) {
					if (parent->getBombIndices()[j] == i)
						parent->deleteBomb(j);
				}

				explosionTimers.push_back(0.0f);
				Xexplosions.push_back(p_cEntityManager->BombExplosionX(v_cBombs[i]));
				Zexplosions.push_back(p_cEntityManager->BombExplosionZ(v_cBombs[i]));
				v_cBombs[i]->SetAlive(false);
			}

			v_cBombs[i]->updateBomb(timePassed);
		}
	}
}

void CGame::loadMap(int index)
{

}

bool CGame::loadTempMenus() {
	OBJLoadInfo loadInfo;

	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.forwardVec = { 0.0f, 1.1f, -1.0f };

	loadInfo.position = { 0.0f, 2.5f, 18.6f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::NAMES_HUD;
	loadInfo.scale = DirectX::XMFLOAT3(2.4f, 0.25f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::ARCADE_GAME;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::MAIN_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(2.55f, 2.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::MAIN_MENU;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::PLAYER_1_WIN;
	loadInfo.scale = DirectX::XMFLOAT3(2.55f, 2.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::WIN_SCREEN;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));



	loadInfo.position = { 0.0f, 20.5f, -9.5f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HELP_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(0.8f, 1.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU2;
	loadInfo.LoadState = GAME_STATE::CONTROLS_SCREEN;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 19.2f, -9.5f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::PAUSE_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.2f, 1.0f);
	loadInfo.meshID = MODELS::MENU2;
	loadInfo.LoadState = GAME_STATE::PAUSE_MENU;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -2.2f, 18.9f, -10.5f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB;
	loadInfo.scale = DirectX::XMFLOAT3(0.5, 0.5f, 0.0001f);
	loadInfo.meshID = MODELS::BOMB;
	loadInfo.LoadState = GAME_STATE::PAUSE_MENU;
	pauseMenuBomb = p_cEntityManager->CreateOBJFromTemplate(loadInfo);
	menuObjects.push_back(pauseMenuBomb);

	//loadInfo.position = { -2.5f, 11.4f, -5.9f };
	//loadInfo.forwardVec = { 0.0f, 1.6f, -1.0f };
	//loadInfo.meshID = MODELS::BOMB;
	//loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLACK_TEX;
	//loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	//loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	//loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	//loadInfo.usedGeo = -1;
	//loadInfo.LoadState = 0;
	//loadInfo.floor = false;
	//loadInfo.destroyable = true;
	//loadInfo.item = true;
	//loadInfo.scale = DirectX::XMFLOAT3(1.0f * 0.5f, 1.0f * 0.5f, 1.0f *0.5f);
	//menuBomb = p_cEntityManager->CreateOBJFromTemplate(loadInfo);
	//menuObjects.push_back(menuBomb);
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

	if (zD < mZ && zD > mZ - 0.2)
	{
		playerToCheck->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	}
	else if (xD < mX && xD > mX - 0.2)
	{
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
		for (int i = 0; i < objects.size(); i++) {
			if (cBomb->Collides(objects[i])) {
				BombCollision(objects[i], cBomb, playerToCheck);
			}
		}

		for (int i = 0; i < v_cBombs.size(); i++) {
			if (v_cBombs[i] && cBomb->Collides((CObject*)v_cBombs[i])) {
				BombCollision((CObject*)v_cBombs[i], cBomb, playerToCheck);
			}
		}

		cBomb->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	}
	else if (xD < mX && xD > mX - 0.25) {
		for (int i = 0; i < objects.size(); i++) {
			if (cBomb->Collides(objects[i])) {
				BombCollision(objects[i], cBomb, playerToCheck);
			}
		}

		for (int i = 0; i < v_cBombs.size(); i++) {
			if (v_cBombs[i] && cBomb->Collides((CObject*)v_cBombs[i])) {
				BombCollision((CObject*)v_cBombs[i], cBomb, playerToCheck);
			}
		}

		cBomb->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
	}
}

void CGame::BombCollision(CObject* objectToCheck, CBomb* cBomb, CPlayer * playerToCheck) {
	TComponent* comp = nullptr;
	TColliderComponent* pCollider;
	objectToCheck->GetComponent(COMPONENT_TYPE::COLLIDER, comp);
	pCollider = (TColliderComponent*)comp;

	if (!cBomb->GetComponent(COMPONENT_TYPE::COLLIDER, comp))
		return;

	TColliderComponent* objCollider = (TColliderComponent*)comp;

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
		playerToCheck->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	}
	else if (xD < mX && xD > mX - 0.25) {
		cBomb->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
		playerToCheck->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
	}
}