#include "CGame.h"
#include <DirectXCollision.h>
#include <iostream>
#include "pools.h"

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
	enum { UP = 0, DOWN, LEFT, RIGHT, ZERO, RMB, SPACE, HELP_MENU, GAME_STATE, FULLSCREEN, COUNT };
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
	'F'
};

bool ControlScreenToggle = false;
bool Controller1Alive = false;
bool Controller2Alive = false;

int boxDropped;
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
		static double timePassed = 0.0;

		prevFrame = currFrame;
		currFrame = GetTickCount64();
		timePassed = (currFrame - prevFrame) / 1000.0;


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

		if (keys[KEYS::HELP_MENU].pressed())
			ControlScreenToggle = !ControlScreenToggle;

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
		if (curGameState == GAME_STATE::ARCADE_GAME)
		{

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
		for (int i = 0; i < objects.size(); ++i)
		{
			if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
			{
				TComponent* cRenderer = nullptr;
				TRendererComponent* renderer = nullptr;
				if (objects[i]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
				{
					renderer = (TRendererComponent*)cRenderer;
					if (renderer->iUsedLoadState == curGameState)
						p_cRendererManager->RenderObject(objects[i]);
				}

				if (ControlScreenToggle == true)
				{
					if (objects[i]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
					{
						renderer = (TRendererComponent*)cRenderer;
						if (renderer->iUsedLoadState == GAME_STATE::CONTROLS_SCREEN)
							p_cRendererManager->RenderObject(objects[i]);
					}
				}
			}
		}

		//RenderBombs
		for (CBomb* bomb : v_cBombs)
		{
			if (!bomb || !bomb->isAlive())
				continue;
			TComponent* renderer = nullptr;
			if (bomb->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
			{
				TRendererComponent* tRenderer = (TRendererComponent*)renderer;
				if (tRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject((CObject*)bomb);
			}
		}

		//RenderExplosions
		for (int i = 0; i < explosionTimers.size(); ++i)
		{
			TComponent* renderer = nullptr;
			if (Xexplosions[i]->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
			{
				TRendererComponent* tRenderer = (TRendererComponent*)renderer;
				if (tRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(Xexplosions[i]);
			}
			if (Zexplosions[i]->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
			{
				TRendererComponent* tRenderer = (TRendererComponent*)renderer;
				if (tRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(Zexplosions[i]);
			}
		}

		//Render Item
		if (item)
		{
			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;
			if (item->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(item);
			}
		}



		/*

				if (p1Ex)
				{
					for (int i = 0; i < objects.size(); ++i)
					{
						if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
						{
							TComponent* cRenderer = nullptr;
							TTransformComponent* renderer = nullptr;
							if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
							{
								renderer = (TTransformComponent*)cRenderer;

								if (renderer->destroyable == true && renderer->item == true)
								{
									if (abs(renderer->fPosition.x - p1EXx) < 4.5f && abs(renderer->fPosition.z - p1EXz) < 1.8f)
									{
										item = p_cEntityManager->ItemDrop(objects.at(i));
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
										//SpawnObject(i, objects, p_cRendererManager, p_cEntityManager);
									}
									else if (abs(renderer->fPosition.z - p1EXz) < 4.5f && abs(renderer->fPosition.x - p1EXx) < 1.8f)
									{
										item = p_cEntityManager->ItemDrop(objects.at(i));
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
										//SpawnObject(i, objects, p_cRendererManager, p_cEntityManager);
									}
								}
								else if (renderer->destroyable == true && renderer->item == false)
								{
									if (abs(renderer->fPosition.x - p1EXx) < 4.5f && abs(renderer->fPosition.z - p1EXz) < 1.8f)
									{
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
									}
									else if (abs(renderer->fPosition.z - p1EXz) < 4.5f && abs(renderer->fPosition.x - p1EXx) < 1.8f)
									{
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
									}
								}
							}
						}
					}
				}
				if (p2Ex)
				{
					for (int i = 0; i < objects.size(); ++i)
					{
						if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
						{
							TComponent* cRenderer = nullptr;
							TTransformComponent* renderer = nullptr;
							if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
							{
								renderer = (TTransformComponent*)cRenderer;

								if (renderer->destroyable == true && renderer->item == true)
								{
									if (abs(renderer->fPosition.x - p2EXx) < 4.5f && abs(renderer->fPosition.z - p2EXz) < 1.8f)
									{
										item = p_cEntityManager->ItemDrop(objects.at(i));
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
										//SpawnObject(i, objects, p_cRendererManager, p_cEntityManager);
									}
									else if (abs(renderer->fPosition.z - p2EXz) < 4.5f && abs(renderer->fPosition.x - p2EXx) < 1.8f)
									{
										item = p_cEntityManager->ItemDrop(objects.at(i));
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
										//SpawnObject(i, objects, p_cRendererManager, p_cEntityManager);
									}
								}
								else if (renderer->destroyable == true && renderer->item == false)
								{
									if (abs(renderer->fPosition.x - p2EXx) < 4.5f && abs(renderer->fPosition.z - p2EXz) < 1.8f)
									{
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
									}
									else if (abs(renderer->fPosition.z - p2EXz) < 4.5f && abs(renderer->fPosition.x - p2EXx) < 1.8f)
									{
										objects.erase(objects.begin() + i, objects.begin() + i + 1);
									}
								}
							}
						}
					}
				}
				if (item)
				{
					TComponent* cRenderer = nullptr;
					TRendererComponent* renderer = nullptr;
					if (item->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
					{
						renderer = (TRendererComponent*)cRenderer;
						if (renderer->iUsedLoadState == curGameState)
							p_cRendererManager->RenderObject(*item);
					}
				}
				if (P1EXISTS && curGameState == 3)
				{
					if (p1Ex)
					{
						if (abs(p1x - p1EXx) < 4.5f && abs(p1z - p1EXz) < 1.8f)
						{
							//delete p1;
							p1 = nullptr;
							P1EXISTS = false;
						}
						else if (abs(p1z - p1EXz) < 4.5f && abs(p1x - p1EXx) < 1.8f)
						{
							//delete p1;
							p1 = nullptr;
							P1EXISTS = false;
						}
					}
					if (p2Ex)
					{
						if (abs(p1x - p2EXx) < 4.5f && abs(p1z - p2EXz) < 1.8f)
						{
							//delete p1;
							p1 = nullptr;
							P1EXISTS = false;
						}
						else if (abs(p1z - p2EXz) < 4.5f && abs(p1x - p2EXx) < 1.8f)
						{
							//delete p1;
							p1 = nullptr;
							P1EXISTS = false;
						}
					}
				}
				if (P2EXISTS && curGameState == 3)
					====== =
					*/
					//Render Players
		for (CPlayer* player : v_cPlayers)
		{
			if (!player || !player->isAlive())
				continue;
			TComponent* renderer = nullptr;
			if (player->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
			{
				TRendererComponent* pRenderer = (TRendererComponent*)renderer;
				if (pRenderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject((CObject*)player);
			}
		}
		g_d3dData->updateCameras();

#pragma region Input

		if (g_pInputRecord->GetState(G_KEY_SPACE, errorCode) == 1)
		{
			std::cout << "SPACE WAS PRESSED, G INPUT STYLE";
		}

		if (keys[KEYS::SPACE].pressed())
		{
			g_pSoundPlayer->Play();
		}

#pragma endregion


		if (GetAsyncKeyState('A'))
		{
			LoadLines();
		}


		if (!p_cRendererManager->Draw())
		{
			g_pLogger->LogCatergorized("FAILURE", "Failed to draw");
		}
	}
}

void CGame::LoadObject()
{
	OBJLoadInfo loadInfo;

	//TCollider collider = GetCenter(v_tMeshTemplates[0]);

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



	//loadInfo.position = { -5.0f, 0.0f, 0.0f };
		//loadInfo.position = { -12.5f, 0.0f, 0.0f };
		//loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
		//loadInfo.LoadState = 3;
		//loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		//loadInfo.meshID = 1;
		//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));


	for (float z = -7.5f; z < 15.0f; z += 2.5f)
	{
		for (float x = -12.5f; x < 15.0f; x += 2.5f)
		{
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
			//collider.center.x = GetCenter(v_tMeshTemplates[0]).center.x + loadInfo.position.x;
			//collider.center.y = GetCenter(v_tMeshTemplates[0]).center.y + loadInfo.position.y;
			//collider.center.z = GetCenter(v_tMeshTemplates[0]).center.z + loadInfo.position.z;
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}


	for (float x = -7.5; x <= 7.5; x += 2.5f)
	{
		loadInfo.position = { x, 0.0f, 10 };
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
	for (float z = -2.5; z <= 7.5; z += 2.5f)
	{

		loadInfo.position = { -12.5, 0.0f, z };
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
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.floor = false;
		loadInfo.destroyable = true;
		loadInfo.item = true;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	}
	for (float z = -2.5f; z < 10.0f; z += 2.5f)
	{
		for (float x = -7.5f; x < 10.0f; x += 2.5f)
		{

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
			loadInfo.item = true;
			loadInfo.collisionLayer = COLLISION_LAYERS::DESTROYABLE;
			loadInfo.floor = false;
			loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}
	}

	for (float x = -15; x <= 15; x += 2.5f)
	{


		loadInfo.position = { x, 0, -10 };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
		loadInfo.meshID = MODELS::CUBE;
		loadInfo.LoadState = 3;
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
	for (float z = -7.5; z <= 12.5; z += 2.5f)
	{

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

	loadInfo.position = { 5, 0, 0 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5, 0, 0 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 5, 0, 5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5, 0, 5 };
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

	loadInfo.position = { 10, 0, 10 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -10, 0, -5 };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.collisionLayer = COLLISION_LAYERS::WALL;
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}


CGame::CGame()
{

}

CGame::~CGame()
{
	delete p_cRendererManager;
}

#define PLAYER_SPEED 5.0f

void CGame::WindowResize()
{
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

void CGame::GamePlayLoop(double timePassed)
{

	for (CPlayer* currPlayer : v_cPlayers)
	{
		if (!currPlayer)
			continue;


		float isUpPressed = 0.0f, isDownPressed = 0.0f, isLeftPressed = 0.0f, isRightPressed = 0.0f, isSouthPressed = 0.0f;
		bool controllerConnected;
		g_pControllerInput->IsConnected(currPlayer->GetControllerIndex(), controllerConnected);
		if (controllerConnected)
		{
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
		if (currPlayer->GetComponent(COMPONENT_TYPE::COLLIDER, col))
		{
			TColliderComponent* tCol = (TColliderComponent*)col;
			DirectX::XMStoreFloat4(&tCol->d3dCollider.Orientation, DirectX::XMQuaternionRotationMatrix(pTransform->mObjMatrix));
		}

		if (isUpPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::UP).held())
		{
			canMove = true;
			for (int i = 0; i < objects.size(); ++i)
			{
				if (currPlayer->Collides(objects[i]))
				{
					canMove = false;
				}
				/*
				if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
				{
					TComponent* cTransform = nullptr;
					TTransformComponent* transform = nullptr;
					if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform))
					{
						transform = (TTransformComponent*)cTransform;
						float dZ = abs(pTransform->fPosition.z - transform->fPosition.z);
						float dX = abs(pTransform->fPosition.x - transform->fPosition.x);
						if (dZ < 2.5f && !transform->nFloor && dX < 2.55f)
						{
							if (pTransform->fPosition.z < transform->fPosition.z)
							{
								canMove = false;
								break;
							}
						}
					}
				}
				*/
			}
			if (canMove == true)
				deltaZ += timePassed * PLAYER_SPEED;
		}
		if (isDownPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::DOWN).held())
		{
			canMove = true;

			for (int i = 0; i < objects.size() - 2; ++i)
			{
				if (currPlayer->Collides(objects[i]))
				{
					canMove = false;
				}
				/*
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cTransform = nullptr;
						TTransformComponent* transform = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform))
						{
							transform = (TTransformComponent*)cTransform;
							float dZ = abs(pTransform->fPosition.z - transform->fPosition.z);
							float dX = abs(pTransform->fPosition.x - transform->fPosition.x);
							if (dZ < 2.5f && !transform->nFloor && dX < 2.55f)
							{
								if (pTransform->fPosition.z > transform->fPosition.z)
								{
									canMove = false;
									break;
								}
							}
						}
					}
				*/
			}
			if (canMove == true)
				deltaZ -= timePassed * PLAYER_SPEED;
		}
		if (isLeftPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::LEFT).held())
		{
			canMove = true;

			for (int i = 0; i < objects.size() - 2; ++i)
			{
				if (currPlayer->Collides(objects[i]))
				{
					canMove = false;
				}
				/*
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cTransform = nullptr;
						TTransformComponent* transform = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform))
						{
							transform = (TTransformComponent*)cTransform;
							float dZ = abs(pTransform->fPosition.z - transform->fPosition.z);
							float dX = abs(pTransform->fPosition.x - transform->fPosition.x);
							if (dZ < 2.2f && !transform->nFloor && dX < 1.5f)
							{
								if (pTransform->fPosition.x > transform->fPosition.x)
								{
									canMove = false;
									break;
								}
							}
						}
					}
				*/
			}
			if (canMove == true)
				deltaX -= timePassed * PLAYER_SPEED;

		}
		if (isRightPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::RIGHT).held())
		{
			canMove = true;

			for (int i = 0; i < objects.size() - 2; ++i)
			{
				if (currPlayer->Collides(objects[i]))
				{
					canMove = false;
				}
				/*
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cTransform = nullptr;
						TTransformComponent* transform = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform))
						{
							transform = (TTransformComponent*)cTransform;
							float dZ = abs(pTransform->fPosition.z - transform->fPosition.z);
							float dX = abs(pTransform->fPosition.x - transform->fPosition.x);
							if (dZ < 2.3f && !transform->nFloor && dX < 1.5f)
							{
								if (pTransform->fPosition.x < transform->fPosition.x)
								{
									canMove = false;
									break;
								}
							}
						}
					}
				*/
			}

			if (canMove == true)
				deltaX += timePassed * PLAYER_SPEED;

		}
		for (int i = 0; i < v_cBombs.size(); ++i)
		{
			if (!v_cBombs[i] || !v_cBombs[i]->isAlive())
				continue;
			TComponent* bTransform;
			if (v_cBombs[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, bTransform))
			{
				TTransformComponent* transform = (TTransformComponent*)bTransform;
				float dz = abs(pTransform->fPosition.z - transform->fPosition.z);
				float dx = abs(pTransform->fPosition.x - transform->fPosition.x);

				if (dz < 1.5f && dx < 1.5f)
				{
					if (dz < dx)
					{
						if (pTransform->fPosition.x < transform->fPosition.x)
						{
							currPlayer->Move(-0.1f, 0.0f - (pTransform->fPosition.z - transform->fPosition.z));
						}
						else if (pTransform->fPosition.x > transform->fPosition.x)
						{
							currPlayer->Move(0.1f, 0.0f - (pTransform->fPosition.z - transform->fPosition.z));
						}
					}
					else if (dx < dz)
					{
						if (pTransform->fPosition.z < transform->fPosition.z)
						{
							currPlayer->Move(0.0f - (pTransform->fPosition.x - transform->fPosition.x), -0.1f);
						}
						else if (pTransform->fPosition.z > transform->fPosition.z)
						{
							currPlayer->Move(0.0f - (pTransform->fPosition.x - transform->fPosition.x), 0.1f);
						}
					}
				}
			}
		}
		if (isSouthPressed == 1.0f || keyboardInputs[currPlayer->GetControllerIndex()].At(CONTROL_KEYS::BOMB).held())
		{
			for (int i = 0; i < maxNumBombs; ++i)
			{
				if (v_cBombs[i] == NULL)
				{
					if (currPlayer->hasAvailableBombSlot())
					{

						currPlayer->AddBombIndex(i);
						v_cBombs[i] = p_cEntityManager->DropBomb(currPlayer);
					}
					continue;
				}

				if (!v_cBombs[i]->isAlive())
				{
					if (currPlayer->hasAvailableBombSlot())
					{
						currPlayer->AddBombIndex(i);
						*v_cBombs[i] = *p_cEntityManager->DropBomb(currPlayer);
					}
					continue;
				}
			}
		}

		if (deltaX != 0.0f || deltaZ != 0.0f)
			currPlayer->Move(deltaX, deltaZ);




		for (int i = 0; i < explosionTimers.size(); ++i)
		{
			if (!currPlayer->isAlive())
				continue;
			DirectX::XMFLOAT3 pPos, xPos, zPos;
			pPos = pTransform->fPosition;
			TComponent* xT = nullptr;
			if (Xexplosions[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, xT))
			{
				TTransformComponent* xTrans = (TTransformComponent*)xT;
				xPos = xTrans->fPosition;
				if (abs(pPos.x - xPos.x) < 4.5f && abs(pPos.z - xPos.z) < 1.8f)
				{
					currPlayer->setAlive(false);
				}
			}
			if (!currPlayer->isAlive())
				continue;
			if (Zexplosions[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, xT))
			{
				TTransformComponent* zTrans = (TTransformComponent*)xT;
				zPos = zTrans->fPosition;

				if (abs(pPos.z - zPos.z) < 4.5f && abs(pPos.x - zPos.x) < 1.8f)
				{
					currPlayer->setAlive(false);
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
void CGame::SpawnObject(int i, std::vector<CObject*> objects, CRendererManager* p_cRendererManager, CEntityManager* p_cEntityManager) {
	item = p_cEntityManager->ItemDrop(objects[i]);
	p_cRendererManager->RenderObject(item);
}



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
		ClearPlayersAndBombs();

		break;
	}
	case GAME_STATE::ARCADE_GAME:
	{
		curGameState = 3;
		LoadObject();
		v_cPlayers[0] = p_cEntityManager->InstantiatePlayer(1, DIFFUSE_TEXTURES::CHICKEN1, DirectX::XMFLOAT3(-10.0f, 0.0f, 10.0f));
		v_cPlayers[1] = p_cEntityManager->InstantiatePlayer(2, DIFFUSE_TEXTURES::CHICKEN2, DirectX::XMFLOAT3(10.0f, 0.0f, -5.0f));

		break;
	}
	case GAME_STATE::WIN_SCREEN:
	{
		ClearPlayersAndBombs();

	}
	default:
		break;
	}
	curGameState = _gameState;
}

void CGame::ClearPlayersAndBombs()
{
	for (int i = 0; i < v_cPlayers.size(); ++i)
		if (v_cPlayers[i])
		{
			delete v_cPlayers[i];
			v_cPlayers[i] = nullptr;
		}
	v_cBombs.clear();
	v_cBombs.resize(maxNumBombs);
	Xexplosions.clear();
	Zexplosions.clear();
	explosionTimers.clear();
	objects.clear();
}

void CGame::updateBombs(double timePassed)
{
	for (int i = 0; i < explosionTimers.size(); ++i)
	{
		if (explosionTimers[i] >= 0.5f)
		{
			Xexplosions.erase(Xexplosions.begin() + i);
			Zexplosions.erase(Zexplosions.begin() + i);
			explosionTimers.erase(explosionTimers.begin() + i);
			--i;
			continue;
		}

		TComponent* exComp;
		Xexplosions[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, exComp);
		TTransformComponent* expTransf = (TTransformComponent*)exComp;
		DirectX::XMFLOAT3 exPos = expTransf->fPosition;
		for (int j = 0; j < objects.size(); ++j)
		{
			TComponent* objC = nullptr;

			if (objects[j]->GetComponent(COMPONENT_TYPE::TRANSFORM, objC))
			{
				TTransformComponent* objT = (TTransformComponent*)objC;
				if (objT->destroyable)
				{
					if ((abs(objT->fPosition.x - exPos.x) < 4.5f && abs(objT->fPosition.z - exPos.z) < 1.8f) || (abs(objT->fPosition.x - exPos.x) < 1.8f && abs(objT->fPosition.z - exPos.z) < 4.5f))
					{
						if (objT->item)
						{
							item = p_cEntityManager->ItemDrop(objects[j]);
						}
						objects.erase(objects.begin() + j);
						--j;
					}
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
				CPlayer* parent = v_cBombs[i]->getParent();
				for (int j = 0; j < parent->getBombIndices().size(); ++j)
				{
					if (parent->getBombIndices()[j] == i)
						parent->deleteBomb(i);
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

bool CGame::loadTempMenus()
{
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
