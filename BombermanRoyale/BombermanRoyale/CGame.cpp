#include "CGame.h"
#include <DirectXCollision.h>
#include <iostream>
#include "pools.h"

#define PLAYER_SPEED 5.0f

const char* backgroundMusicFilePath = ".//Assets//Music//RushedDevelopment_BattleDemo.wav";
const char* placeHolderSFX = ".//Assets//Music//snd_15186.wav";
const char* walkSFX = ".//Assets//Music//RD_UI_Scroll_Up.wav";
const char* bombPlaceSFX = ".//Assets//Music//RD_UI_Scroll_Down.wav";
const char* menuSFX = ".//Assets//Music//RD_UI_Select.wav";
const char* explosionSFX = ".//Assets//Music//RD_Bomb_Explode_02.wav";
const char* spawnSFX = ".//Assets//Music//RD_Upgrade_Pickup_01.wav";
const char* powerUpSFX = ".//Assets//Music//RD_Upgrade_Pickup_03.wav";
const char* warningSFX = ".//Assets//Music//RD_Upgrade_Pickup_02.wav";
const char* fallingSFX = ".//Assets//Music//FallingSound.wav";
const char* playerfallingSFX = ".//Assets//Music//PlayerFalling.wav";
const char* WinScreenSFX = ".//Assets//Music//WinScreen.wav";
const char* DeathSFX = ".//Assets//Music//DeathSound.wav";

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
Button duck;
CObject* pauseMenuBomb;
float pauseMenuTimer;
int menux = 0;
int menuz = 0;


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
bool warningSoundPlaying = false;
bool fallingSoundPlaying = false;
bool playingfallingSoundPlaying = false;

bool isPaused = false;

static double timePassed = 0.0f;
double AIbombaction = 0.0f;
double tempTime = 0.0f;
double tempMapTime = 0.0f;
double tempShakeTime = 0.0f;
double mapTime = 0.0f;
double shakeTime = 0.0f;
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

int currNumControllers = 0;
int prevNumControllers = 0;

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

