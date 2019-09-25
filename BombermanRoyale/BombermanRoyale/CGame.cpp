#include "CGame.h"
#include <DirectXCollision.h>
#include <iostream>
#include "pools.h"

#define PLAYER_SPEED 5.0f
#define AI_SPEED 5.0f

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
	enum { UP = 0, DOWN, LEFT, RIGHT, ZERO, RMB, SPACE, HELP_MENU, GAME_STATE, FULLSCREEN, PAUSE, DISC_TOG, AI_DEC_COUNT, AI_INC_COUNT, COUNT };
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
	VK_ESCAPE,
	'T',
	'U',
	'I',
};

bool ControlScreenToggle = false;
bool PauseMenuToggle = false;
bool Controller1Alive = false;
bool Controller2Alive = false;
bool soundplaying;
bool soundplaying2;
bool warningSoundPlaying = false;
bool fallingSoundPlaying = false;
bool playerfallingSoundPlaying = false;
bool loadHappened = false;
bool isPaused = false;
float centerAItimer1 = 2.0f;

static double timePassed = 0.0f;
static double loadScreenTime = 0.0f;
double AIbombaction = 0.0f;
double tempTime = 0.0f;
double tempMapTime = 0.0f;
double tempShakeTime = 0.0f;
double mapTime = 0.0f;
double shakeTime = 0.0f;
double offMapTimer = 0;

int menux = 0;
int menuz = 0;
int boxDropped;
int currLayer = 0;
int passes = 0;
int numPlayers = 2;
int currNumControllers = 0;
int prevNumControllers = 0;

float pauseMenuTimer;
float fMinX;
float fMaxX;
float fMinZ;
float fMaxZ;
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
		timer.Signal();
		if (!isPaused)
			mapTime += timePassed;


		loadScreenTime = timer.Delta() + loadScreenTime;
		if (loadScreenTime < 0.5)
		{
			setGameState(GAME_STATE::GP_SPLASH);
		}

		if (loadScreenTime > 4 && (curGameState == GAME_STATE::FS_SPLASH || curGameState == GAME_STATE::GP_SPLASH) && loadHappened == false)
		{
			setGameState(GAME_STATE::FS_SPLASH);
			
		}

		if (loadScreenTime > 8 && (curGameState == GAME_STATE::LOAD_SCREEN || curGameState == GAME_STATE::FS_SPLASH) && loadHappened == false)
		{
			setGameState(GAME_STATE::LOAD_SCREEN);

		}

		if (loadScreenTime > 12 && (curGameState == GAME_STATE::MAIN_MENU || curGameState == GAME_STATE::LOAD_SCREEN) && loadHappened == false)
		{
			setGameState(GAME_STATE::MAIN_MENU);
			loadHappened = true;
		}



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


		if (keys[KEYS::FULLSCREEN].pressed()) 
		{
			FullScreen = !FullScreen;
			this->WindowResize();
		}


		if (keys[KEYS::HELP_MENU].pressed() || p1Help.Pressed()) {
			ControlScreenToggle = !ControlScreenToggle;
			isPaused = !isPaused;
		}

		if (keys[KEYS::GAME_STATE].pressed())
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
		if (p1Pause.Released())
		{
			if (curGameState == GAME_STATE::WIN_SCREEN) {
				setGameState(GAME_STATE::MAIN_MENU);
			}
			else if (curGameState == GAME_STATE::CREDIT_SCREEN) {
				setGameState(GAME_STATE::MAIN_MENU);
			}
		}

		if (keys[KEYS::DISC_TOG].released())
		{
			PlayerDisconnectToggle = !PlayerDisconnectToggle;
		}

		if (keys[KEYS::PAUSE].pressed()) {
			isPaused = !isPaused;
			ControlScreenToggle = !ControlScreenToggle;

			if (isPaused == false) {
				g_pAudioHolder->ResumeAll();
				g_pMusicStream->ResumeStream();
				timePassed = tempTime;
				mapTime = tempMapTime;
				SprinklersOn = true;
			}
		}


		g_pControllerInput->GetNumConnected(currNumControllers);

		if (isPaused == true) {
			g_pAudioHolder->PauseAll();
			tempTime = timePassed;
			tempMapTime = mapTime;
			SprinklersOn = false;
			timePassed = 0;
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
					else {
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
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::MAIN_MENU)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::MAIN_MENU;
						}
					}
					setGameState(GAME_STATE::MAIN_MENU);
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
					else {
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
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::MAIN_MENU)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::MAIN_MENU + 1;
						}
					}
					setGameState(GAME_STATE::MAIN_MENU);
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
			if (keys[KEYS::AI_DEC_COUNT].released())
			{
				numAI--;
			}
			if (keys[KEYS::AI_INC_COUNT].released())
			{
				numAI++;
			}
			if (menuBomb)
			{
				menuBomb->GetInput();

				menucontroltimer += timePassed;
				if (menuBomb->GetCharacterController()->GetUpDown() > 0.0f && menucontroltimer > 0.2 && menuz > 0)
				{

					menucontroltimer = 0.0;
					if (menuz < 3)
						menuBomb->Move(0.0f, 3.5f, false);
					else if (menuz == 4)
						menuBomb->Move(0.0f, 3.0f, false);
					else
						menuBomb->Move(0.0f, 2.5f, false);

					for (int i = 0; i < MenuSounds.size(); ++i)
					{
						MenuSounds.at(i)->isSoundPlaying(soundplaying);
						if (!soundplaying)
						{
							MenuSounds.at(i)->Play();
							break;
						}
					}
					if (menuz == 1 && menux == 2)
					{
						menuBomb->Move(-1.0f, 0.0f, false);
						menux -= 1;
					}
					menuz -= 1;
					menuIndex = (menuz * 2) + menux;
					if (menuz > 3)
						menuIndex = 7;
					if (menux == 2)
						menuIndex = 8;
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU + menuIndex;
						}
					}
					setGameState(GAME_STATE::ARCADE_MENU);
				}
				if (menuBomb->GetCharacterController()->GetUpDown() < 0.0f && menucontroltimer > 0.2 && menuz < 4)
				{

					menucontroltimer = 0.0;
					if (menuz < 2)
						menuBomb->Move(0.0f, -3.5f, false);
					else if (menuz == 3)
						menuBomb->Move(0.0f, -3.0f, false);
					else
						menuBomb->Move(0.0f, -2.5f, false);

					for (int i = 0; i < MenuSounds.size(); ++i)
					{
						MenuSounds.at(i)->isSoundPlaying(soundplaying);
						if (!soundplaying)
						{
							MenuSounds.at(i)->Play();
							break;
						}
					}
					if (menuz >= 2 && menux > 0)
					{
						menuBomb->Move(-8.0f, 0.0f, false);
						menux -= 1;
					}
					if (menuz == 1 && menux == 2)
					{
						menuBomb->Move(-1.0f, 0.0f, false);
						menux -= 1;
					}
					menuz += 1;
					menuIndex = (menuz * 2) + menux;
					if (menuz > 3)
						menuIndex = 7;
					if (menux == 2)
						menuIndex = 8;
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU + menuIndex;
						}
					}
					setGameState(GAME_STATE::ARCADE_MENU);
				}
				if (menuBomb->GetCharacterController()->GetLeftRight() > 0.0f && menucontroltimer > 0.2 && menux < 2 && menuz < 3)
				{

					menucontroltimer = 0.0;
					if (menux < 1)
						menuBomb->Move(8.0f, 0.0f, false);
					else if (menux == 1)
					{
						if (menuz < 3)
							menuBomb->Move(1.0f, ((float)menuz - 1.0f) * 3.5f, false);
						else if (menuz == 3)
							menuBomb->Move(1.0f, 9.0f, false);
						else if (menuz > 3)
							menuBomb->Move(1.0f, (((float)menuz - 3) * 2.5f) + 9.0f, false);
						menuz = 1;
					}
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
					menuIndex = (menuz * 2) + menux;
					if (menuz > 3)
						menuIndex = 7;
					if (menux == 2)
						menuIndex = 8;
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU + menuIndex;
						}
					}
					setGameState(GAME_STATE::ARCADE_MENU);

				}
				if (menuBomb->GetCharacterController()->GetLeftRight() < 0.0f && menucontroltimer > 0.2 && menux > 0)
				{

					menucontroltimer = 0.0;
					if (menux == 2)
						menuBomb->Move(-1.0f, 0.0f, false);
					else
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
					menuIndex = (menuz * 2) + menux;
					if (menuz > 3)
						menuIndex = 7;
					if (menux == 2)
						menuIndex = 8;
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU + menuIndex;
						}
					}
					setGameState(GAME_STATE::ARCADE_MENU);
				}
				if (menuBomb->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::ACTION))
				{
					menuIndex = (menuz * 2) + menux;
					if (menuz > 3)
						menuIndex = 7;
					if (menux == 2)
						menuIndex = 8;
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
						if (mapsize > 1)
							mapsize--;
						break;
					}
					case 1:
					{
						if (mapsize < 3)
							mapsize++;
						break;
					}
					case 2:
					{
						if (numPLAYERS > 2)
							numPLAYERS--;
						break;
					}
					case 3:
					{
						if (numPLAYERS < 4 && numAI + numPLAYERS < 4)
							numPLAYERS++;
						break;
					}
					case 4:
					{
						if (numAI > 0)
							numAI--;
						break;
					}
					case 5:
					{
						if (numAI < 2 && numPLAYERS + numAI < 4)
							numAI++;
						break;
					}
					case 6:
					{
						for (CObject* menu : menuObjects)
						{
							TComponent* cRenderer;
							TComponent* cTexture;
							if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
								continue;
							menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
							TRendererComponent* renderer = (TRendererComponent*)cRenderer;
							TTextureComponent* Texture = (TTextureComponent*)cTexture;
							if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
							{
								Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU;
							}
						}
						//menuBomb->Move(0.0f, 6.0f, false);
						menuz = 0;
						menux = 0;
						menuIndex = 0;
						setGameState(GAME_STATE::ARCADE_GAME);
						break;
					}
					case 7:
					{
						for (CObject* menu : menuObjects)
						{
							TComponent* cRenderer;
							TComponent* cTexture;
							if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
								continue;
							menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
							TRendererComponent* renderer = (TRendererComponent*)cRenderer;
							TTextureComponent* Texture = (TTextureComponent*)cTexture;
							if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
							{
								Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU;
							}
						}
						//menuBomb->Move(0.0f, 9.0f, false);
						menuz = 0;
						menux = 0;
						menuIndex = 0;
						setGameState(GAME_STATE::MAIN_MENU);
						break;
					}
					case 8:
					{
						for (CObject* menu : menuObjects)
						{
							TComponent* cRenderer;
							TComponent* cTexture;
							if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
								continue;
							menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
							TRendererComponent* renderer = (TRendererComponent*)cRenderer;
							TTextureComponent* Texture = (TTextureComponent*)cTexture;
							if (renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU)
							{
								Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::ARCADE_MENU;
							}
						}
						menuz = 0;
						menux = 0;
						menuIndex = 0;
						setGameState(GAME_STATE::CHARACTER_SCREEN);
						CSx = -11.35f;
						break;
					}
					}
				}
			}
		}
