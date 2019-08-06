#include "CGame.h"
#include <DirectXCollision.h>
#include <iostream>

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

static std::vector<key> keys(19);

struct KEYS
{
	enum { UP, DOWN, LEFT, RIGHT, ZERO, RMB, SPACE, P2UP, P2LEFT, P2DOWN, P2RIGHT, P1UP, P1LEFT, P1DOWN, P1RIGHT, P2BOMB, P1BOMB, HELP_MENU };
};

static std::vector<int> keycodes =
{
	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,
	VK_NUMPAD0,
	VK_RBUTTON,
	VK_SPACE,
	VK_NUMPAD5,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	'I',
	'J',
	'K',
	'L',
	VK_NUMPAD6,
	'O',
	VK_F1
};


key fullScreenButton;
key GameStateButton;
key SFXButton;
key P1BOMB;
key P2BOMB;
key ControlScreenButton;

bool Player1 = false;
bool Player2 = false;
bool P1EXISTS = false;
bool P2EXISTS = false;
bool ControlScreenToggle = false;
bool Controller1Alive = false;
bool Controller2Alive = false;
float3 P1POS = { -10.0f, 0.0f, 10.0f };
float3 P2POS = { 10.0f, 0.0f, -5.0f };

float isLDPADPressed = 0.0f;
float isRDPADPressed = 0.0f;
float isUDPADPressed = 0.0f;
float isDDPADPressed = 0.0f;
float isSouthButtonPressed = 0.0f;

float isP1LDPADPressed = 0.0f;
float isP1RDPADPressed = 0.0f;
float isP1UDPADPressed = 0.0f;
float isP1DDPADPressed = 0.0f;
float isP1SouthButtonPressed = 0.0f;