bool CGame::Initialize()
{
	keyboardInputs.resize(2);
	keyboardInputs[0].controls.resize(5);
	keyboardInputs[0].keycodes.resize(5);
	keyboardInputs[0].keycodes = { 'I', 'K', 'J', 'L', 'O' };

	keyboardInputs[1].controls.resize(5);
	keyboardInputs[1].keycodes.resize(5);
	keyboardInputs[1].keycodes = { VK_NUMPAD5, VK_NUMPAD2, VK_NUMPAD1, VK_NUMPAD3, VK_NUMPAD6 };
	viewPos = g_d3dData->viewMat;
	shakeTime = 0;

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

	if (G_FAIL(g_pAudioHolder->CreateSound(warningSFX, &warnSound))) {
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}

	if (G_FAIL(g_pAudioHolder->CreateSound(fallingSFX, &fallingSound))) {
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}

	if (G_FAIL(g_pAudioHolder->CreateSound(playerfallingSFX, &playerfallingSound))) {
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}

	if (G_FAIL(g_pAudioHolder->CreateSound(WinScreenSFX, &WinScreenSound))) {
		g_pLogger->LogCatergorized("FAILURE", "Failed to create SFX");
	}

	if (G_FAIL(g_pAudioHolder->CreateSound(DeathSFX, &DeathSound))) {
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
	//duck.SetButtonID(G_EAST_BTN);

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
		timer.Signal();

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
			//SprinklersOn = false;

		}

		if (keys[KEYS::GAME_STATE].pressed() || p1Pause.Released())
		{
			if (curGameState == GAME_STATE::MAIN_MENU)
			{
				setGameState(GAME_STATE::ARCADE_MENU);
			}
			else if (curGameState == GAME_STATE::ARCADE_MENU) {
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
			//SprinklersOn = false;
			if (isPaused == false) {
				g_pAudioHolder->ResumeAll();
				g_pMusicStream->ResumeStream();
				timePassed = tempTime;
				mapTime = tempMapTime;
				SprinklersOn = true;
				//shakeTime = 0;
			}
		}
		g_pControllerInput->GetNumConnected(currNumControllers);

		if (isPaused == true)
		{
			g_pAudioHolder->PauseAll();
			tempTime = timePassed;
			tempMapTime = mapTime;
			mapTime = 0;
			//tempShakeTime = shakeTime;
			//shakeTime += timePassed;
			SprinklersOn = false;
			timePassed = 0;
			//g_d3dData->resetCamera();
		}

#pragma region MAIN MENU

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
				if (menuBomb->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::ACTION))
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
						setGameState(GAME_STATE::ARCADE_MENU);
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
#pragma endregion

#pragma region ARCADE MENU

		if (curGameState == GAME_STATE::ARCADE_MENU)
		{
			if (menuBomb)
			{
				menuBomb->GetInput();

				menucontroltimer += timePassed;
				if (menuBomb->GetCharacterController()->GetUpDown() > 0.0f && menucontroltimer > 0.2 && menuz > 0)
				{

					menucontroltimer = 0.0;
					menuBomb->Move(0.0f, 3.0f, false);
					menuz -= 1;
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
				if (menuBomb->GetCharacterController()->GetUpDown() < 0.0f && menucontroltimer > 0.2 && menuz < 3)
				{

					menucontroltimer = 0.0;
					menuBomb->Move(0.0f, -3.0f, false);
					menuz += 1;
					for (int i = 0; i < MenuSounds.size(); ++i)
					{
						MenuSounds.at(i)->isSoundPlaying(soundplaying);
						if (!soundplaying)
						{
							MenuSounds.at(i)->Play();
							break;
						}
					}
					if (menuz > 1 && menux > 0)
					{
						menuBomb->Move(-8.0f, 0.0f, false);
						menux -= 1;
					}

				}
				if (menuBomb->GetCharacterController()->GetLeftRight() > 0.0f && menucontroltimer > 0.2 && menux < 1 && menuz < 2)
				{

					menucontroltimer = 0.0;
					menuBomb->Move(8.0f, 0.0f, false);
					menux += 1;
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
				if (menuBomb->GetCharacterController()->GetLeftRight() < 0.0f && menucontroltimer > 0.2 && menux > 0)
				{

					menucontroltimer = 0.0;
					menuBomb->Move(-8.0f, 0.0f, false);
					menux -= 1;
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
				if (menuBomb->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::ACTION))
				{
					menuIndex = (menuz * 2) + menux;
					if (menuz > 2)
						menuIndex = 5;
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
						if (numPLAYERS > 2)
							numPLAYERS--;
						break;
					}
					case 1:
					{
						if (numPLAYERS < 4 && numAI + numPLAYERS < 4)
							numPLAYERS++;
						break;
					}
					case 2:
					{
						if (numAI > 0)
							numAI--;
						break;
					}
					case 3:
					{
						if (numAI < 2 && numPLAYERS + numAI < 4)
							numAI++;
						break;
					}
					case 4:
					{
						menuBomb->Move(0.0f, 6.0f, false);
						menuz = 0;
						menux = 0;
						menuIndex = 0;
						setGameState(GAME_STATE::ARCADE_GAME);

						break;
					}
					case 5:
					{
						menuBomb->Move(0.0f, 9.0f, false);
						menuz = 0;
						menux = 0;
						menuIndex = 0;
						setGameState(GAME_STATE::MAIN_MENU);
						menuIndex = 0;
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
#pragma endregion

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
				int aiwinner = 0;

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
							if (player != NULL)
							{
								if (player->isAlive())
								{
									break;
								}
							}
							winner++;

						}
						for (CPlayer* AI : v_cAI)
						{
							if (AI != NULL)
							{
								if (AI->isAlive())
								{
									aiwinner++;
									break;
								}
							}
						}
						winner -= aiwinner;
						if (winner == 0)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_1_WIN;
							WinScreenSound->Play();
						}
						else if (winner == 1)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_2_WIN;
							WinScreenSound->Play();
						}
						else if (winner == 2)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_3_WIN;
							WinScreenSound->Play();
						}
						else if (winner == 3)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::PLAYER_4_WIN;
							WinScreenSound->Play();
						}
						else
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::DRAW_SCREEN;
							WinScreenSound->Play();
						}
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

		//render particles
		if (bombExploded)
		{
			InitSortedParticles(sortedParticles, timer.Delta(), bombPos, { 1,1,0,1 });
		}
		if (SprinklersOn == true)
		{
			//RenderParticles
			InitFreeParticles(firstEmit, shared_pool, timer.Delta());
			InitFreeParticles(secondEmit, shared_pool, timer.Delta());
			InitFreeParticles(thirdEmit, shared_pool, timer.Delta());
			InitFreeParticles(fourthEmit, shared_pool, timer.Delta());
		}

		//RenderObjects
		if (mapTime >= 42 && passes < 4)
		{
			fallingSoundPlaying = false;
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
			if (warningSoundPlaying == false)
			{
				warnSound->Play();
				warningSoundPlaying = true;
			}
		}

		if (mapTime >= 45 && passes < 4)
		{
			passes += 1;
			warningSoundPlaying = false;
			fallingSoundPlaying = false;

			g_pControllerInput->StartVibration(0, 0.25f, 1, 0);
			g_pControllerInput->StartVibration(0, 0.25f, 1, 1);
			g_pControllerInput->StartVibration(0, 0.25f, 1, 2);
			g_pControllerInput->StartVibration(0, 0.25f, 1, 3);

			if (fallingSoundPlaying == false)
			{
				fallingSound->Play();
				fallingSoundPlaying = true;
			}

			for (int passes = 0; passes < 6; passes++)
			{
				for (int i = 0; i < objects.size(); ++i)
				{
					TComponent* cRenderer = nullptr;
					TTransformComponent* renderer = nullptr;

					if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
					{
						renderer = (TTransformComponent*)cRenderer;

						if (renderer->fPosition.x == fMinX || renderer->fPosition.z == fMinZ || renderer->fPosition.x == fMaxX || renderer->fPosition.z == fMaxZ) {
							objects[i] = nullptr;
							SprinklersOn = false;
							for (CPlayer* player : v_cPlayers)
							{
								if (!player || !player->isAlive())
									continue;

								TComponent* _prenderer = nullptr;

								if (player->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
									TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

									if (pRenderer->fPosition.x == renderer->fPosition.x && pRenderer->fPosition.z == renderer->fPosition.z)
									{
										player->setAlive(false);

										playerfallingSound->Play();
									}
								}

							}

							for (CPlayer* AI : v_cAI) {
								if (!AI || !AI->isAlive())
									continue;

								TComponent* _prenderer = nullptr;

								if (AI->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
									TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

									if (pRenderer->fPosition.x == renderer->fPosition.x && pRenderer->fPosition.z == renderer->fPosition.z)
										AI->setAlive(false);
								}
							}

							for (int i = 0; i < items.size(); i++)
							{
								TComponent* _iRenderer = nullptr;
								if (items[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, _iRenderer)) {
									TTransformComponent* iRenderer = (TTransformComponent*)_iRenderer;

									if (iRenderer->fPosition.x == renderer->fPosition.x && iRenderer->fPosition.z == renderer->fPosition.z)
										items.erase(items.begin() + i);
								}
							}
							objects.erase(objects.begin() + i);
						}

					}

				}
			}


			//if (fMinX < -7.5) {
			fMinX += 2.5;
			fMinZ += 2.5;
			fMaxX -= 2.5;
			fMaxZ -= 2.5;
			//}

			mapTime = 0;
		}

		if (objects.size() > 0)
		{
			for (int i = 0; i < objects.size() - 1; ++i) {
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

		//screenshake
		if (bombExploded) {
			shakeTime += timePassed;

			viewPos = g_d3dData->screenShake();

			if (shakeTime >= 0.5 || isPaused) {
				bombExploded = false;
				if (!bombExploded) {
					g_d3dData->resetCamera();
					shakeTime = 0;
				}
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

		//Render AI
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

		//Render Emitter
		if (objects.size() > 0)
		{
			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;
			if (objects[objects.size() - 1]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;

				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(objects[objects.size() - 1]);
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


		if (!p_cRendererManager->Draw(timePassed, curGameState, this))
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
	loadInfo.position = { -10, 0, 10 };
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
	loadInfo.forwardVec = { 0.0f, 0.0f, 1.0f };
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.position = { 0.0f, 0.0f, 2.5f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

void CGame::Cleanup()
{
	delete p_cEntityManager;
	delete menuBomb;
	menuBomb = nullptr;
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
		if (object)
			delete object;
		object = nullptr;
	}
	menuObjects.clear();

	for (CBomb* bomb : v_cBombs)
	{
		if (bomb)
			delete bomb;
		bomb = nullptr;
	}
	v_cBombs.clear();
	for (CPlayer* player : v_cPlayers)
	{
		if (player)
			delete player;
		player = nullptr;
	}
	v_cPlayers.clear();
	for (CObject* exp : Xexplosions)
	{
		if (exp)
			delete exp;
		exp = nullptr;
	}
	Xexplosions.clear();
	for (CObject* exp : Zexplosions)
	{
		if (exp)
			delete exp;
		exp = nullptr;
	}
	Zexplosions.clear();
	explosionTimers.clear();

}

CGame::CGame()
{

}

CGame::~CGame()
{
	delete p_cRendererManager;
}


void CGame::WindowResize() {

	g_pWindow->GetClientWidth(g_d3dData->windowWidthHeight.x);
	g_pWindow->GetClientHeight(g_d3dData->windowWidthHeight.y);

	g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), static_cast<float>(g_d3dData->windowWidthHeight.x) / static_cast<float>(g_d3dData->windowWidthHeight.y), 0.1f, 100.0f);
	GW::SYSTEM::GWindowStyle style = (FullScreen) ? GW::SYSTEM::GWindowStyle::FULLSCREENBORDERLESS : GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED;
	g_pWindow->ChangeWindowStyle(style);

	g_pWindow->GetClientWidth(g_d3dData->windowWidthHeight.x);
	g_pWindow->GetClientHeight(g_d3dData->windowWidthHeight.y);

	g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), static_cast<float>(g_d3dData->windowWidthHeight.x) / static_cast<float>(g_d3dData->windowWidthHeight.y), 0.1f, 100.0f);
	g_d3dData->d3dViewport.Width = static_cast<float>(g_d3dData->windowWidthHeight.x);
	g_d3dData->d3dViewport.Height = static_cast<float>(g_d3dData->windowWidthHeight.y);
}

void CGame::GamePlayLoop(double timePassed)
{
	int width;
	int height;
	if (fMaxX == 15.0f)
	{
		width = (((fMaxX - 2.5f) - (fMinX + 2.5f)) / 2.5f) + 1;
		height = (((fMaxZ - 2.5f) - (fMinZ + 2.5f)) / 2.5f) + 1;
	}
	else
	{
		width = (((fMaxX)-(fMinX)) / 2.5f) + 1;
		height = (((fMaxZ)-(fMinZ)) / 2.5f) + 1;
	}
	int gridsize = width * height;

	std::vector<int> GRID;
	GRID.resize(gridsize);

	for (int i = 0; i < GRID.size(); ++i)
	{
		int z = i / width;
		int x = i % width;
		float zpos;
		float xpos;
		if (fMaxX == 15.0f)
		{
			zpos = (fMaxZ - 2.5f) - (float(z) * 2.5f);
			xpos = (fMaxX - 2.5f) - (float(x) * 2.5f);
		}
		else
		{
			zpos = fMaxZ - (float(z) * 2.5f);
			xpos = fMaxX - (float(x) * 2.5f);
		}
		bool filled = false;

		for (CBomb* bomb : v_cBombs)
		{
			if (!bomb || !bomb->isAlive())
				continue;

			TComponent* pComponent;
			bomb->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
			TTransformComponent* bTransform = (TTransformComponent*)pComponent;

			float dX = abs(bTransform->fPosition.x - xpos);
			float dZ = abs(bTransform->fPosition.z - zpos);

			if (dX < 1.25f && dZ < 1.25f)
			{
				GRID.at(i) = GRID_SYSTEM::BOMB;
				filled = true;
				break;
			}
		}

		if (filled)
		{
			continue;
		}

		for (CObject* Xexplode : Xexplosions)
		{
			if (!Xexplode)
				continue;

			TComponent* pComponent;
			Xexplode->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
			TTransformComponent* bTransform = (TTransformComponent*)pComponent;

			float dX = abs(bTransform->fPosition.x - xpos);
			float dZ = abs(bTransform->fPosition.z - zpos);

			if (dX < 1.25f && dZ < 1.25f)
			{
				GRID.at(i) = GRID_SYSTEM::BOMB;
				filled = true;
				break;
			}
		}

		if (filled)
		{
			continue;
		}

		for (CPlayer* player : v_cPlayers)
		{
			if (!player || !player->isAlive())
				continue;

			TComponent* pComponent;
			player->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
			TTransformComponent* pTransform = (TTransformComponent*)pComponent;

			float dX = abs(pTransform->fPosition.x - xpos);
			float dZ = abs(pTransform->fPosition.z - zpos);

			if ((dX < 1.25f && dZ < 1.25f) && player->isAlive())
			{
				GRID.at(i) = GRID_SYSTEM::PLAYER;
				filled = true;
				break;
			}
		}

		if (filled)
		{
			continue;
		}

		for (CObject* pow : items)
		{
			if (!pow)
				continue;

			TComponent* pComponent;
			pow->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
			TTransformComponent* iTransform = (TTransformComponent*)pComponent;

			float dX = abs(iTransform->fPosition.x - xpos);
			float dZ = abs(iTransform->fPosition.z - zpos);

			if (dX < 1.25f && dZ < 1.25f)
			{
				GRID.at(i) = GRID_SYSTEM::POWERUP;
				filled = true;
				break;
			}
		}

		if (filled)
		{
			continue;
		}

		for (CObject* obj : objects)
		{
			TComponent* pComponent;
			obj->GetComponent(COMPONENT_TYPE::TRANSFORM, pComponent);
			TTransformComponent* oTransform = (TTransformComponent*)pComponent;

			float dX = abs(oTransform->fPosition.x - xpos);
			float dZ = abs(oTransform->fPosition.z - zpos);

			if ((dX < 1.25f && dZ < 1.25f) && !oTransform->nFloor && !oTransform->destroyable)
			{
				GRID.at(i) = GRID_SYSTEM::CRATE;
				filled = true;
				break;
			}
			else if ((dX < 1.25f && dZ < 1.25f) && oTransform->destroyable)
			{
				GRID.at(i) = GRID_SYSTEM::DESTROYABLE;
				filled = true;
				break;
			}
		}

		if (!filled)
		{
			GRID.at(i) = GRID_SYSTEM::FREE;
		}

	}

	for (int i = 0; i < GRID.size(); i++)
	{
		if (GRID[i] == GRID_SYSTEM::BOMB)
		{
			if (i - 1 < GRID.size() && i - 1 > 0)
				GRID[i - 1] = GRID_SYSTEM::EXPLOSION_RADIUS;
			if (i - width < GRID.size() && i - width > 0)
				GRID[i - width] = GRID_SYSTEM::EXPLOSION_RADIUS;
			if (i + 1 < GRID.size() && i + 1 > 0)
				GRID[i + 1] = GRID_SYSTEM::EXPLOSION_RADIUS;
			if (i + width < GRID.size() && i + width > 0)
				GRID[i + width] = GRID_SYSTEM::EXPLOSION_RADIUS;

		}
	}

	for (CPlayer* currAI : v_cAI)
	{
		if (!currAI || !currAI->isAlive())
			continue;

		float deltaX = 0.0f, deltaZ = 0.0f;
		AIbombaction += timePassed;

		TComponent* AIComponent;
		currAI->GetComponent(COMPONENT_TYPE::TRANSFORM, AIComponent);
		TTransformComponent* AITransform = (TTransformComponent*)AIComponent;

		//float dX = abs(AITransform->fPosition.x - xpos);
		//float dZ = abs(AITransform->fPosition.z - zpos);
		int x = 0;
		int z = 0;
		float dec = 0;
		if (fMaxX == 15.0f)
		{
			//dec = abs(AITransform->fPosition.x) / 2.5f;
			//x = dec;
			//dec = dec - x;
			dec = abs((fMaxX - 2.5f) - AITransform->fPosition.x) / 2.5f;
			dec += 1.0f;
			x = dec;
			//if (dec > 0.0f)
			//	x = abs((fMaxX - 2.5f) - AITransform->fPosition.x + 2.5f) / 2.5f;
			if (x > width - 1)
				x = width - 1;

			//dec = abs(AITransform->fPosition.z) / 2.5f;
			//z = dec;
			//dec = dec - z;
			dec = abs((fMaxZ - 2.5f) - AITransform->fPosition.z) / 2.5f;
			dec += 1.0f;
			z = dec;
			//if (dec > 0.0f)
			//	z = abs((fMaxZ - 2.5f) - AITransform->fPosition.z + 2.5f) / 2.5f;
			if (z > height - 1)
				z = height - 1;
		}
		else
		{
			//dec = abs(AITransform->fPosition.x) / 2.5f;
			//x = dec;
			//dec = dec - x;
			dec = abs(fMaxX - AITransform->fPosition.x) / 2.5f;
			dec += 1.0f;
			x = dec;
			//if (dec > 0.0f)
			//	x = abs(fMaxX - AITransform->fPosition.x + 2.5f) / 2.5f;
			if (x > width - 1)
				x = width - 1;

			//dec = abs(AITransform->fPosition.z) / 2.5f;
			//z = dec;
			//dec = dec - z;
			dec = abs(fMaxZ - AITransform->fPosition.z) / 2.5f;
			dec += 1.0f;
			z = dec;
			//if (dec > 0.0f)
			//	z = abs(fMaxZ - AITransform->fPosition.z + 2.5f) / 2.5f;
			if (z > height - 1)
				z = height - 1;

		}

		for (CObject* cObj : objects) {
			if (currAI->Collides(cObj))
				PlayerCollision(currAI, cObj, deltaX, deltaZ);
		}

		bool breakout = false;
		int gridlocation = (z * width) + x;
		if (gridlocation < GRID.size())
		{

			if (GRID.at(gridlocation) == GRID_SYSTEM::BOMB)
			{
				int tile = GRID[gridlocation];
				//currAI->Move(((x / ((width - 1) / 2)) - 1) * timePassed * PLAYER_SPEED, ((z / ((height - 1) / 2)) - 1) * timePassed * PLAYER_SPEED);
				for (int dZ = -1; dZ <= 1; dZ++)
				{
					for (int dX = -1; dX <= 1; dX++)
					{
						bool zbounds = true;
						bool xbounds = true;
						int zchange;
						int xchange;
						zchange = z + dZ;
						xchange = x + dX;

						if (zchange < 0 || zchange > height - 1)
						{
							zbounds = false;
							zchange = z;
						}

						if (xchange < 0 || xchange > width - 1)
						{
							xbounds = false;
							xchange = x;
						}
						gridlocation = (zchange * width) + xchange;
						tile = GRID[gridlocation];
						zchange -= z;
						xchange -= x;
						if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
						{

							deltaX = timePassed * PLAYER_SPEED * -xchange;
							deltaZ = timePassed * PLAYER_SPEED * -zchange;
							currAI->Move(deltaX, deltaZ);
							for (CObject* cObj : objects) {
								if (currAI->Collides(cObj))
									PlayerCollision(currAI, cObj, deltaX, deltaZ);
							}
						}
					}
				}
			}

			if (GRID.at(gridlocation) == GRID_SYSTEM::EXPLOSION_RADIUS)
			{
				int tile = GRID[gridlocation];
				//currAI->Move(((x / ((width - 1) / 2)) - 1) * timePassed * PLAYER_SPEED, ((z / ((height - 1) / 2)) - 1) * timePassed * PLAYER_SPEED);
				for (int dZ = -1; dZ <= 1; dZ++)
				{
					for (int dX = -1; dX <= 1; dX++)
					{
						bool zbounds = true;
						bool xbounds = true;
						int zchange;
						int xchange;
						zchange = z + dZ;
						xchange = x + dX;

						if (zchange < 0 || zchange > height - 1)
						{
							zbounds = false;
							zchange = z;
						}

						if (xchange < 0 || xchange > width - 1)
						{
							xbounds = false;
							xchange = x;
						}
						gridlocation = (zchange * width) + xchange;
						tile = GRID[gridlocation];
						zchange -= z;
						xchange -= x;
						if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
						{

							deltaX = timePassed * PLAYER_SPEED * -xchange;
							deltaZ = timePassed * PLAYER_SPEED * -zchange;
							currAI->Move(deltaX, deltaZ);
							for (CObject* cObj : objects) {
								if (currAI->Collides(cObj))
									PlayerCollision(currAI, cObj, deltaX, deltaZ);
							}
						}
					}
				}
			}


			for (int gridcheck = 0; gridcheck <= 5; gridcheck++)
			{
				for (int dZ = -1; dZ <= 1; ++dZ)
				{
					for (int dX = -1; dX <= 1; ++dX)
					{
						bool zbounds = true;
						bool xbounds = true;
						int zchange;
						int xchange;
						zchange = z + dZ;
						xchange = x + dX;

						if (zchange < 0 || zchange > height - 1)
						{
							zbounds = false;
							zchange = z;
						}

						if (xchange < 0 || xchange > width - 1)
						{
							xbounds = false;
							xchange = x;
						}

						gridlocation = ((zchange)* width) + (xchange);
						zchange -= z;
						xchange -= x;

						//if (AITransform->fPosition.x < 0.0f)
						//	xchange *= -1.0f;
						//
						//if (AITransform->fPosition.z < 0.0f)
						//	zchange *= -1.0f;

						if (gridlocation < GRID.size())
						{
							int tile = GRID[gridlocation];
							if (gridcheck == 0 || gridcheck == 1)
							{
								if (tile == GRID_SYSTEM::BOMB /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
								{
									deltaX = timePassed * PLAYER_SPEED * xchange;
									deltaZ = timePassed * PLAYER_SPEED * zchange;
									currAI->Move(deltaX, deltaZ);
									for (CObject* cObj : objects) {
										if (currAI->Collides(cObj))
											PlayerCollision(currAI, cObj, deltaX, deltaZ);
									}

									for (int dZ = -1; dZ <= 1; ++dZ)
									{
										for (int dX = -1; dX <= 1; ++dX)
										{
											zbounds = true;
											xbounds = true;
											zchange = 0;
											xchange = 0;
											zchange = z + dZ;
											xchange = x + dX;

											if (zchange < 0 || zchange > height - 1)
											{
												zbounds = false;
												zchange = z;
											}

											if (xchange < 0 || xchange > width - 1)
											{
												xbounds = false;
												xchange = x;
											}

											gridlocation = ((zchange)* width) + (xchange);
											zchange -= z;
											xchange -= x;

											if (gridlocation < GRID.size())
											{
												int tile = GRID[gridlocation];
												if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
												{

													deltaX = timePassed * PLAYER_SPEED * -xchange;
													deltaZ = timePassed * PLAYER_SPEED * -zchange;
													currAI->Move(deltaX, deltaZ);
													for (CObject* cObj : objects) {
														if (currAI->Collides(cObj))
															PlayerCollision(currAI, cObj, deltaX, deltaZ);
													}
												}
											}
										}
									}
								}
							}
							else if (gridcheck == 2)
							{
								if (tile == GRID_SYSTEM::POWERUP /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
								{

									deltaX = timePassed * PLAYER_SPEED * -xchange;
									deltaZ = timePassed * PLAYER_SPEED * -zchange;
									currAI->Move(deltaX, deltaZ);
									for (CObject* cObj : objects) {
										if (currAI->Collides(cObj))
											PlayerCollision(currAI, cObj, deltaX, deltaZ);
									}
									for (int dZ = -1; dZ <= 1; ++dZ)
									{
										for (int dX = -1; dX <= 1; ++dX)
										{
											zbounds = true;
											xbounds = true;
											zchange = 0;
											xchange = 0;
											zchange = z + dZ;
											xchange = x + dX;

											if (zchange < 0 || zchange > height - 1)
											{
												zbounds = false;
												zchange = z;
											}

											if (xchange < 0 || xchange > width - 1)
											{
												xbounds = false;
												xchange = x;
											}

											gridlocation = ((zchange)* width) + (xchange);
											zchange -= z;
											xchange -= x;

											if (gridlocation < GRID.size())
											{
												int tile = GRID[gridlocation];
												if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
												{

													deltaX = timePassed * PLAYER_SPEED * -xchange;
													deltaZ = timePassed * PLAYER_SPEED * -zchange;
													currAI->Move(deltaX, deltaZ);
													for (CObject* cObj : objects) {
														if (currAI->Collides(cObj))
															PlayerCollision(currAI, cObj, deltaX, deltaZ);
													}
												}
											}
										}
									}
									gridcheck = 5;
								}
							}

							else if (gridcheck == 3 && AIbombaction >= 4.0f)
							{
								if (tile == GRID_SYSTEM::DESTROYABLE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
								{
									if (currAI->hasAvailableBombSlot())
									{
										AIbombaction = 0.0f;
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
												currAI->AddBombIndex(i);
												std::vector<CBomb*> bombs;

												switch (currAI->GetBombType()) {

												case 4:
													if (v_cBombs[i]) {
														bombs = p_cEntityManager->DropBomb0(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}
													else {
														bombs = p_cEntityManager->DropBomb0(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}

													break;
												case 1:
													if (v_cBombs[i]) {
														bombs = p_cEntityManager->DropBomb1(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}
													else {
														bombs = p_cEntityManager->DropBomb1(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}

													break;
												case 2:
													if (v_cBombs[i]) {
														bombs = p_cEntityManager->DropBomb2(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}
													else {
														bombs = p_cEntityManager->DropBomb2(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}

													break;
												case 3:
													if (v_cBombs[i]) {
														bombs = p_cEntityManager->DropBomb3(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}
													else {
														bombs = p_cEntityManager->DropBomb3(currAI, objects);
														for (int j = 0; j < bombs.size(); j++) {
															v_cBombs[i + j] = bombs[j];
														}
													}

													break;
												default:
													if (v_cBombs[i])
														*v_cBombs[i] = *p_cEntityManager->DropBomb(currAI);
													else
														v_cBombs[i] = p_cEntityManager->DropBomb(currAI);
													break;
												}

												break;
											}
										}
									}
								}
							}
							else if (gridcheck == 4)
							{
								if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
								{

									deltaX = timePassed * PLAYER_SPEED * -xchange;
									deltaZ = timePassed * PLAYER_SPEED * -zchange;
									currAI->Move(deltaX, deltaZ);
									for (CObject* cObj : objects) {
										if (currAI->Collides(cObj))
											PlayerCollision(currAI, cObj, deltaX, deltaZ);
									}
								}
							}
						}
					}
				}
			}


		}
		else if (gridlocation >= GRID.size())
		{
			if (x >= width - 1)
			{
				x = width - 1;
			}
			if (z >= height - 1)
			{
				z = height - 1;
			}
			gridlocation = (z * width) + x;

			if (GRID.at(gridlocation) == GRID_SYSTEM::BOMB)
			{
				int tile = GRID[gridlocation];

				//currAI->Move(((x / ((width - 1) / 2)) - 1) * timePassed * PLAYER_SPEED, ((z / ((height - 1) / 2)) - 1) * timePassed * PLAYER_SPEED);
				for (int dZ = -1; dZ <= 1; dZ++)
				{
					for (int dX = -1; dX <= 1; dX++)
					{
						bool zbounds = true;
						bool xbounds = true;
						int zchange;
						int xchange;
						zchange = z + dZ;
						xchange = x + dX;

						if (zchange < 0 || zchange > height - 1)
						{
							zbounds = false;
							zchange = z;
						}

						if (xchange < 0 || xchange > width - 1)
						{
							xbounds = false;
							xchange = x;
						}
						if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
						{


							deltaX = timePassed * PLAYER_SPEED * -dX;
							deltaZ = timePassed * PLAYER_SPEED * -dZ;
							currAI->Move(deltaX, deltaZ);
							for (CObject* cObj : objects) {
								if (currAI->Collides(cObj))
									PlayerCollision(currAI, cObj, deltaX, deltaZ);
							}
						}
					}
				}
			}

			else
			{
				for (int gridcheck = 0; gridcheck < 5; gridcheck++)
				{
					for (int dZ = -1; dZ <= 1; ++dZ)
					{
						for (int dX = -1; dX <= 1; ++dX)
						{
							bool zbounds = true;
							bool xbounds = true;
							int zchange;
							int xchange;
							zchange = z + dZ;
							xchange = x + dX;

							if (zchange < 0 && zchange > height - 1)
							{
								zbounds = false;
								zchange = z;
							}

							if (xchange < 0 && xchange > width - 1)
							{
								xbounds = false;
								xchange = x;
							}

							gridlocation = ((zchange)* width) + (xchange);
							zchange -= z;
							xchange -= x;

							//if (AITransform->fPosition.x < 0.0f)
							//	xchange *= -1.0f;
							//
							//if (AITransform->fPosition.z < 0.0f)
							//	zchange *= -1.0f;

							if (gridlocation < GRID.size())
							{
								int tile = GRID[gridlocation];
								if (gridcheck == 0)
								{
									if (tile == GRID_SYSTEM::BOMB && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
									{
										deltaX = timePassed * PLAYER_SPEED * xchange;
										deltaZ = timePassed * PLAYER_SPEED * zchange;
										currAI->Move(deltaX, deltaZ);
										for (CObject* cObj : objects) {
											if (currAI->Collides(cObj))
												PlayerCollision(currAI, cObj, deltaX, deltaZ);
										}

										for (int dZ = -1; dZ <= 1; ++dZ)
										{
											for (int dX = -1; dX <= 1; ++dX)
											{
												zbounds = true;
												xbounds = true;
												zchange = 0;
												xchange = 0;
												zchange = z + dZ;
												xchange = x + dX;

												if (zchange < 0 && zchange > height - 1)
												{
													zbounds = false;
													zchange = z;
												}

												if (xchange < 0 && xchange > width - 1)
												{
													xbounds = false;
													xchange = x;
												}

												gridlocation = ((zchange)* width) + (xchange);
												zchange -= z;
												xchange -= x;

												if (gridlocation < GRID.size())
												{
													int tile = GRID[gridlocation];
													if (tile == GRID_SYSTEM::FREE  && xbounds && zbounds)
													{

														deltaX = timePassed * PLAYER_SPEED * -xchange;
														deltaZ = timePassed * PLAYER_SPEED * -zchange;
														currAI->Move(deltaX, deltaZ);
														for (CObject* cObj : objects) {
															if (currAI->Collides(cObj))
																PlayerCollision(currAI, cObj, deltaX, deltaZ);
														}
													}
												}
											}
										}
									}
								}


								else if (gridcheck == 1 && AIbombaction >= 4.0f)
								{
									if (tile == GRID_SYSTEM::DESTROYABLE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
									{
										if (currAI->hasAvailableBombSlot())
										{
											AIbombaction = 0.0f;
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
													currAI->AddBombIndex(i);
													std::vector<CBomb*> bombs;

													switch (currAI->GetBombType()) {

													case 4:
														if (v_cBombs[i]) {
															bombs = p_cEntityManager->DropBomb0(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}
														else {
															bombs = p_cEntityManager->DropBomb0(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}

														break;
													case 1:
														if (v_cBombs[i]) {
															bombs = p_cEntityManager->DropBomb1(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}
														else {
															bombs = p_cEntityManager->DropBomb1(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}

														break;
													case 2:
														if (v_cBombs[i]) {
															bombs = p_cEntityManager->DropBomb2(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}
														else {
															bombs = p_cEntityManager->DropBomb2(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}

														break;
													case 3:
														if (v_cBombs[i]) {
															bombs = p_cEntityManager->DropBomb3(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}
														else {
															bombs = p_cEntityManager->DropBomb3(currAI, objects);
															for (int j = 0; j < bombs.size(); j++) {
																v_cBombs[i + j] = bombs[j];
															}
														}

														break;
													default:
														if (v_cBombs[i])
															*v_cBombs[i] = *p_cEntityManager->DropBomb(currAI);
														else
															v_cBombs[i] = p_cEntityManager->DropBomb(currAI);
														break;
													}

													break;
												}
											}
										}
									}
								}
								else if (gridcheck == 2)
								{
									if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
									{

										deltaX = timePassed * PLAYER_SPEED * -xchange;
										deltaZ = timePassed * PLAYER_SPEED * -zchange;
										currAI->Move(deltaX, deltaZ);
										for (CObject* cObj : objects) {
											if (currAI->Collides(cObj))
												PlayerCollision(currAI, cObj, deltaX, deltaZ);
										}
									}
								}
							}
						}
					}
				}
			}
		}



		for (CObject* cObj : objects) {
			if (currAI->Collides(cObj))
				PlayerCollision(currAI, cObj, deltaX, deltaZ);
		}

		TComponent* _prenderer = nullptr;
		TComponent* _brenderer = nullptr;

		for (CBomb* bomb : v_cBombs) {
			if (bomb && bomb->isAlive()) {
				if (bomb->getTimer() >= 0.5f) {
					if (currAI->Collides(bomb)) {
						PlayerBombCollision(currAI, bomb);
					}
				}
			}
		}
		for (int i = 0; i < items.size(); i++)
		{
			if (currAI->Collides((CObject*)items[i]))
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
				currAI->SetBombType(items[i]->GetItemType());
				if (currAI->GetNumBombs() < 3)
				{
					currAI->incNumBombs();
				}

				items.erase(items.begin() + i);
				--i;
			}
		}

		if (currAI->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer))
		{
			TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

			if (pRenderer->fPosition.x < fMinX - 1.3 || pRenderer->fPosition.x > fMaxX + 1.3 || pRenderer->fPosition.z < fMinZ - 1.3 || pRenderer->fPosition.z > fMaxZ + 1.3)
			{
				offMapTimer += timePassed;

				if (offMapTimer >= 0.25)
				{
					offMapTimer = 0;
					currAI->setAlive(false);
				}
			}
		}

	}

	for (CPlayer* currPlayer : v_cPlayers)
	{
		if (!currPlayer || !currPlayer->isAlive())
			continue;

		currPlayer->GetInput();

		CharacterController* cont = currPlayer->GetCharacterController();

		float deltaX = 0.0f, deltaZ = 0.0f;

		if (currPlayer->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::PAUSE) && !ControlScreenToggle)
		{
			
				int previndex = menuIndex;
				menuIndex = 0;
				previndex = menuIndex - previndex;
				pauseMenuBomb->Move(0.0f, (-(float)previndex*1.2f), false);
				PauseMenuToggle = !PauseMenuToggle;
				isPaused = !isPaused;
		}



		if (currNumControllers < prevNumControllers || currNumControllers == 0)
		{
			PauseMenuToggle = true;
			isPaused = true;
		}
		g_pControllerInput->GetNumConnected(prevNumControllers);
		if (currNumControllers > prevNumControllers)
		{
			PauseMenuToggle = false;
			isPaused = false;
		}

		if (cont->IsControllerConnected())
		{
			deltaX = cont->GetLeftRight() * timePassed * PLAYER_SPEED;
			deltaZ = cont->GetUpDown() * timePassed * PLAYER_SPEED;
		}
		else
		{
			if (cont->ButtonHeld(DEFAULT_BUTTONS::LEFT))
				deltaX -= 1.0f * timePassed	 * PLAYER_SPEED;
			if (cont->ButtonHeld(DEFAULT_BUTTONS::RIGHT))
				deltaX += 1.0f * timePassed * PLAYER_SPEED;
			if (cont->ButtonHeld(DEFAULT_BUTTONS::UP))
				deltaZ += 1.0f * timePassed * PLAYER_SPEED;
			if (cont->ButtonHeld(DEFAULT_BUTTONS::DOWN))
				deltaZ -= 1.0f * timePassed * PLAYER_SPEED;
		}


		if (deltaX != 0.0f || deltaZ != 0.0f)
		{
			if (currPlayer->GetCrouchStatus() == false)
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
		}

		/*	if (deltaX != 0.0f || deltaZ != 0.0f)
			{
				if (currPlayer->GetCrouchStatus() == true)
				{

				}
			}*/

			//CROUCH
		if (currPlayer->GetCharacterController()->ButtonPressed(DEFAULT_BUTTONS::CROUCH) && !isPaused) {

			currPlayer->CrouchRoll(0, 0, -1.5f, false);
			currPlayer->ChangeCrouchStatus();
		}
		if (currPlayer->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::CROUCH) && !isPaused) {

			currPlayer->CrouchRoll(0, 0, 1.5f, false);
			currPlayer->ChangeCrouchStatus();
		}

		for (CObject* cObj : objects) {
			if (currPlayer->Collides(cObj))
				PlayerCollision(currPlayer, cObj, deltaX, deltaZ);
		}
		TComponent* _prenderer = nullptr;
		TComponent* _brenderer = nullptr;
		for (CBomb* bomb : v_cBombs)
		{
			if (bomb && bomb->isAlive())
				if (bomb->getTimer() >= 0.5f)
					if (currPlayer->Collides(bomb))
						PlayerBombCollision(currPlayer, bomb);
		}
		int bombindex = 0;
		for (CBomb* bomb : v_cBombs)
		{

			if (!bomb)
				continue;

			if (bomb->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer))
			{
				TTransformComponent* bRenderer = (TTransformComponent*)_prenderer;
				if (bRenderer->fPosition.x < fMinX || bRenderer->fPosition.x > fMaxX || bRenderer->fPosition.z < fMinZ || bRenderer->fPosition.z > fMaxZ)
				{
					if (bomb->isAlive())
					{
						CPlayer* parent = bomb->getParent();
						for (int j = 0; j < parent->getBombIndices().size(); ++j)
						{
							if (parent->getBombIndices()[j] == bombindex)
							{
								bomb->Cleanup();
								parent->deleteBomb(j);
							}
						}
						bomb->SetAlive(false);
					}
					bomb->updateBomb(timePassed);

				}
			}
			bombindex++;
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
				delete items[i];
				items[i] = nullptr;
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
					playerfallingSound->Play();
				}
			}
		}

		if (currPlayer->GetCharacterController()->ButtonPressed(DEFAULT_BUTTONS::ACTION) && !isPaused)
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

				int numBombsPlaced = 0;
				std::vector<CBomb*> bombs;

				switch (currPlayer->GetBombType()) {

				case 4:
					bombs = p_cEntityManager->DropBomb0(currPlayer, objects);
					break;
				case 1:
					bombs = p_cEntityManager->DropBomb1(currPlayer, objects);
					break;
				case 2:
					bombs = p_cEntityManager->DropBomb2(currPlayer, objects);

					break;
				case 3:
					bombs = p_cEntityManager->DropBomb3(currPlayer, objects);

					break;
				default:

					bombs.resize(1);
					bombs[0] = p_cEntityManager->DropBomb(currPlayer);
					break;
				}
				for (int j = 0; j < bombs.size(); ++j) {
					for (int i = 0; i < maxNumBombs; ++i)
					{

						if (v_cBombs[i] == nullptr || !v_cBombs[i]->isAlive())
						{
							v_cBombs[i] = bombs[j];
							currPlayer->AddBombIndex(i);

							break;
						}
					}
				}
				currPlayer->IncPlacedBombs();
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


				}
				if (currPlayer->GetCharacterController()->GetUpDown() < 0.0f && pauseMenuTimer > 4.0f && menuIndex < 2)
				{

					pauseMenuTimer = 0.0f;
					pauseMenuBomb->Move(0.0f, -1.2f, false);
					menuIndex += 1;


				}
			}
			if (currPlayer->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::ACTION))
			{
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

void CGame::InitSortedParticles(end::sorted_pool_t<particle, 500>& sortedPool, double deltaTime, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color) {
	//Init number of particles as loop
	for (size_t i = 0; i < 1; i++) {
		int count = sortedPool.alloc();

		if (count != -1) {
			sortedPool[count].pos.x = pos.x;
			sortedPool[count].pos.y = pos.y;
			sortedPool[count].pos.z = pos.z;
			sortedPool[count].speed.x = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX));
			sortedPool[count].speed.y = (0.0f + (2.0f - 0.0f) * ((float)rand() / (float)RAND_MAX));
			sortedPool[count].speed.z = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX));
			sortedPool[count].speed.y *= particleSpeed;
			sortedPool[count].timer = 1.5f + (1.8f - 1.5f) * ((float)rand() / (float)RAND_MAX);
			sortedPool[count].color = color;
		}
	}

	for (size_t i = 0; i < sortedPool.size(); i++) {
		//add_line(sortedPool[i].pos, sortedPool[i].prev_pos, sortedPool[i].color);
		sortedPool[i].prev_pos = sortedPool[i].pos;
		sortedPool[i].pos.x += (sortedPool[i].speed.x * deltaTime);
		sortedPool[i].pos.y += (sortedPool[i].speed.y * deltaTime);
		sortedPool[i].pos.z += (sortedPool[i].speed.z * deltaTime);
		sortedPool[i].speed.y -= particleGravity * deltaTime;
		sortedPool[i].timer -= deltaTime;
		add_line(sortedPool[i].pos, sortedPool[i].prev_pos, sortedPool[i].color);

		if (sortedPool[i].timer <= 0) {
			sortedPool.free(i);
			i--;
		}
	}
}

void CGame::InitFreeParticles(emitter& emitter, end::pool_t<particle, 1024>& freePool, double deltaTime) {
	//init emitters
	firstEmit.spawn_pos = { fMaxX,2.5f,fMaxZ - 2.5f };
	secondEmit.spawn_pos = { fMinX,2.5f,fMinZ - 2.5f };
	thirdEmit.spawn_pos = { fMinX,2.5f,fMaxZ - 2.5f };
	fourthEmit.spawn_pos = { fMaxX,2.5f,fMinZ - 2.5f };
	//firstEmit.spawn_color = { 1,0,0,1 };
	//secondEmit.spawn_color = { 1,0,0,1 };
	//thirdEmit.spawn_color = { 1,0,0,1 };

	//alloc space
	int count = freePool.alloc();
	int emitCount = emitter.indices.alloc();

	emitter.indices[emitCount] = count;
	freePool[count].pos = emitter.spawn_pos;
	freePool[count].prev_pos = emitter.spawn_pos;
	freePool[count].speed.x = (-5.0f + (5.0f - -5.0f) * ((float)rand() / (float)RAND_MAX));
	freePool[count].speed.y = (0.0f + (2.0f - 0.0f) * ((float)rand() / (float)RAND_MAX));
	freePool[count].speed.z = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX));
	freePool[count].speed.x *= particleSpeed;
	freePool[count].timer = ((float)rand() / (float)RAND_MAX);
	freePool[count].color = { 1,1,1,1 };

	for (size_t i = 0; i < emitter.indices.size(); i++) {
		int Ecount = emitter.indices[i];

		if (Ecount <= -1 || Ecount > 1024) {
			break;
		}

		if (Ecount != -1) {
			freePool[Ecount].timer -= deltaTime;

			if (freePool[Ecount].timer <= 0.0f) {
				freePool.free(Ecount);
				emitter.indices.free(i);
				i--;
				continue;
			}
			//add_line(freePool[Ecount].pos, freePool[Ecount].prev_pos, freePool[Ecount].color);
			freePool[Ecount].prev_pos = freePool[Ecount].pos;
			freePool[Ecount].pos.x += (freePool[Ecount].speed.x * deltaTime);
			freePool[Ecount].pos.y += (freePool[Ecount].speed.y * deltaTime);
			freePool[Ecount].pos.z += (freePool[Ecount].speed.z * deltaTime);
			freePool[Ecount].speed.y -= particleGravity * deltaTime;
			add_line(freePool[Ecount].pos, freePool[Ecount].prev_pos, freePool[Ecount].color);
		}
	}
}

void CGame::InitFreeParticles(emitter& emitter, end::pool_t<particle, 1024>& freePool, double deltaTime, CObject* obj) {
	TComponent* cRenderer = nullptr;
	TTransformComponent* renderer = nullptr;
	if (obj->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
	{
		renderer = (TTransformComponent*)cRenderer;

	}

	//init emitters
	freeEmit.spawn_pos = renderer->fPosition;

	//alloc space
	int count = freePool.alloc();
	int emitCount = emitter.indices.alloc();

	emitter.indices[emitCount] = count;
	freePool[count].pos = emitter.spawn_pos;
	freePool[count].prev_pos = emitter.spawn_pos;
	freePool[count].speed.x = (-5.0f + (5.0f - -5.0f) * ((float)rand() / (float)RAND_MAX));
	freePool[count].speed.y = (0.0f + (2.0f - 0.0f) * ((float)rand() / (float)RAND_MAX));
	freePool[count].speed.z = (-3.0f + (3.0f - -3.0f) * ((float)rand() / (float)RAND_MAX));
	freePool[count].speed.x *= particleSpeed;
	freePool[count].timer = ((float)rand() / (float)RAND_MAX);
	freePool[count].color = { 1,0,0,1 };

	for (size_t i = 0; i < emitter.indices.size(); i++) {
		int Ecount = emitter.indices[i];

		if (Ecount <= -1 || Ecount > 1024) {
			break;
		}

		if (Ecount != -1) {
			freePool[Ecount].timer -= deltaTime;

			if (freePool[Ecount].timer <= 0.0f) {
				freePool.free(Ecount);
				emitter.indices.free(i);
				i--;
				continue;
			}

			freePool[Ecount].prev_pos = freePool[Ecount].pos;
			freePool[Ecount].pos.x += (freePool[Ecount].speed.x * deltaTime);
			freePool[Ecount].pos.y += (freePool[Ecount].speed.y * deltaTime);
			freePool[Ecount].pos.z += (freePool[Ecount].speed.z * deltaTime);
			freePool[Ecount].speed.y -= particleGravity * deltaTime;
			//add_line(freePool[Ecount].pos, freePool[Ecount].prev_pos, freePool[Ecount].color);
		}
	}
}

void CGame::SpawnParticles(CObject* obj, double time, double timePassed) {
	for (double i = 0; i < time; i += timePassed)
	{
		InitFreeParticles(freeEmit, shared_pool, timePassed, obj);
	}
}

void CGame::setGameState(int _gameState)
{
	switch (_gameState)
	{
	case GAME_STATE::MAIN_MENU:
	{
		p1Pause.Reset(false);
		ClearPlayersAndBombs();
		g_pMusicStream->isStreamPlaying(soundplaying);
		if (!soundplaying)
			g_pMusicStream->ResumeStream();
		menuBomb = p_cEntityManager->InstantiatePlayer(1, MODELS::BOMB, DIFFUSE_TEXTURES::BOMB4, DirectX::XMFLOAT3(-1.5f, 11.4f, -6.8f), GAME_STATE::MAIN_MENU, DirectX::XMFLOAT3(0.0f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f));
		SprinklersOn = false;
		break;
	}
	case GAME_STATE::ARCADE_MENU:
	{
		p1Pause.Reset(false);
		ClearPlayersAndBombs();
		delete menuBomb;
		menuBomb = nullptr;
		menuBomb = p_cEntityManager->InstantiatePlayer(1, MODELS::BOMB, DIFFUSE_TEXTURES::BOMB, DirectX::XMFLOAT3(-4.35f, 11.9f, -5.6f), GAME_STATE::ARCADE_MENU, DirectX::XMFLOAT3(0.0f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f));
		SprinklersOn = false;
		break;
	}
	case GAME_STATE::ARCADE_GAME:
	{
		SprinklersOn = true;
		passes = 0;
		delete menuBomb;
		shakeTime = 0;
		menuBomb = nullptr;
		LoadObject();
		v_cPlayers[0] = p_cEntityManager->InstantiatePlayer(1, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(-12.5f, 0.0f, 12.5f));
		v_cPlayers[1] = p_cEntityManager->InstantiatePlayer(2, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(12.5f, 0.0f, -7.5f));

		if (numPLAYERS > 2)
			v_cPlayers[2] = p_cEntityManager->InstantiatePlayer(3, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));
		if (numPLAYERS > 3)
			v_cPlayers[3] = p_cEntityManager->InstantiatePlayer(4, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(-12.5f, 0.0f, -7.5f));
		if (numAI > 0)
			v_cAI[1] = p_cEntityManager->InstantiatePlayer(4, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(-12.5f, 0.0f, -7.5f));
		if (numAI > 1)
			v_cAI[0] = p_cEntityManager->InstantiatePlayer(3, MODELS::CHICKEN, DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(12.5f, 0.0f, 12.5f));
		fMinX = -15;
		fMaxX = 15;
		fMinZ = -10;
		fMaxZ = 15;
		mapTime = 0;


		spawnSound1->isSoundPlaying(soundplaying);
		if (!soundplaying)
			spawnSound1->Play();

		spawnSound2->isSoundPlaying(soundplaying);
		if (!soundplaying)
			spawnSound2->Play();

		v_cBombs.resize(maxNumBombs);
		break;
	}
	case GAME_STATE::WIN_SCREEN:
	{
		ClearPlayersAndBombs();
		break;
	}
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
	for (CBomb* bomb : v_cBombs)
	{
		if (bomb)
			delete bomb;
		bomb = nullptr;
	}

	v_cBombs.clear();


	for (int i = 0; i < v_cAI.size(); ++i)
		if (v_cAI[i])
		{
			v_cAI[i]->Cleanup();
			delete v_cAI[i];
			v_cAI[i] = nullptr;
		}


	for (CObject* object : objects)
	{
		if (object)
			delete object;
		object = nullptr;
	}
	for (CObject* exp : Xexplosions)
	{
		if (exp)
			delete exp;
		exp = nullptr;
	}
	Xexplosions.clear();
	for (CObject* exp : Zexplosions)
	{
		if (exp)
			delete exp;
		exp = nullptr;
	}
	for (CItem* item : items)
	{
		if (item)
			delete item;
		item = nullptr;
	}
	Zexplosions.clear();
	explosionTimers.clear();
	objects.clear();
	items.clear();
	shakeTime = 0;
}

void CGame::updateBombs(double timePassed)
{
	for (int i = 0; i < explosionTimers.size(); ++i)
	{
		if (explosionTimers[i] >= 0.3f)
		{
			delete Xexplosions[i];
			Xexplosions.erase(Xexplosions.begin() + i);
			delete Zexplosions[i];
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

					switch (rand() % 10) {
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
					default:
						objects.erase(objects.begin() + j);
						--j;
						break;

					}
				}
			}
		}

		TComponent* Xplo = nullptr;
		TComponent* Zplo = nullptr;
		TTransformComponent* XexplosionTrans;
		XexplosionTrans = nullptr;
		TTransformComponent* ZexplosionTrans;
		ZexplosionTrans = nullptr;

		for (CPlayer* player : v_cPlayers) {
			if (player) {

				if (Xexplosions[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, Xplo)) {
					XexplosionTrans = (TTransformComponent*)Xplo;
				}
				if (Zexplosions[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, Zplo)) {
					ZexplosionTrans = (TTransformComponent*)Zplo;
				}
				if (Xexplosions[i]->Collides((CObject*)player) || Zexplosions[i]->Collides((CObject*)player))
				{
					if (player->GetCrouchStatus() == false || XexplosionTrans->fPosition.y == 0 || ZexplosionTrans->fPosition.y == 0)
					{
						player->setAlive(false);
						DeathSound->Play();
					}
				}
			}

		}

		for (CPlayer* AI : v_cAI) {
			if (AI) {
				if (Xexplosions[i]->Collides((CObject*)AI) || Zexplosions[i]->Collides((CObject*)AI))
				{
					if (AI->GetCrouchStatus() == false || XexplosionTrans->fPosition.y == 0 || ZexplosionTrans->fPosition.y == 0)
					{
						AI->setAlive(false);
						DeathSound->Play();
					}
				}
			}
		}



		for (int k = 0; k < v_cBombs.size(); k++)
		{
			if (v_cBombs[k] && v_cBombs[k]->isAlive())
			{
				if (Xexplosions[i]->Collides((CObject*)v_cBombs[k]) || Zexplosions[i]->Collides((CObject*)v_cBombs[k]))
				{
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
				bombExploded = true;

				TComponent* cRenderer = nullptr;
				TTransformComponent* Brenderer = nullptr;

				if (v_cBombs[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
				{
					Brenderer = (TTransformComponent*)cRenderer;
					bombPos = Brenderer->fPosition;
				}
				//SpawnParticles((CObject*)v_cBombs[i], particleLife, timer.Delta());

				if (v_cPlayers.at(0) && v_cPlayers.at(0)->isAlive())
					g_pControllerInput->StartVibration(0, 0.25f, 1, 0);
				if (v_cPlayers.at(1) && v_cPlayers.at(1)->isAlive())
					g_pControllerInput->StartVibration(0, 0.25f, 1, 1);
				if (v_cPlayers.at(2) && v_cPlayers.at(2)->isAlive())
					g_pControllerInput->StartVibration(0, 0.25f, 1, 2);
				if (v_cPlayers.at(3) && v_cPlayers.at(3)->isAlive())
					g_pControllerInput->StartVibration(0, 0.25f, 1, 3);

				CPlayer* parent = v_cBombs[i]->getParent();
				for (int j = 0; j < parent->getBombIndices().size(); ++j) {
					if (parent->getBombIndices()[j] == i) {
						parent->deleteBomb(j);
						parent->DecPlacedBombs();
					}
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

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::MAIN_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(2.55f, 2.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::MAIN_MENU;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::ARCADE_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(2.52f, 2.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::ARCADE_MENU;
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


	return true;
}

void CGame::PlayerCollision(CPlayer * playerToCheck, CObject* cObj, float dx, float dz)
{
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


	TTransformComponent* pTrans = nullptr;
	if (playerToCheck->GetComponent(COMPONENT_TYPE::TRANSFORM, comp))
		pTrans = (TTransformComponent*)comp;

	if (zD < mZ && zD > mZ - 0.2)
	{
		pTrans->fPosition.z += ((mZ - zD) + 0.1f) *(float)upDown;
		pCollider->d3dCollider.Center.z += ((mZ - zD) + 0.1f) *(float)upDown;
		//playerToCheck->Move(0, ((mZ - zD) + 0.1f) * (float)upDown);
	}
	else if (xD < mX && xD > mX - 0.2)
	{

		pTrans->fPosition.x += ((mX - xD)) *(float)leftRight;
		pCollider->d3dCollider.Center.x += ((mX - xD)) *(float)leftRight;
		//playerToCheck->Move(((mX - xD) + 0.1f) * (float)leftRight, 0);
	}

	pTrans->mObjMatrix.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
	pTrans->mObjMatrix *= DirectX::XMMatrixTranslation(pTrans->fPosition.x, pTrans->fPosition.y, pTrans->fPosition.z);
	//pTrans->ResetMatrix();
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