#pragma endregion

#pragma region CHARACTER CUSTOMIZATION SCREEN

		if (curGameState == GAME_STATE::CHARACTER_SCREEN)
		{
			if (menuBomb)
			{
				menuBomb->GetInput();

				menucontroltimer += timePassed;
				if (menuBomb->GetCharacterController()->GetLeftRight() > 0.0f && menucontroltimer > 0.3 && menuIndex < 7)
				{
					menucontroltimer = 0.0;
					if (menuIndex % 2 == 0)
					{
						menuBomb->Move(3.1f, 0.0f, false);
						CSx += 3.1f;
					}
					else
					{
						menuBomb->Move(3.0f, 0.0f, false);
						CSx += 3.0f;
					}
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
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::CHARACTER_SCREEN)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::CHARACTER_SCREEN1 + menuIndex;
						}
					}
					setGameState(GAME_STATE::CHARACTER_SCREEN);

				}
				if (menuBomb->GetCharacterController()->GetLeftRight() < 0.0f && menucontroltimer > 0.3 && menuIndex > 0)
				{
					menucontroltimer = 0.0;
					if (menuIndex % 2 == 0)
					{
						menuBomb->Move(-3.0f, 0.0f, false);
						CSx -= 3.0f;
					}
					else
					{
						menuBomb->Move(-3.1f, 0.0f, false);
						CSx -= 3.1f;
					}
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
					for (CObject* menu : menuObjects)
					{
						TComponent* cRenderer;
						TComponent* cTexture;
						if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
							continue;
						menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
						TRendererComponent* renderer = (TRendererComponent*)cRenderer;
						TTextureComponent* Texture = (TTextureComponent*)cTexture;
						if (renderer->iUsedLoadState == GAME_STATE::CHARACTER_SCREEN)
						{
							Texture->iUsedDiffuseIndex = DIFFUSE_TEXTURES::CHARACTER_SCREEN1 + menuIndex;
						}
					}
					setGameState(GAME_STATE::CHARACTER_SCREEN);
				}
				if (menuBomb->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::PAUSE))
				{
					setGameState(GAME_STATE::ARCADE_MENU);
					CSx = -11.35;
					menuIndex = 0;
				}
				if (menuBomb->GetCharacterController()->ButtonReleased(DEFAULT_BUTTONS::ACTION))
				{
					TComponent* cMesh;
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
						if (playermodel[0] > 3)
						{
							playermodel[0] -= 1;
							PlayersInCustom[0]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[0]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 1:
					{
						if (playermodel[0] < 4)
						{
							playermodel[0] += 1;
							PlayersInCustom[0]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[0]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 2:
					{
						if (playermodel[1] > 3)
						{
							playermodel[1] -= 1;
							PlayersInCustom[1]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[1]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 3:
					{
						if (playermodel[1] < 4)
						{
							playermodel[1] += 1;
							PlayersInCustom[1]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[1]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 4:
					{
						if (numPLAYERS > 2 && playermodel[2] > 3)
						{
							playermodel[2] -= 1;
							PlayersInCustom[2]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[2]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						else if (numAI > 0 && AImodel[0] > 3 && numPLAYERS < 3)
						{
							AImodel[0] -= 1;
							AiInCustom[0]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(AImodel[0]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 5:
					{
						if (numPLAYERS > 2 && playermodel[2] < 4)
						{
							playermodel[2] += 1;
							PlayersInCustom[2]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[2]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						else if (numAI > 0 && AImodel[0] < 4 && numPLAYERS < 3)
						{
							AImodel[0] += 1;
							AiInCustom[0]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(AImodel[0]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 6:
					{
						if (numPLAYERS > 3 && playermodel[3] > 3)
						{
							playermodel[3] -= 1;
							PlayersInCustom[3]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[3]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						else if (numAI > 0 && AImodel[1] > 3 && numPLAYERS > 2)
						{
							AImodel[1] -= 1;
							AiInCustom[1]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(AImodel[1]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						else if (numAI > 1 && AImodel[1] > 3)
						{
							AImodel[1] -= 1;
							AiInCustom[1]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(AImodel[1]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						break;
					}
					case 7:
					{
						if (numPLAYERS > 3 && playermodel[3] < 4)
						{
							playermodel[3] += 1;
							PlayersInCustom[3]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(playermodel[3]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						else if (numAI > 0 && AImodel[1] < 4 && numPLAYERS > 2)
						{
							AImodel[1] += 1;
							AiInCustom[1]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(AImodel[1]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
						else if (numAI > 1 && AImodel[1] < 4)
						{
							AImodel[1] += 1;
							AiInCustom[1]->GetComponent(COMPONENT_TYPE::MESH, cMesh);
							TMeshComponent* mesh = (TMeshComponent*)cMesh;
							mesh = new TMeshComponent(v_tMeshTemplates.at(AImodel[1]));
							setGameState(GAME_STATE::CHARACTER_SCREEN);
						}
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
			//if (!isPaused)
			//{
			//	g_pMusicStream->ResumeStream();
			//	if (tempTime != 0)
			//		timePassed = tempTime;
			//	if (tempMapTime != 0)
			//		mapTime = tempMapTime;
			//}

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
						g_d3dData->viewMat = g_d3dData->camMat;
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

		CustomMeshUpdate();
		g_d3dData->updateCameras();

#pragma region Input
		if (g_pInputRecord->GetState(G_KEY_SPACE, errorCode) == 1) {
			std::cout << "SPACE WAS PRESSED, G INPUT STYLE";
		}

		if (keys[KEYS::SPACE].pressed()) {
			g_pSoundPlayer->Play();
		}

#pragma endregion

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

void CGame::LoadObjectBattle() {
	OBJLoadInfo loadInfo;

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

	//Floor
	for (float z = fMinZ; z <= fMaxZ; z += 2.5f) {
		for (float x = fMinX; x <= fMaxX; x += 2.5f) {
			loadInfo.position = { x, -2.5f, z };
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	//top and bottom walls
	for (float x = fMinX; x <= fMaxX; x += 2.5f) {
		loadInfo.position = { x, 0, fMinZ };
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

		loadInfo.position = { x, 0, fMaxZ };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	//left and right walls
	for (float z = fMinZ; z <= fMaxZ; z += 2.5f) {
		loadInfo.position = { fMinX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	//center box
	loadInfo.position = { 0, 0, 2.5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	//world box
	loadInfo.forwardVec = { 0.0f, 0.0f, 1.0f };
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.position = { 0, 0, 2.5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

void CGame::LoadObjectSmall() {
	OBJLoadInfo loadInfo;

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

	for (float z = fMinZ + 2.5f; z < fMaxZ; z += 2.5f) {
		for (float x = fMinX + 2.5f; x < fMaxX; x += 2.5f) {
			loadInfo.position = { x, -2.5f, z };
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	for (float z = fMinZ + 5; z <= fMaxZ - 5; z += 2.5f) {
		for (float x = fMinX + 5; x <= fMaxX - 5; x += 2.5f) {
			if (rand() % 7 >= 1) {
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

	for (float x = fMinX + 7.5; x <= fMaxX - 7.5; x += 2.5f) {
		loadInfo.position = { x, 0.0f, fMaxZ - 5.0f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMinZ + 5.0f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMaxZ - 2.5f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMinZ + 2.5f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = fMinZ + 7.5; z <= fMaxZ - 7.5; z += 2.5f) {
		loadInfo.position = { fMinX + 2.5f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMinX + 5.0f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX - 5.0f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX - 2.5f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float x = fMinX; x <= fMaxX; x += 2.5f) {
		loadInfo.position = { x, 0, fMinZ };
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

		loadInfo.position = { x, 0, fMaxZ };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = fMinZ; z <= fMaxZ; z += 2.5f) {
		loadInfo.position = { fMinX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	loadInfo.position = { fMinX + 5,  0, fMinZ + 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMinX + 5, 0, fMaxZ - 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMaxX - 5, 0, fMinZ + 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMaxX - 5, 0, fMaxZ - 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5, 0, -5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -10, 0, 0 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -10, 0, 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { -5, 0, 10 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 0, 0, 10 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 0, 0, -5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { 5, 0, -5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
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

	loadInfo.position = { 0, 0, 2.5f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.forwardVec = { 0.0f, 0.0f, 1.0f };
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.position = { 0.0f, 0.0f, 2.5f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

void CGame::LoadObjectMedium() {
	OBJLoadInfo loadInfo;

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

	for (float z = fMinZ + 2.5f; z < fMaxZ; z += 2.5f) {
		for (float x = fMinX + 2.5f; x < fMaxX; x += 2.5f) {
			loadInfo.position = { x, -2.5f, z };
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	for (float z = fMinZ + 7.5; z <= fMaxZ - 7.5; z += 2.5f) {
		for (float x = fMinX + 7.5; x <= fMaxX - 7.5; x += 2.5f) {
			if (rand() % 7 >= 1) {
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

	for (float x = fMinX + 7.5; x <= fMaxX - 7.5; x += 2.5f) {
		loadInfo.position = { x, 0.0f, fMaxZ - 5.0f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMinZ + 5.0f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMaxZ - 2.5f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMinZ + 2.5f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = fMinZ + 7.5; z <= fMaxZ - 7.5; z += 2.5f) {
		loadInfo.position = { fMinX + 2.5f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMinX + 5.0f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX - 5.0f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX - 2.5f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float x = fMinX; x <= fMaxX; x += 2.5f) {
		loadInfo.position = { x, 0, fMinZ };
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

		loadInfo.position = { x, 0, fMaxZ };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = fMinZ; z <= fMaxZ; z += 2.5f) {
		loadInfo.position = { fMinX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	loadInfo.position = { fMinX + 5,  0, fMinZ + 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMinX + 5, 0, fMaxZ - 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMaxX - 5, 0, fMinZ + 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMaxX - 5, 0, fMaxZ - 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0, 0, 2.5f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.forwardVec = { 0.0f, 0.0f, 1.0f };
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.position = { 0.0f, 0.0f, 2.5f };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

void CGame::LoadObjectLarge() {
	OBJLoadInfo loadInfo;

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

	for (float z = fMinZ + 2.5f; z < fMaxZ; z += 2.5f) {
		for (float x = fMinX + 2.5f; x < fMaxX; x += 2.5f) {
			loadInfo.position = { x, -2.5f, z };
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	for (float z = fMinZ + 5; z <= fMaxZ - 5; z += 2.5f) {
		for (float x = fMinX + 5; x <= fMaxX - 5; x += 2.5f) {
			if (rand() % 7 >= 1) {
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

	for (float x = fMinX + 7.5; x <= fMaxX - 7.5; x += 2.5f) {
		loadInfo.position = { x, 0.0f, fMaxZ - 5.0f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMinZ + 5.0f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMaxZ - 2.5f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { x, 0.0f, fMinZ + 2.5f };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = fMinZ + 7.5; z <= fMaxZ - 7.5; z += 2.5f) {
		loadInfo.position = { fMinX + 2.5f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMinX + 5.0f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX - 5.0f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX - 2.5f, 0.0f, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float x = fMinX; x <= fMaxX; x += 2.5f) {
		loadInfo.position = { x, 0, fMinZ };
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

		loadInfo.position = { x, 0, fMaxZ };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	for (float z = fMinZ; z <= fMaxZ; z += 2.5f) {
		loadInfo.position = { fMinX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { fMaxX, 0, z };
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}

	loadInfo.position = { fMinX + 5,  0, fMinZ + 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMinX + 5, 0, fMaxZ - 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMaxX - 5, 0, fMinZ + 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	loadInfo.position = { fMaxX - 5, 0, fMaxZ - 5 };
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0, 0, 2.5f };
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
	AI_Method(timePassed, 0.016f);
	for (CPlayer* currPlayer : v_cPlayers)
	{
		if (!currPlayer || !currPlayer->isAlive())
			continue;

		currPlayer->GetInput();



		currPlayer->updatePlayer(timePassed);

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
			if (isPaused == false) {
				g_pMusicStream->ResumeStream();
				g_pMusicStream->SetVolume(1.0f);
				timePassed = tempTime;
				mapTime = tempMapTime;
				if (passes < 1)
					SprinklersOn = true;
			}
			if (isPaused == true) {
				g_pAudioHolder->PauseAll();
				g_pMusicStream->ResumeStream();
				g_pMusicStream->SetVolume(0.2f);
				tempTime = timePassed;
				tempMapTime = mapTime;
				SprinklersOn = false;
				timePassed = 0;
			}
		}

		// Pause on DC
		if (PlayerDisconnectToggle)
		{

			if (currNumControllers < numPLAYERS || currNumControllers == 0)

			{
				playerdisconnect = true;
				PauseMenuToggle = true;
				isPaused = true;
				for (int i = 0; i < numPLAYERS; i++)
				{
					g_pControllerInput->IsConnected(i, playerdisconnect);
					playerdisconnect = !playerdisconnect;
					if (playerdisconnect)
					{
						PlayerDiscIndex = i;
						break;
					}
				}

			}



			g_pControllerInput->GetNumConnected(prevNumControllers);


			if (currNumControllers == numPLAYERS && playerdisconnect)
			{
				playerdisconnect = false;
				PauseMenuToggle = false;
				isPaused = false;
			}
		}
		// End DC Code

		if (cont->IsControllerConnected())
		{
			deltaX = cont->GetLeftRight();
			deltaZ = cont->GetUpDown();
		}
		else
		{
			if (cont->ButtonHeld(DEFAULT_BUTTONS::LEFT))
				deltaX -= 1.0f;
			if (cont->ButtonHeld(DEFAULT_BUTTONS::RIGHT))
				deltaX += 1.0f;
			if (cont->ButtonHeld(DEFAULT_BUTTONS::UP))
				deltaZ += 1.0f;
			if (cont->ButtonHeld(DEFAULT_BUTTONS::DOWN))
				deltaZ -= 1.0f;
		}

		if (abs(deltaX) <= 0.01f && abs(deltaZ) <= 0.01f)
		{
			if (currPlayer->SetCurrentAnimaion("Idle") == 1)
				currPlayer->ResetAnimation();
		}
		else if (abs(deltaX) > 0.75f || abs(deltaZ) > 0.75f)
		{
			if (currPlayer->SetCurrentAnimaion("Run") == 1)
				currPlayer->ResetAnimation();
		}
		else
		{
			if (currPlayer->SetCurrentAnimaion("Walk") == 1)
				currPlayer->ResetAnimation();
		}

		deltaX *= timePassed * PLAYER_SPEED;
		deltaZ *= timePassed * PLAYER_SPEED;

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
			else
				if (currPlayer->SetCurrentAnimaion("Idle") == 1)
					currPlayer->ResetAnimation();
		}

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
						if (bomb->getTimer() <= 2.0f)
						{
							bomb->CrouchRoll(0, 0, -0.05f, false);
						}
						if (bomb->getTimer() >= 2.0f)
						{
							CPlayer* parent = bomb->getParent();
							for (int j = 0; j < parent->getBombIndices().size(); ++j)
							{
								if (parent->getBombIndices()[j] == bombindex)
								{
									bomb->Cleanup();
									parent->deleteBomb(j);
									parent->DecPlacedBombs();
								}
							}
							bomb->SetAlive(false);

						}
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
					playerfallingSound->Play();
					currPlayer->CrouchRoll(0, 0, -1, false);
					if (offMapTimer >= 1)
					{
						currPlayer->setAlive(false);
						offMapTimer = 0;
					}
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

				case 2:
					bombs = p_cEntityManager->DropBomb0(currPlayer, objects);
					break;
				case 1:
					bombs = p_cEntityManager->DropBomb1(currPlayer, objects);
					break;
				case 4:
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
					if (isPaused == false) {
						g_pMusicStream->ResumeStream();
						g_pMusicStream->SetVolume(1.0f);
						timePassed = tempTime;
						mapTime = tempMapTime;
						SprinklersOn = true;
					}
					break;
				}
				case 1:
				{
					if (!playerdisconnect)
						ControlScreenToggle = !ControlScreenToggle;
					break;
				}
				case 2:
				{
					int previndex = menuIndex;
					menuIndex = 0;
					previndex = menuIndex - previndex;
					pauseMenuBomb->Move(0.0f, (-(float)previndex*1.2f), false);
					isPaused = !isPaused;
					PauseMenuToggle = !PauseMenuToggle;
					mapTime = 0;
					setGameState(GAME_STATE::MAIN_MENU);

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

			freePool[Ecount].prev_pos = freePool[Ecount].pos;
			freePool[Ecount].pos.x += (freePool[Ecount].speed.x * deltaTime);
			freePool[Ecount].pos.y += (freePool[Ecount].speed.y * deltaTime);
			freePool[Ecount].pos.z += (freePool[Ecount].speed.z * deltaTime);
			freePool[Ecount].speed.y -= particleGravity * deltaTime;
			add_line(freePool[Ecount].pos, freePool[Ecount].prev_pos, freePool[Ecount].color);
		}
	}
}

void CGame::setGameState(int _gameState) {
	switch (_gameState) {
	case GAME_STATE::MAIN_MENU:
	{
		g_d3dData->viewMat = g_d3dData->camMat;
		//p1Pause.Reset(false);
		ClearPlayersAndBombs();
		g_pMusicStream->isStreamPlaying(soundplaying);
		if (!soundplaying)
			g_pMusicStream->ResumeStream();

		switch (mapsize) {
		case 1:
			fMinX = -15;
			fMaxX = 15;
			fMinZ = -10;
			fMaxZ = 15;
			break;
		case 2:
			fMinX = -20;
			fMaxX = 20;
			fMinZ = -15;
			fMaxZ = 20;
			break;
		case 3:
			fMinX = -25;
			fMaxX = 25;
			fMinZ = -20;
			fMaxZ = 25;
			break;
		}
		mapTime = 0;

		menuBomb = p_cEntityManager->InstantiatePlayer(1, MODELS::BOMB, DIFFUSE_TEXTURES::BOMB4, DirectX::XMFLOAT3(-1.5f, 11.4f, -6.8f), GAME_STATE::MAIN_MENU, DirectX::XMFLOAT3(0.0f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f));
		SprinklersOn = false;
		break;
	}
	case GAME_STATE::ARCADE_MENU:
	{
		g_d3dData->viewMat = g_d3dData->camMat;
		p1Pause.Reset(false);
		ClearPlayersAndBombs();
		delete menuBomb;
		menuBomb = nullptr;
		menuBomb = p_cEntityManager->InstantiatePlayer(1, MODELS::BOMB, DIFFUSE_TEXTURES::BOMB, DirectX::XMFLOAT3(-8.45f, 15.7f, 2.6f), GAME_STATE::ARCADE_MENU, DirectX::XMFLOAT3(0.0f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f));
		SprinklersOn = false;
		break;
	}
	case GAME_STATE::CHARACTER_SCREEN:
	{
		p1Pause.Reset(false);
		ClearPlayersAndBombs();

		if (menuBomb) {
			delete menuBomb;
			menuBomb = nullptr;
		}

		menuBomb = p_cEntityManager->InstantiatePlayer(1, MODELS::BOMB, DIFFUSE_TEXTURES::BOMB, DirectX::XMFLOAT3(-8.45f, 15.7f, 2.6f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.2f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f));
		PlayersInCustom[0] = p_cEntityManager->InstantiatePlayer(1, playermodel[0], DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(-10.3f, 11.4f, -8.4f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.4f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));
		PlayersInCustom[1] = p_cEntityManager->InstantiatePlayer(2, playermodel[1], DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(-4.0f, 11.4f, -8.4f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.5f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));

		if (numPLAYERS > 2)
			PlayersInCustom[2] = p_cEntityManager->InstantiatePlayer(3, playermodel[2], DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(2.8f, 11.4f, -8.4f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.7f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));
		if (numPLAYERS > 3)
			PlayersInCustom[3] = p_cEntityManager->InstantiatePlayer(4, playermodel[3], DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(9.1f, 11.4f, -8.4f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.8f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));
		if (numAI > 0)
			AiInCustom[1] = p_cEntityManager->InstantiatePlayer(3, AImodel[1], DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(9.1f, 11.4f, -8.4f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.7f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));
		if (numAI > 1)
			AiInCustom[0] = p_cEntityManager->InstantiatePlayer(4, AImodel[0], DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(2.8f, 11.4f, -8.4f), GAME_STATE::CHARACTER_SCREEN, DirectX::XMFLOAT3(0.8f, 1.6f, -1.0f), DirectX::XMFLOAT3(0.03f, 0.03f, 0.03f));

		SprinklersOn = false;
		break;
	}
	case GAME_STATE::ARCADE_GAME:
	{
		SprinklersOn = true;
		passes = 0;
		ClearPlayersAndBombs();
		delete menuBomb;
		shakeTime = 0;
		menuBomb = nullptr;
		mapTime = 0;
		centerAItimer1 = 2.0f;

		switch (mapsize) {
		case 1:
			fMinX = -15;
			fMaxX = 15;
			fMinZ = -10;
			fMaxZ = 15;
			mapPasses = 4;
			LoadObjectSmall();

			break;
		case 2:
			fMinX = -20;
			fMaxX = 20;
			fMinZ = -15;
			fMaxZ = 20;
			mapPasses = 6;
			LoadObjectMedium();
			g_d3dData->viewMat = DirectX::XMMatrixTranslation(0, -15.0f, 8.0f) * g_d3dData->viewMat;
			g_d3dData->tempCamera = g_d3dData->viewMat;
			break;
		case 3:
			fMinX = -25;
			fMaxX = 25;
			fMinZ = -20;
			fMaxZ = 25;
			mapPasses = 8;
			LoadObjectLarge();
			g_d3dData->viewMat = DirectX::XMMatrixTranslation(0, -27.0f, 17.0f) * g_d3dData->viewMat;
			g_d3dData->tempCamera = g_d3dData->viewMat;
			break;
		}

		if (playermodel[0] == MODELS::CHICKEN - 1)
			v_cPlayers[0] = p_cEntityManager->InstantiatePlayer(1, playermodel[0], DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(fMinX + 2.5, 0.0f, fMaxZ - 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
		else
			v_cPlayers[0] = p_cEntityManager->InstantiatePlayer(1, playermodel[0], DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(fMinX + 2.5, 0.0f, fMaxZ - 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
		if (playermodel[1] == MODELS::CHICKEN - 1)
			v_cPlayers[1] = p_cEntityManager->InstantiatePlayer(2, playermodel[1], DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(fMaxX - 2.5, 0.0f, fMinZ + 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
		else
			v_cPlayers[1] = p_cEntityManager->InstantiatePlayer(2, playermodel[1], DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(fMaxX - 2.5, 0.0f, fMinZ + 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));

		if (numPLAYERS > 2) {
			if (playermodel[2] == MODELS::CHICKEN - 1)
				v_cPlayers[2] = p_cEntityManager->InstantiatePlayer(3, playermodel[2], DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(fMaxX - 2.5, 0.0f, fMaxZ - 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
			else
				v_cPlayers[2] = p_cEntityManager->InstantiatePlayer(3, playermodel[2], DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(fMaxX - 2.5, 0.0f, fMaxZ - 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
		}

		if (numPLAYERS > 3) {
			if (playermodel[3] == MODELS::CHICKEN - 1)
				v_cPlayers[3] = p_cEntityManager->InstantiatePlayer(4, playermodel[3], DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(fMinX + 2.5, 0.0f, fMinZ + 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.6f, 0.8f, 0.6f));
			else
				v_cPlayers[3] = p_cEntityManager->InstantiatePlayer(4, playermodel[3], DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(fMinX + 2.5, 0.0f, fMinZ + 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.6f, 0.8f, 0.6f));
		}

		if (numAI > 0) {
			if (AImodel[1] == MODELS::CHICKEN - 1)
				v_cAI[1] = p_cEntityManager->InstantiatePlayer(4, AImodel[1], DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(fMinX + 2.5, 0.0f, fMinZ + 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
			else
				v_cAI[1] = p_cEntityManager->InstantiatePlayer(4, AImodel[1], DIFFUSE_TEXTURES::CHICKEN4, DirectX::XMFLOAT3(fMinX + 2.5, 0.0f, fMinZ + 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
		}

		if (numAI > 1) {
			if (AImodel[0] == MODELS::CHICKEN - 1)
				v_cAI[0] = p_cEntityManager->InstantiatePlayer(3, AImodel[0], DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(fMaxX - 2.5, 0.0f, fMaxZ - 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
			else
				v_cAI[0] = p_cEntityManager->InstantiatePlayer(3, AImodel[0], DIFFUSE_TEXTURES::CHICKEN3, DirectX::XMFLOAT3(fMaxX - 2.5, 0.0f, fMaxZ - 2.5), GAME_STATE::ARCADE_GAME, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3(0.04f, 0.04f, 0.03f));
		}

		//mapTime = 0;
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
		g_d3dData->viewMat = g_d3dData->camMat;
		switch (mapsize) {
		case 1:
			fMinX = -15;
			fMaxX = 15;
			fMinZ = -10;
			fMaxZ = 15;
			break;
		case 2:
			fMinX = -20;
			fMaxX = 20;
			fMinZ = -15;
			fMaxZ = 20;
			break;
		case 3:
			fMinX = -25;
			fMaxX = 25;
			fMinZ = -20;
			fMaxZ = 25;
			break;
		}
		mapTime = 0;
		ClearPlayersAndBombs();
		break;
	}
	}

	curGameState = _gameState;
}

void CGame::ClearPlayersAndBombs() {
	for (int i = 0; i < v_cPlayers.size(); ++i)
		if (v_cPlayers[i]) {
			v_cPlayers[i]->Cleanup();
			delete v_cPlayers[i];
			v_cPlayers[i] = nullptr;
		}

	for (CBomb* bomb : v_cBombs) {
		if (bomb)
			delete bomb;
		bomb = nullptr;
	}

	v_cBombs.clear();
	for (int i = 0; i < v_cAI.size(); ++i)
		if (v_cAI[i]) {
			v_cAI[i]->Cleanup();
			delete v_cAI[i];
			v_cAI[i] = nullptr;
		}

	for (int i = 0; i < AiInCustom.size(); ++i)
		if (AiInCustom[i]) {
			AiInCustom[i]->Cleanup();
			delete AiInCustom[i];
			AiInCustom[i] = nullptr;
		}

	for (int i = 0; i < PlayersInCustom.size(); ++i)
		if (PlayersInCustom[i]) {
			PlayersInCustom[i]->Cleanup();
			delete PlayersInCustom[i];
			PlayersInCustom[i] = nullptr;
		}

	for (CObject* object : objects) {
		if (object)
			delete object;
		object = nullptr;
	}

	for (CObject* exp : Xexplosions) {
		if (exp)
			delete exp;
		exp = nullptr;
	}

	Xexplosions.clear();
	for (CObject* exp : Zexplosions) {
		if (exp)
			delete exp;
		exp = nullptr;
	}

	for (CItem* item : items) {
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

void CGame::updateBombs(double timePassed) {
	for (int i = 0; i < explosionTimers.size(); ++i) {
		if (explosionTimers[i] >= 0.3f) {
			delete Xexplosions[i];
			Xexplosions.erase(Xexplosions.begin() + i);
			delete Zexplosions[i];
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

				if (Xexplosions[i]->Collides((CObject*)player) || Zexplosions[i]->Collides((CObject*)player)) {
					if (player->GetCrouchStatus() == false || XexplosionTrans->fPosition.y == 0 || ZexplosionTrans->fPosition.y == 0) {
						player->setAlive(false);

						DeathSound->Play();
					}
				}
			}
		}

		for (CPlayer* AI : v_cAI) {
			if (AI) {
				if (Xexplosions[i]->Collides((CObject*)AI) || Zexplosions[i]->Collides((CObject*)AI)) {
					if (AI->GetCrouchStatus() == false || XexplosionTrans->fPosition.y == 0 || ZexplosionTrans->fPosition.y == 0) {
						AI->setAlive(false);
						DeathSound->Play();
					}
				}
			}
		}

		for (int k = 0; k < v_cBombs.size(); k++) {
			if (v_cBombs[k] && v_cBombs[k]->isAlive()) {
				if (Xexplosions[i]->Collides((CObject*)v_cBombs[k]) || Zexplosions[i]->Collides((CObject*)v_cBombs[k])) {
					v_cBombs[k]->SetToExplode();
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
				bombExploded = true;

				TComponent* cRenderer = nullptr;
				TTransformComponent* Brenderer = nullptr;

				if (v_cBombs[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer)) {
					Brenderer = (TTransformComponent*)cRenderer;
					bombPos = Brenderer->fPosition;
				}

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

bool CGame::loadTempMenus() {
	OBJLoadInfo loadInfo;
	float aspectratio = 0.0f;
	aspectratio = g_d3dData->windowWidthHeight.x / g_d3dData->windowWidthHeight.y;
	float fullscreen = 1920.0f / 1080.0f;
	aspectratio = fullscreen / aspectratio;

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
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::LOADING_SCREEN;
	loadInfo.scale = DirectX::XMFLOAT3(2.515f, 1.95f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::LOAD_SCREEN;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::GP_LOGO;
	loadInfo.scale = DirectX::XMFLOAT3(2.515f, 1.95f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::GP_SPLASH;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::FS_LOGO;
	loadInfo.scale = DirectX::XMFLOAT3(2.515f, 1.95f, 1.0f) ;
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::FS_SPLASH;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::ARCADE_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(2.515f, 2.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::ARCADE_MENU;
	menuObjects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 11.4f, -4.2f };
	loadInfo.forwardVec = { 0.0f, 1.59f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CHARACTER_SCREEN1;
	loadInfo.scale = DirectX::XMFLOAT3(2.515f, 2.0f, 1.0f);
	loadInfo.meshID = MODELS::MENU1;
	loadInfo.LoadState = GAME_STATE::CHARACTER_SCREEN;
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

void CGame::PlayerCollision(CPlayer * playerToCheck, CObject* cObj, float dx, float dz) {
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

	if (zD < mZ && zD > mZ - 0.2) {
		pTrans->fPosition.z += ((mZ - zD) + 0.1f) *(float)upDown;
		pCollider->d3dCollider.Center.z += ((mZ - zD) + 0.1f) *(float)upDown;
	}
	else if (xD < mX && xD > mX - 0.2) {
		pTrans->fPosition.x += ((mX - xD)) *(float)leftRight;
		pCollider->d3dCollider.Center.x += ((mX - xD)) *(float)leftRight;
	}

	pTrans->mObjMatrix.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
	pTrans->mObjMatrix *= DirectX::XMMatrixTranslation(pTrans->fPosition.x, pTrans->fPosition.y, pTrans->fPosition.z);
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

void CGame::AI_Method(double timepassed, double action_time)
{
	AItime += timepassed;
	AIbombaction += timepassed;
	centerAItimer1 += timepassed;
	if (action_time < AItime)
	{
		AItime = 0.0;
		int width;
		int height;

		width = (((fMaxX)-(fMinX)) / 2.5f) + 1;
		height = (((fMaxZ)-(fMinZ)) / 2.5f);

		int gridsize = width * height;

		std::vector<int> GRID;
		GRID.resize(gridsize);

		for (int i = 0; i < GRID.size(); ++i)
		{
			int z = i / width;
			int x = i % width;
			float zpos;
			float xpos;

			zpos = fMaxZ - (float(z) * 2.5f);
			xpos = fMaxX - (float(x) * 2.5f);

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
			float cZ;
			float cX;
			float startx = 0;
			float startz = 0;
			float endx = 0;
			float endz = 0;

			TComponent* AIComponent;
			currAI->GetComponent(COMPONENT_TYPE::TRANSFORM, AIComponent);
			TTransformComponent* AITransform = (TTransformComponent*)AIComponent;


			//float dX = abs(AITransform->fPosition.x - xpos);
			//float dZ = abs(AITransform->fPosition.z - zpos);
			int x = 0;
			int z = 0;
			float dec = 0;
			int gridlocation = 237;
			for (int i = 0; i < GRID.size(); ++i)
			{
				z = (i / width);
				x = i % width;
				float zpos;
				float xpos;

				zpos = fMaxZ - (float(z) * 2.5f);
				xpos = fMaxX - (float(x) * 2.5f);

				TComponent* pComponent;

				float dX = abs(AITransform->fPosition.x - xpos);
				float dZ = abs(AITransform->fPosition.z - zpos);
				if (!AI_1_STARTMoving)
				{
					startx = AITransform->fPosition.x;
					startz = AITransform->fPosition.z;
					endx = AITransform->fPosition.x;
					endz = AITransform->fPosition.z + 5.0f;
					AI_1_STARTMoving = true;
				}

				cZ = AITransform->fPosition.z - zpos;
				cX = AITransform->fPosition.x - xpos;

				if (dX <= 1.25f && dZ <= 1.25f)
				{

					if (centerAItimer1 >= 3.0f)
					{
						currAI->Move(cX, cZ, true);
						centerAItimer1 = 0.0f;
					}
					gridlocation = i;
					break;
				}
			}
			//dec = abs(AITransform->fPosition.x) / 2.5f;
			//x = dec;
			//dec = dec - x;
			//dec = abs(fMaxX - AITransform->fPosition.x) / 2.5f;
			//
			////dec += 1.0f;
			//x = dec;
			//x += 1;
			////if (dec > 0.0f)
			////	x = abs(fMaxX - AITransform->fPosition.x + 2.5f) / 2.5f;
			//if (x > width - 1)
			//	x = width - 1;
			//
			////dec = abs(AITransform->fPosition.z) / 2.5f;
			////z = dec;
			////dec = dec - z;
			//dec = abs(fMaxZ - AITransform->fPosition.z) / 2.5f;
			////dec += 1.0f;
			//z = dec;
			////if (dec > 0.0f)
			////	z = abs(fMaxZ - AITransform->fPosition.z + 2.5f) / 2.5f;
			//if (z > height - 1)
			//	z = height - 1;

			for (CObject* cObj : objects) {
				if (currAI->Collides(cObj))
					PlayerCollision(currAI, cObj, deltaX, deltaZ);
			}



			bool breakout = false;
			//gridlocation = (z * width) + x;
			//if (AI_1_STARTMoving && !AI_1_Moving)
			//{
			//	bool idk = currAI->MoveOverTime(startx, endx, startz, endz, timepassed);
			//	if (idk)
			//		AI_1_Moving = true;
			//}
			//else if (AI_1_Moving)
			//{
			//	//AI_1_STARTMoving = false;
			//	AI_1_Moving = false;
			//}
			//else
			//{
				if (gridlocation < GRID.size())
				{

					if (GRID.at(gridlocation) == GRID_SYSTEM::BOMB)
					{
						int tile = GRID[gridlocation];
						//currAI->Move(((x / ((width - 1) / 2)) - 1) * timepassed * AI_SPEED, ((z / ((height - 1) / 2)) - 1) * timepassed * AI_SPEED);
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
									continue;
								}

								if (xchange < 0 || xchange > width - 1)
								{
									xbounds = false;
									xchange = x;
									continue;
								}
								gridlocation = (zchange * width) + xchange;
								tile = GRID[gridlocation];
								zchange -= z;
								xchange -= x;
								if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
								{

									deltaX = timepassed * AI_SPEED * -xchange;
									deltaZ = timepassed * AI_SPEED * -zchange;
									if (abs(deltaX) + abs(deltaZ) <= 0.2f)
									{
										if (currAI->SetCurrentAnimaion("Idle") == 1)
											currAI->ResetAnimation();
									}
									else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
									{
										if (currAI->SetCurrentAnimaion("Run") == 1)
											currAI->ResetAnimation();
									}
									else
									{
										if (currAI->SetCurrentAnimaion("Walk") == 1)
											currAI->ResetAnimation();
									}
									currAI->Move(deltaX, deltaZ);
									//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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
						//currAI->Move(((x / ((width - 1) / 2)) - 1) * timepassed * AI_SPEED, ((z / ((height - 1) / 2)) - 1) * timepassed * AI_SPEED);
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
									continue;
								}

								if (xchange < 0 || xchange > width - 1)
								{
									xbounds = false;
									xchange = x;
									continue;
								}
								gridlocation = (zchange * width) + xchange;
								tile = GRID[gridlocation];
								zchange -= z;
								xchange -= x;
								if (tile == GRID_SYSTEM::FREE /*&& (zchange == 0 xor xchange == 0)*/ && xbounds && zbounds)
								{

									deltaX = timepassed * AI_SPEED * -xchange;
									deltaZ = timepassed * AI_SPEED * -zchange;
									if (abs(deltaX) + abs(deltaZ) <= 0.2f)
									{
										if (currAI->SetCurrentAnimaion("Idle") == 1)
											currAI->ResetAnimation();
									}
									else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
									{
										if (currAI->SetCurrentAnimaion("Run") == 1)
											currAI->ResetAnimation();
									}
									else
									{
										if (currAI->SetCurrentAnimaion("Walk") == 1)
											currAI->ResetAnimation();
									}
									currAI->Move(deltaX, deltaZ);
									//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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
									continue;
								}

								if (xchange < 0 || xchange > width - 1)
								{
									xbounds = false;
									xchange = x;
									continue;
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
										if (tile == GRID_SYSTEM::BOMB && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
										{
											deltaX = timepassed * AI_SPEED * xchange;
											deltaZ = timepassed * AI_SPEED * zchange;
											if (abs(deltaX) + abs(deltaZ) <= 0.2f)
											{
												if (currAI->SetCurrentAnimaion("Idle") == 1)
													currAI->ResetAnimation();
											}
											else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
											{
												if (currAI->SetCurrentAnimaion("Run") == 1)
													currAI->ResetAnimation();
											}
											else
											{
												if (currAI->SetCurrentAnimaion("Walk") == 1)
													currAI->ResetAnimation();
											}
											currAI->Move(deltaX, deltaZ);
											//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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
														continue;
													}

													if (xchange < 0 || xchange > width - 1)
													{
														xbounds = false;
														xchange = x;
														continue;
													}


													gridlocation = ((zchange)* width) + (xchange);
													zchange -= z;
													xchange -= x;


													if (gridlocation < GRID.size())
													{
														int tile = GRID[gridlocation];
														if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
														{


															deltaX = timepassed * AI_SPEED * -xchange;
															deltaZ = timepassed * AI_SPEED * -zchange;
															if (abs(deltaX) + abs(deltaZ) <= 0.2f)
															{
																if (currAI->SetCurrentAnimaion("Idle") == 1)
																	currAI->ResetAnimation();
															}
															else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
															{
																if (currAI->SetCurrentAnimaion("Run") == 1)
																	currAI->ResetAnimation();
															}
															else
															{
																if (currAI->SetCurrentAnimaion("Walk") == 1)
																	currAI->ResetAnimation();
															}
															currAI->Move(deltaX, deltaZ);
															//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
															for (CObject* cObj : objects) {
																if (currAI->Collides(cObj))
																	PlayerCollision(currAI, cObj, deltaX, deltaZ);
															}
														}
													}
												}
											}
										}
										if (tile == GRID_SYSTEM::EXPLOSION_RADIUS && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
										{
											deltaX = timepassed * AI_SPEED * xchange;
											deltaZ = timepassed * AI_SPEED * zchange;
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
														continue;
													}

													if (xchange < 0 || xchange > width - 1)
													{
														xbounds = false;
														xchange = x;
														continue;
													}
													gridlocation = (zchange * width) + xchange;
													tile = GRID[gridlocation];
													zchange -= z;
													xchange -= x;
													if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
													{

														deltaX = timepassed * AI_SPEED * -xchange;
														deltaZ = timepassed * AI_SPEED * -zchange;
														if (abs(deltaX) + abs(deltaZ) <= 0.2f)
														{
															if (currAI->SetCurrentAnimaion("Idle") == 1)
																currAI->ResetAnimation();
														}
														else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
														{
															if (currAI->SetCurrentAnimaion("Run") == 1)
																currAI->ResetAnimation();
														}
														else
														{
															if (currAI->SetCurrentAnimaion("Walk") == 1)
																currAI->ResetAnimation();
														}
														currAI->Move(deltaX, deltaZ);
														//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
														for (CObject* cObj : objects) {
															if (currAI->Collides(cObj))
																PlayerCollision(currAI, cObj, deltaX, deltaZ);
														}
													}
												}
											}
										}
									}
									else if (gridcheck == 2)
									{
										if (tile == GRID_SYSTEM::POWERUP && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
										{
											deltaX = timepassed * AI_SPEED * -xchange;
											deltaZ = timepassed * AI_SPEED * -zchange;
											currAI->Move(deltaX, deltaZ);
											//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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
														continue;
													}

													if (xchange < 0 || xchange > width - 1)
													{
														xbounds = false;
														xchange = x;
														continue;
													}

													gridlocation = ((zchange)* width) + (xchange);
													zchange -= z;
													xchange -= x;

													if (gridlocation < GRID.size())
													{
														int tile = GRID[gridlocation];
														if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
														{

															deltaX = timepassed * AI_SPEED * xchange;
															deltaZ = timepassed * AI_SPEED * zchange;
															if (abs(deltaX) + abs(deltaZ) <= 0.2f)
															{
																if (currAI->SetCurrentAnimaion("Idle") == 1)
																	currAI->ResetAnimation();
															}
															else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
															{
																if (currAI->SetCurrentAnimaion("Run") == 1)
																	currAI->ResetAnimation();
															}
															else
															{
																if (currAI->SetCurrentAnimaion("Walk") == 1)
																	currAI->ResetAnimation();
															}
															currAI->Move(deltaX, deltaZ);
															//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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

									else if (gridcheck == 3 && AIbombaction >= 3.0f)
									{
										if (tile == GRID_SYSTEM::DESTROYABLE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
										{
											deltaX = timepassed * AI_SPEED * -xchange;
											deltaZ = timepassed * AI_SPEED * -zchange;
											currAI->Move(deltaX, deltaZ);
											//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (xchange*5.0f), AITransform->fPosition.z, AITransform->fPosition.z + (zchange*5.0f));
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


														case 2:
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
														case 4:
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
											dZ = 2;
											break;
										}
									}
									else if (gridcheck == 4)
									{
										if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
										{

											deltaX = timepassed * AI_SPEED * -xchange;
											deltaZ = timepassed * AI_SPEED * -zchange;
											if (abs(deltaX) + abs(deltaZ) <= 0.2f)
											{
												if (currAI->SetCurrentAnimaion("Idle") == 1)
													currAI->ResetAnimation();
											}
											else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
											{
												if (currAI->SetCurrentAnimaion("Run") == 1)
													currAI->ResetAnimation();
											}
											else
											{
												if (currAI->SetCurrentAnimaion("Walk") == 1)
													currAI->ResetAnimation();
											}
											currAI->Move(deltaX, deltaZ);
											//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
											for (CObject* cObj : objects) {
												if (currAI->Collides(cObj))
													PlayerCollision(currAI, cObj, deltaX, deltaZ);
											}
											dZ = 2;
											break;
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

						//currAI->Move(((x / ((width - 1) / 2)) - 1) * timepassed * AI_SPEED, ((z / ((height - 1) / 2)) - 1) * timepassed * AI_SPEED);
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
									continue;
								}

								if (xchange < 0 || xchange > width - 1)
								{
									xbounds = false;
									xchange = x;
									continue;
								}
								if (tile == GRID_SYSTEM::FREE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
								{
									deltaX = timepassed * AI_SPEED * -dX;
									deltaZ = timepassed * AI_SPEED * -dZ;
									if (abs(deltaX) + abs(deltaZ) <= 0.2f)
									{
										if (currAI->SetCurrentAnimaion("Idle") == 1)
											currAI->ResetAnimation();
									}
									else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
									{
										if (currAI->SetCurrentAnimaion("Run") == 1)
											currAI->ResetAnimation();
									}
									else
									{
										if (currAI->SetCurrentAnimaion("Walk") == 1)
											currAI->ResetAnimation();
									}
									currAI->Move(deltaX, deltaZ);
									//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
									for (CObject* cObj : objects) {
										if (currAI->Collides(cObj))
											PlayerCollision(currAI, cObj, deltaX, deltaZ);
									}
									dZ = 2;
									break;
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
										continue;
									}


									if (xchange < 0 && xchange > width - 1)
									{
										xbounds = false;
										xchange = x;
										continue;
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
												deltaX = timepassed * AI_SPEED * xchange;
												deltaZ = timepassed * AI_SPEED * zchange;
												if (abs(deltaX) + abs(deltaZ) <= 0.2f)
												{
													if (currAI->SetCurrentAnimaion("Idle") == 1)
														currAI->ResetAnimation();
												}
												else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
												{
													if (currAI->SetCurrentAnimaion("Run") == 1)
														currAI->ResetAnimation();
												}
												else
												{
													if (currAI->SetCurrentAnimaion("Walk") == 1)
														currAI->ResetAnimation();
												}
												currAI->Move(deltaX, deltaZ);
												//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
												for (CObject* cObj : objects) {
													if (currAI->Collides(cObj))
														PlayerCollision(currAI, cObj, deltaX, deltaZ);
												}


												for (int dZ = -1; dZ <= 1; ++dZ)
												{
													for (int dX = -1; dX <= 2; ++dX)
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
															continue;
														}


														if (xchange < 0 && xchange > width - 1)
														{
															xbounds = false;
															xchange = x;
															continue;
														}

														gridlocation = ((zchange)* width) + (xchange);
														zchange -= z;
														xchange -= x;

														if (gridlocation < GRID.size())
														{
															int tile = GRID[gridlocation];
															if (tile == GRID_SYSTEM::FREE  && xbounds && zbounds)
															{

																deltaX = timepassed * AI_SPEED * -xchange;
																deltaZ = timepassed * AI_SPEED * -zchange;
																if (abs(deltaX) + abs(deltaZ) <= 0.2f)
																{
																	if (currAI->SetCurrentAnimaion("Idle") == 1)
																		currAI->ResetAnimation();
																}
																else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
																{
																	if (currAI->SetCurrentAnimaion("Run") == 1)
																		currAI->ResetAnimation();
																}
																else
																{
																	if (currAI->SetCurrentAnimaion("Walk") == 1)
																		currAI->ResetAnimation();
																}
																currAI->Move(deltaX, deltaZ);
																//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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

										else if (gridcheck == 1 && AIbombaction >= 3.0f)
										{
											if (tile == GRID_SYSTEM::DESTROYABLE && (zchange == 0 xor xchange == 0) && xbounds && zbounds)
											{
												deltaX = timepassed * AI_SPEED * -xchange;
												deltaZ = timepassed * AI_SPEED * -zchange;
												currAI->Move(deltaX, deltaZ);
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

															case 2:
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
															case 4:
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

												deltaX = timepassed * AI_SPEED * -xchange;
												deltaZ = timepassed * AI_SPEED * -zchange;
												if (abs(deltaX) + abs(deltaZ) <= 0.2f)
												{
													if (currAI->SetCurrentAnimaion("Idle") == 1)
														currAI->ResetAnimation();
												}
												else if (abs(deltaX) + abs(deltaZ) >= 1.5f)
												{
													if (currAI->SetCurrentAnimaion("Run") == 1)
														currAI->ResetAnimation();
												}
												else
												{
													if (currAI->SetCurrentAnimaion("Walk") == 1)
														currAI->ResetAnimation();
												}
												currAI->Move(deltaX, deltaZ);
												//currAI->MoveOverTime(AITransform->fPosition.x, AITransform->fPosition.x + (deltaX * 4.0f), AITransform->fPosition.z, AITransform->fPosition.z + (deltaZ * 4.0f), timepassed);
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
			//}

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
					offMapTimer += timepassed;

					if (offMapTimer >= 0.25)
					{
						offMapTimer = 0;
						currAI->setAlive(false);
					}
				}
			}

		}
	}
}

void CGame::CustomMeshUpdate() {

	//RenderMenus
	for (CObject* menu : menuObjects) {
		TComponent* cRenderer;
		if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			continue;

		TRendererComponent* renderer = (TRendererComponent*)cRenderer;
		if (renderer->iUsedLoadState == curGameState || (PauseMenuToggle && renderer->iUsedLoadState == GAME_STATE::PAUSE_MENU))
			p_cRendererManager->RenderObject(menu);

		if (renderer->iUsedLoadState == GAME_STATE::CONTROLS_SCREEN && ControlScreenToggle) {
			p_cRendererManager->RenderObject(menu);
		}
	}

	//render particles
	if (bombExploded) {
		InitSortedParticles(sortedParticles, timer.Delta(), bombPos, { 1,1,0,1 });
	}

	if (SprinklersOn == true) {
		InitFreeParticles(firstEmit, shared_pool, timePassed);
		InitFreeParticles(secondEmit, shared_pool, timePassed);
		InitFreeParticles(thirdEmit, shared_pool, timePassed);
		InitFreeParticles(fourthEmit, shared_pool, timePassed);
	}

	//RenderObjects
	if (mapTime >= 25 && passes < mapPasses && !isPaused) {


		warnSound->isSoundPlaying(warningSoundPlaying);
		playerfallingSound->isSoundPlaying(playerfallingSoundPlaying);
		fallingSound->isSoundPlaying(fallingSoundPlaying);
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
					if (!warningSoundPlaying)
					{
						warnSound->Play();
					}
					if (mapTime >= 28)
					{
						if (!fallingSoundPlaying) {
							fallingSound->Play();
						}

						g_pControllerInput->StartVibration(0, 0.125f, 1, 0);
						g_pControllerInput->StartVibration(0, 0.125f, 1, 1);
						g_pControllerInput->StartVibration(0, 0.125f, 1, 2);
						g_pControllerInput->StartVibration(0, 0.125f, 1, 3);



						objects[i]->CrouchRoll(0, 0, -0.75f, false);
						SprinklersOn = false;

						for (CPlayer* player : v_cPlayers) {
							if (!player || !player->isAlive())
								continue;

							TComponent* _prenderer = nullptr;
							if (player->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
								TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;

								if (pRenderer->fPosition.x == renderer->fPosition.x && pRenderer->fPosition.z == renderer->fPosition.z) {
									//player->setAlive(false);
									player->CrouchRoll(0, 0, -1, false);
									if (!playerfallingSoundPlaying)
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
									AI->CrouchRoll(0, 0, -0.5f, false);
							}
						}

						for (int i = 0; i < items.size(); i++) {
							TComponent* _iRenderer = nullptr;
							if (items[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, _iRenderer)) {
								TTransformComponent* iRenderer = (TTransformComponent*)_iRenderer;

								if (iRenderer->fPosition.x == renderer->fPosition.x && iRenderer->fPosition.z == renderer->fPosition.z)
									items[i]->CrouchRoll(0, 0, -0.5f, false);
							}
						}


					}

					if (mapTime >= 29 && passes < mapPasses) {
						passes += 1;

						for (int passes = 0; passes < 7; passes++) {
							for (int i = 0; i < objects.size(); ++i) {
								TComponent* cRenderer = nullptr;
								TTransformComponent* renderer = nullptr;
								if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer)) {
									renderer = (TTransformComponent*)cRenderer;

									if (renderer->fPosition.x == fMinX || renderer->fPosition.z == fMinZ || renderer->fPosition.x == fMaxX || renderer->fPosition.z == fMaxZ) {
										//SprinklersOn = false;

										for (CPlayer* player : v_cPlayers) {
											if (!player || !player->isAlive())
												continue;

											TComponent* _prenderer = nullptr;
											if (player->GetComponent(COMPONENT_TYPE::TRANSFORM, _prenderer)) {
												TTransformComponent* pRenderer = (TTransformComponent*)_prenderer;


												if (pRenderer->fPosition.x == renderer->fPosition.x && pRenderer->fPosition.z == renderer->fPosition.z) {
													player->setAlive(false);
													//playerfallingSound->Play();
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

										for (int i = 0; i < items.size(); i++) {
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

						fMinX += 2.5;
						fMinZ += 2.5;
						fMaxX -= 2.5;
						fMaxZ -= 2.5;
						mapTime = 0;
					}
				}
			}
		}
	}



	if (objects.size() > 0) {
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
	if (bombExploded && curGameState == GAME_STATE::ARCADE_GAME) {
		shakeTime += timePassed;

		viewPos = g_d3dData->screenShake();
		if (shakeTime >= 0.5 || isPaused) {
			bombExploded = false;

			if (!bombExploded) {
				if (mapsize > 1)
					g_d3dData->resetCamera();
				else
					g_d3dData->viewMat = g_d3dData->camMat;
				shakeTime = 0;
			}
		}
	}
	else if (bombExploded) {
		shakeTime = 0.6f;

		if (shakeTime >= 0.5 || isPaused) {
			bombExploded = false;
			if (!bombExploded) {

				shakeTime = 0;
			}
		}
	}

	//Render Players
	for (CPlayer* player : v_cPlayers) {
		if (!player || !player->GetRenderable())
			continue;

		TComponent* renderer = nullptr;
		if (player->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
			TRendererComponent* pRenderer = (TRendererComponent*)renderer;

			if (pRenderer->iUsedLoadState == curGameState)
				p_cRendererManager->RenderObject((CObject*)player);
		}
	}

	//Character Screen Player Models
	for (CPlayer* player : PlayersInCustom) {
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
	for (CPlayer* AI : v_cAI) {
		if (!AI || !AI->isAlive())
			continue;

		TComponent* renderer = nullptr;
		if (AI->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
			TRendererComponent* pRenderer = (TRendererComponent*)renderer;

			if (pRenderer->iUsedLoadState == curGameState)
				p_cRendererManager->RenderObject((CObject*)AI);
		}
	}

	//Character Screen AI Models
	for (CPlayer* AI : AiInCustom) {
		if (!AI || !AI->isAlive())
			continue;

		TComponent* renderer = nullptr;
		if (AI->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
			TRendererComponent* pRenderer = (TRendererComponent*)renderer;

			if (pRenderer->iUsedLoadState == curGameState)
				p_cRendererManager->RenderObject((CObject*)AI);
		}
	}

	//Render Emitter
	if (objects.size() > 0) {
		TComponent* cRenderer = nullptr;
		TRendererComponent* renderer = nullptr;
		if (objects[objects.size() - 1]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer)) {
			renderer = (TRendererComponent*)cRenderer;

			if (renderer->iUsedLoadState == curGameState)
				p_cRendererManager->RenderObject(objects[objects.size() - 1]);
		}
	}

	if (menuBomb) {
		TComponent* renderer = nullptr;
		if (menuBomb->GetComponent(COMPONENT_TYPE::RENDERER, renderer)) {
			TRendererComponent* pRenderer = (TRendererComponent*)renderer;

			if (pRenderer->iUsedLoadState == curGameState)
				p_cRendererManager->RenderObject((CObject*)menuBomb);
		}
	}
}

void CGame::WallDrop(CObject* objectToCheck) {
	TComponent* cRenderer = nullptr;
	TTransformComponent* renderer = nullptr;
	objectToCheck->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer);
	renderer = (TTransformComponent*)cRenderer;
	renderer->fPosition.y -= 1.5f;
}