bool CGame::Initialize()
{
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
		fullScreenButton.prevState = fullScreenButton.currState;

		fullScreenButton.currState = GetAsyncKeyState('F') & 0x8000;

		GameStateButton.prevState = GameStateButton.currState;

		GameStateButton.currState = GetAsyncKeyState('G') & 0x8000;

		SFXButton.prevState = SFXButton.currState;
		SFXButton.currState = GetAsyncKeyState(keycodes[KEYS::SPACE]) & 0x8000;

		ControlScreenButton.prevState = ControlScreenButton.currState;

		ControlScreenButton.currState = GetAsyncKeyState(keycodes[KEYS::HELP_MENU]) & 0x8000;

		if (fullScreenButton.pressed())
		{
			FullScreen = !FullScreen;
			this->WindowResize();
		}

		if (GameStateButton.pressed())
		{

			if (curGameState == 0)
			{
				curGameState = 3;
				P1EXISTS = true;
				P2EXISTS = true;
				TComponent* cTransform = nullptr;
				TTransformComponent* Transform = nullptr;
				p2->GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform);
				Transform = (TTransformComponent*)cTransform;
				float Dx = P2POS.x - Transform->fPosition.x;
				float Dz = P2POS.z - Transform->fPosition.z;
				p2->Move(Dx, Dz);
				p1->GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform);
				Transform = (TTransformComponent*)cTransform;
				Dx = P1POS.x - Transform->fPosition.x;
				Dz = P1POS.z - Transform->fPosition.z;
				p1->Move(Dx, Dz);
			}
			else if (curGameState == 3)
			{
				curGameState = 0;
				P1EXISTS = true;
				P2EXISTS = true;
			}
			else if (curGameState == 5)
			{
				curGameState = 0;
				P1EXISTS = true;
				P2EXISTS = true;
			}

		}

		if (ControlScreenButton.pressed())
		{
			if (ControlScreenToggle == false)
				ControlScreenToggle = true;
			else
				ControlScreenToggle = false;
		}

		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;
		prevCursor = currCursor;
		GetCursorPos(&currCursor);

		for (int i = 0; i < keycodes.size(); ++i)
		{
			keys[i].prevState = keys[i].currState;
			keys[i].currState = GetAsyncKeyState(keycodes[i]) & 0x8000;
		}
		g_d3dData->debugCamDelta = { 0.0f, 0.0f };
		if (keys[KEYS::UP].held())
			g_d3dData->debugCamDelta.y += 0.1f;
		if (keys[KEYS::DOWN].held())
			g_d3dData->debugCamDelta.y += -0.1f;
		if (keys[KEYS::LEFT].held())
			g_d3dData->debugCamDelta.x += -0.1f;
		if (keys[KEYS::RIGHT].held())
			g_d3dData->debugCamDelta.x += 0.1f;

		if (!P2EXISTS && P1EXISTS)
		{
			curGameState = 5;
			p2 = objects.at(objects.size() - 2);
			
			
		}
		if (!P1EXISTS && P2EXISTS)
		{
			curGameState = 5;
			p1 = objects.at(objects.size() - 1);
			
		}
		if (!P2EXISTS && !P1EXISTS)
		{
			curGameState = 5;
			p2 = objects.at(objects.size() - 2);
			p1 = objects.at(objects.size() - 1);
			
		}

		if (P2EXISTS)
		{
			TComponent* p2Component;
			p2->GetComponent(COMPONENT_TYPE::TRANSFORM, p2Component);
			TTransformComponent* p2Transform = (TTransformComponent*)p2Component;
			if (g_pControllerInput->IsConnected(0,Controller1Alive))
			{
				g_pControllerInput->GetState(0, G_DPAD_UP_BTN, isUDPADPressed);
				g_pControllerInput->GetState(0, G_DPAD_DOWN_BTN, isDDPADPressed);
				g_pControllerInput->GetState(0, G_DPAD_LEFT_BTN, isLDPADPressed);
				g_pControllerInput->GetState(0, G_DPAD_RIGHT_BTN, isRDPADPressed);
				g_pControllerInput->GetState(0, G_SOUTH_BTN, isSouthButtonPressed);
			}

			if (g_pControllerInput->IsConnected(1, Controller2Alive))
			{
				g_pControllerInput->GetState(1, G_DPAD_UP_BTN, isP1UDPADPressed);
				g_pControllerInput->GetState(1, G_DPAD_DOWN_BTN, isP1DDPADPressed);
				g_pControllerInput->GetState(1, G_DPAD_LEFT_BTN, isP1LDPADPressed);
				g_pControllerInput->GetState(1, G_DPAD_RIGHT_BTN, isP1RDPADPressed);
				g_pControllerInput->GetState(1, G_SOUTH_BTN, isP1SouthButtonPressed);
			}
			
			//TComponent* p1Component;
			//TTransformComponent* p1Transform;
			//if (P1EXISTS)
			//{
			//	p1->GetComponent(COMPONENT_TYPE::TRANSFORM, p1Component);
			//	p1Transform = (TTransformComponent*)p1Component;
			//}
			p2Move = true;
			float deltaP2X = 0.0f, deltaP2Z = 0.0f;
			if (keys[KEYS::P2UP].held() || isUDPADPressed == 1.0f)
			{
				//std::cout << isUDPADPressed;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p2Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p2Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dZ < 2.0f && !renderer->nFloor && dX < 2.05f)
							{
								if (p2Transform->fPosition.z < renderer->fPosition.z)
								{
									p2Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}
			
			
			
					}
				}
				if (p2Move == true)
					deltaP2Z += 0.1f;

			}
			if (keys[KEYS::P2DOWN].held() || isDDPADPressed == 1.0f)
			{
				p2Move = true;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p2Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p2Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dZ < 1.6f && !renderer->nFloor && dX < 2.05f)
							{
								if (p2Transform->fPosition.z > renderer->fPosition.z)
								{
									p2Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if (p2Move == true)
					deltaP2Z += -0.1f;
			}
			if (keys[KEYS::P2LEFT].held() || isLDPADPressed == 1.0f)
			{
				p2Move = true;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p2Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p2Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dX < 2.2f && !renderer->nFloor && dZ < 1.5f)
							{
								if (p2Transform->fPosition.x > renderer->fPosition.x)
								{
									p2Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if (p2Move == true)
					deltaP2X += -0.1f;
			}
			if (keys[KEYS::P2RIGHT].held() || isRDPADPressed == 1.0f)
			{
				p2Move = true;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p2Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p2Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dX < 2.3f && !renderer->nFloor && dZ < 1.5f)
							{
								if (p2Transform->fPosition.x < renderer->fPosition.x)
								{
									p2Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if (p2Move == true)
					deltaP2X += 0.1f;
			}
			if ((keys[KEYS::P2BOMB].pressed() && p2B == nullptr ) || (isSouthButtonPressed == 1.0f && p2B == nullptr))
			{
				p2B = p_cEntityManager->DropBomb(p2);
				p2BTimer = 0.0f;
			}
			if (deltaP2X != 0.0f || deltaP2Z != 0.0f)
				p2->Move(deltaP2X, deltaP2Z);


			/*
			OBJLoadInfo loadInfo;
			loadInfo.position = { P2POS.x, 0.0f, P2POS.z };
			loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
			loadInfo.meshID = 0;
			loadInfo.usedDiffuse = DIFFUSE_TEXTURES::RED_TEX;
			loadInfo.usedVertex = VERTEX_SHADER::BASIC;
			loadInfo.usedPixel = PIXEL_SHADER::BASIC;
			loadInfo.usedInput = INPUT_LAYOUT::BASIC;
			loadInfo.usedGeo = -1;
			loadInfo.LoadState = 3;
			loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
			if(!P2EXISTS)
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
			else
				objects.at(objects.size() - 2) = p_cEntityManager->CreateOBJFromTemplate(loadInfo)

			P2EXISTS = true;

			*/
			//TComponent* cRenderer = nullptr;
			//TTransformComponent* renderer = nullptr;
			//if (objects[objects.size() - 1]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
			//{
			//	renderer = (TTransformComponent*)cRenderer;
			//	if (curGameState == 3)
			//	{
			//		if (keys[KEYS::P2UP].held())
			//			renderer->fPosition.y += 0.1f;
			//		if (keys[KEYS::P2DOWN].held())
			//			renderer->fPosition.y += -0.1f;
			//		if (keys[KEYS::P2LEFT].held())
			//			renderer->fPosition.x += -0.1f;
			//		if (keys[KEYS::P2RIGHT].held())
			//			renderer->fPosition.x += 0.1f;
			//		objects[0].
			//	}
			//}
		}
		if (P1EXISTS)
		{
			TComponent* p1Component;
			p1->GetComponent(COMPONENT_TYPE::TRANSFORM, p1Component);
			TTransformComponent* p1Transform = (TTransformComponent*)p1Component;
			//TComponent* p2Component;
			//TTransformComponent* p2Transform;
			//if (P2EXISTS)
			//{
			//	p2->GetComponent(COMPONENT_TYPE::TRANSFORM, p2Component);
			//	p2Transform = (TTransformComponent*)p2Component;
			//}
			p1Move = true;
			float deltaP1X = 0.0f, deltaP1Z = 0.0f;
			if (keys[KEYS::P1UP].held() || isP1UDPADPressed == 1.0f)
			{
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p1Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p1Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dZ < 2.0f && !renderer->nFloor && dX < 2.05f)
							{
								if (p1Transform->fPosition.z < renderer->fPosition.z)
								{
									p1Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if(p1Move == true)
					deltaP1Z += 0.1f;
				
			}
			if (keys[KEYS::P1DOWN].held() || isP1DDPADPressed == 1.0f)
			{
				p1Move = true;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p1Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p1Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dZ < 1.6f && !renderer->nFloor && dX < 2.05f)
							{
								if (p1Transform->fPosition.z > renderer->fPosition.z)
								{
									p1Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if (p1Move == true)
					deltaP1Z += -0.1f;
			}
			if (keys[KEYS::P1LEFT].held() || isP1LDPADPressed == 1.0f)
			{
				p1Move = true;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p1Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p1Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dX < 2.2f && !renderer->nFloor && dZ < 1.5f)
							{
								if (p1Transform->fPosition.x > renderer->fPosition.x)
								{
									p1Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if (p1Move == true)
					deltaP1X += -0.1f;
			}
			if (keys[KEYS::P1RIGHT].held() || isP1RDPADPressed == 1.0f)
			{
				p1Move = true;
				for (int i = 0; i < objects.size() - 2; ++i)
				{
					if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
					{
						TComponent* cRenderer = nullptr;
						TTransformComponent* renderer = nullptr;
						if (objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
						{
							renderer = (TTransformComponent*)cRenderer;
							float dZ = abs(p1Transform->fPosition.z - abs(renderer->fPosition.z));
							float dX = abs(p1Transform->fPosition.x - abs(renderer->fPosition.x));
							if (dX < 2.1f && !renderer->nFloor && dZ < 1.5f)
							{
								if (p1Transform->fPosition.x < renderer->fPosition.x)
								{
									p1Move = false;
									break;
								}
							}
							//dZ = abs(p1Transform->fPosition.z - p2Transform->fPosition.z);
							//dX = abs(p1Transform->fPosition.x - p2Transform->fPosition.x);
							//if(dZ < 0.2f && dX < 1.25f)
							//{
							//	p1Move = false;
							//	break;
							//}
						}



					}
				}
				if (p1Move == true)
					deltaP1X += 0.1f;
			}
			if ((keys[KEYS::P1BOMB].pressed() && p1B == nullptr) || (isP1SouthButtonPressed == 1.0f && p1B == nullptr))
			{
				p1B = p_cEntityManager->DropBomb(p1);
				p1BTimer = 0.0f;
			}
			if (deltaP1X != 0.0f || deltaP1Z != 0.0f)
				p1->Move(deltaP1X, deltaP1Z);
			/*
			OBJLoadInfo loadInfo;
			loadInfo.position = { P1POS.x, 0.0f, P1POS.z };
			loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
			loadInfo.meshID = 0;
			loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLUE_TEX;
			loadInfo.usedVertex = VERTEX_SHADER::BASIC;
			loadInfo.usedPixel = PIXEL_SHADER::BASIC;
			loadInfo.usedInput = INPUT_LAYOUT::BASIC;
			loadInfo.usedGeo = -1;
			loadInfo.LoadState = 3;
			loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
			P1EXISTS = true;
			*/

			//TComponent* cRenderer = nullptr;
			//TTransformComponent* renderer = nullptr;
			//if (objects[2]->GetComponent(COMPONENT_TYPE::TRANSFORM, cRenderer))
			//{
			//	renderer = (TTransformComponent*)cRenderer;
			//	if (curGameState == 3)
			//	{
			//		if (keys[KEYS::P1UP].held())
			//			renderer->fPosition.y += 0.1f;
			//		if (keys[KEYS::P1DOWN].held())
			//			renderer->fPosition.y += -0.1f;
			//		if (keys[KEYS::P1LEFT].held())
			//			renderer->fPosition.x += -0.1f;
			//		if (keys[KEYS::P1RIGHT].held())
			//			renderer->fPosition.x += 0.1f;
			//	}
			//}
		}


		if (keys[KEYS::ZERO].pressed())
			g_d3dData->ToggleUseDebugCamera();

		if (keys[KEYS::RMB].held())
		{
			g_d3dData->debugCursorRot.y = 0.002f*(currCursor.y - prevCursor.y);
			g_d3dData->debugCursorRot.x = 0.002f*(currCursor.x - prevCursor.x);
		}
		g_d3dData->updateCameras();

		if (p2B)
		{
			p2BTimer += 0.1f;
			if (p2BTimer >= 18)
			{

				g_pSoundPlayer->Play();
				p2Ex = p_cEntityManager->BombExplosionX(p2B);
				p2Ez = p_cEntityManager->BombExplosionZ(p2B);
				p2ETimer = 0.0f;
				delete p2B;
				p2B = nullptr;
			}
		}

		if (p2Ex)
		{
			p2ETimer += 0.1f;
			if (p2ETimer >= 6)
			{

				
				delete p2Ex;
				delete p2Ez;
				p2Ex = nullptr;
				p2Ez = nullptr;
			}
		}

		if (p1B)
		{
			p1BTimer += 0.1f;
			if (p1BTimer >= 18)
			{

				g_pSoundPlayer->Play();
				p1Ex = p_cEntityManager->BombExplosionX(p1B);
				p1Ez = p_cEntityManager->BombExplosionZ(p1B);
				p1ETimer = 0.0f;
				delete p1B;
				p1B = nullptr;
			}
		}

		if (p1Ex)
		{
			p1ETimer += 0.1f;
			if (p1ETimer >= 6)
			{


				delete p1Ex;
				delete p1Ez;
				p1Ex = nullptr;
				p1Ez = nullptr;
			}
		}

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
						p_cRendererManager->RenderObject(*objects[i]);
				}
				if (ControlScreenToggle == true)
				{
					if (objects[i]->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
					{
						renderer = (TRendererComponent*)cRenderer;
						if (renderer->iUsedLoadState == GameState::CONTROLS_SCREEN)
							p_cRendererManager->RenderObject(*objects[i]);
					}
				}


			}
		}

		if (p1B)
		{

			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;
			if (p1B->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(*p1B);
			}
		}
		if (p2B)
		{

			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;
			if (p2B->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(*p2B);
			}
		}
		if (p1Ex)
		{

			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;
			if (p1Ex->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(*p1Ex);
			}
			if (p1Ez->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(*p1Ez);
			}
		}
		if (p2Ex)
		{

			TComponent* cRenderer = nullptr;
			TRendererComponent* renderer = nullptr;
			if (p2Ex->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(*p2Ex);
			}
			if (p2Ez->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
			{
				renderer = (TRendererComponent*)cRenderer;
				if (renderer->iUsedLoadState == curGameState)
					p_cRendererManager->RenderObject(*p2Ez);
			}
		}

		TComponent* Component;
		TTransformComponent* p1Transform;
		TTransformComponent* p2Transform;
		TTransformComponent* p1ExTransform;
		TTransformComponent* p2ExTransform;
		float p1x;
		float p1z;
		float p2x;
		float p2z;
		float p1EXx;
		float p1EXz;
		float p2EXx;
		float p2EXz;
		if (p1)
		{
			p1->GetComponent(COMPONENT_TYPE::TRANSFORM, Component);
			p1Transform = (TTransformComponent*)Component;
			p1x = p1Transform->fPosition.x;
			p1z = p1Transform->fPosition.z;
		}
		if (p2)
		{
			p2->GetComponent(COMPONENT_TYPE::TRANSFORM, Component);
			p2Transform = (TTransformComponent*)Component;
			p2x = p2Transform->fPosition.x;
			p2z = p2Transform->fPosition.z;
		}
		if (p1Ex)
		{
			p1Ex->GetComponent(COMPONENT_TYPE::TRANSFORM, Component);
			p1ExTransform = (TTransformComponent*)Component;
			p1EXx = p1ExTransform->fPosition.x;
			p1EXz = p1ExTransform->fPosition.z;

			//p1Ez->GetComponent(COMPONENT_TYPE::TRANSFORM, Component);
			//TTransformComponent* p1EzTransform = (TTransformComponent*)Component;
			//float p1EZx = p1EzTransform->fPosition.x;
			//float p1EZz = p1EzTransform->fPosition.z;
		}
		if (p2Ex)
		{
			p2Ex->GetComponent(COMPONENT_TYPE::TRANSFORM, Component);
			p2ExTransform = (TTransformComponent*)Component;
			p2EXx = p2ExTransform->fPosition.x;
			p2EXz = p2ExTransform->fPosition.z;

			//p2Ez->GetComponent(COMPONENT_TYPE::TRANSFORM, Component);
			//TTransformComponent* p2EzTransform = (TTransformComponent*)Component;
			//float p2EZx = p2EzTransform->fPosition.x;
			//float p2EZz = p2EzTransform->fPosition.z;
		}
		

		if (P1EXISTS && curGameState == 3)
		{
			if (p1Ex)
			{
				if (abs(p1x - p1EXx) < 4.9f && abs(p1z - p1EXz) < 2.0f)
				{

					//delete p1;
					p1 = nullptr;
					P1EXISTS = false;

				}
				else if (abs(p1z - p1EXz) < 4.9f && abs(p1x - p1EXx) < 2.0f)
				{
					//delete p1;
					p1 = nullptr;
					P1EXISTS = false;
				}
			}
			if (p2Ex)
			{
				if (abs(p1x - p2EXx) < 4.9f && abs(p1z - p2EXz) < 2.0f)
				{

					//delete p1;
					p1 = nullptr;
					P1EXISTS = false;

				}
				else if (abs(p1z - p2EXz) < 4.9f && abs(p1x - p2EXx) < 2.0f)
				{
					//delete p1;
					p1 = nullptr;
					P1EXISTS = false;
				}
			}

		}
		if (P2EXISTS && curGameState ==3)
		{
			if (p1Ex)
			{
				if (abs(p2x - p1EXx) < 4.9f && abs(p2z - p1EXz) < 2.0f)
				{

					//delete p2;
					p2 = nullptr;
					P2EXISTS = false;

				}
				else if (abs(p2z - p1EXz) < 4.9f && abs(p2x - p1EXx) < 2.0f)
				{
					//delete p2;
					p2 = nullptr;
					P2EXISTS = false;
				}
			}
			if (p2Ex)
			{
				if (abs(p2x - p2EXx) < 4.9f && abs(p2z - p2EXz) < 2.0f)
				{

					//delete p2;
					p2 = nullptr;
					P2EXISTS = false;

				}
				else if (abs(p2z - p2EXz) < 4.9f && abs(p2x - p2EXx) < 2.0f)
				{
					//delete p2;
					p2 = nullptr;
					P2EXISTS = false;
				}
			}

		}
		


#pragma region Input

		if (g_pInputRecord->GetState(G_KEY_SPACE, errorCode) == 1)
		{
			std::cout << "SPACE WAS PRESSED, G INPUT STYLE";
		}

		if (SFXButton.pressed())
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

	TCollider collider = GetCenter(v_tMeshTemplates[0]);

	//window data(width, height)
	g_pWindow->GetClientWidth(width);
	g_pWindow->GetClientHeight(height);

	loadInfo.position = { 0.0f, 0.0f,0.0f };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.floor = false;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);

	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 12.0f, 0.0f, 0.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.meshID = 1;

	//collider data
	collider.center.x = GetCenter(v_tMeshTemplates[0]).center.x + loadInfo.position.x;
	collider.center.y = GetCenter(v_tMeshTemplates[0]).center.y + loadInfo.position.y;
	collider.center.z = GetCenter(v_tMeshTemplates[0]).center.z + loadInfo.position.z;

	loadInfo.collider.Center = collider.center;
	loadInfo.collider.Extents = collider.extents;

	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -12.0f, 0.0f, 0.0f };

	//collider data
	collider.center.x = GetCenter(v_tMeshTemplates[0]).center.x + loadInfo.position.x;
	collider.center.y = GetCenter(v_tMeshTemplates[0]).center.y + loadInfo.position.y;
	collider.center.z = GetCenter(v_tMeshTemplates[0]).center.z + loadInfo.position.z;

	loadInfo.collider.Center = collider.center;

	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));



	loadInfo.position = { 0.0f, 0.0f, 2.5f };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HAY_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);



	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));


	//loadInfo.position = { -5.0f, 0.0f, 0.0f };
	//loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
	//loadInfo.LoadState = 3;
	//loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	//loadInfo.meshID = 1;
	//objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, -0.8f, 20.0f };
	loadInfo.forwardVec = { 0.0f, 0.95f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::NAMES_HUD;

	loadInfo.scale = DirectX::XMFLOAT3(2.4f, 0.25f, 1.0f);
	loadInfo.meshID = 2;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));


	loadInfo.position = { 0.0f, -2.4f, 3.0f };
	loadInfo.forwardVec = { 0.0f, 0.95f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::MAIN_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(1.75f, 1.99f, 1.0f);
	loadInfo.meshID = 2;
	loadInfo.LoadState = 0;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, -2.4f, 3.0f };
	loadInfo.forwardVec = { 0.0f, 0.95f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::WIN_SCREEN;
	loadInfo.scale = DirectX::XMFLOAT3(1.75f, 1.99f, 1.0f);
	loadInfo.meshID = 2;
	loadInfo.LoadState = 5;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { 0.0f, 5.0f, -4.5f };
	loadInfo.forwardVec = { 0.0f, 0.95f, -1.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::HELP_MENU;
	loadInfo.scale = DirectX::XMFLOAT3(0.8f, 1.0f, 1.0f);
	loadInfo.meshID = 2;
	loadInfo.LoadState = 6;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	for (float x = -10; x <= 10; x += 2.5f)
	{
		for (float z = -5; z <= 10; z += 2.5f)
		{
			loadInfo.position = { x, -2.5f, z };
			loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
			loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
			loadInfo.meshID = 0;
			loadInfo.LoadState = 3;
			loadInfo.floor = true;
			loadInfo.scale = DirectX::XMFLOAT3(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f);
			objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
		}

	}

	loadInfo.position = { P2POS.x, 0.0f, P2POS.z };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::RED_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	p2 = objects.at(objects.size() - 1);
	P2EXISTS = true;

	loadInfo.position = { P1POS.x, 0.0f, P1POS.z };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLUE_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	p1 = objects.at(objects.size() - 1);
	P1EXISTS = true;


}

CGame::CGame()
{

}

CGame::~CGame()
{
	delete p_cRendererManager;
}

void CGame::WindowResize()
{




	if (FullScreen)
	{
		g_pWindow->GetClientWidth(width);
		g_pWindow->GetClientHeight(height);
		g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
		g_pWindow->ChangeWindowStyle(GW::SYSTEM::GWindowStyle::FULLSCREENBORDERLESS);
		g_pWindow->GetClientWidth(width);
		g_pWindow->GetClientHeight(height);
		g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
		g_d3dData->d3dViewport.Height = static_cast<float>(height);
		g_d3dData->d3dViewport.Width = static_cast<float>(width);
		//g_d3dData->d3dSwapChain->SetFullscreenState(FullScreen, nullptr);
	}
	if (!FullScreen)
	{
		g_pWindow->GetClientWidth(width);
		g_pWindow->GetClientHeight(height);
		g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
		g_pWindow->ChangeWindowStyle(GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED);
		g_pWindow->GetClientWidth(width);
		g_pWindow->GetClientHeight(height);
		g_d3dData->projMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
		g_d3dData->d3dViewport.Height = static_cast<float>(height);
		g_d3dData->d3dViewport.Width = static_cast<float>(width);
		//g_d3dData->d3dSwapChain->SetFullscreenState(FullScreen, nullptr);
	}
}

void CGame::GamePlayLoop()
{

}