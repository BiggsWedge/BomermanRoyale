#include "CGame.h"
#include <iostream>

const char* backgroundMusicFilePath = ".//Assets//Music//Level_Music1.wav";
const char* placeHolderSFX = ".//Assets//Music//snd_15186.wav";

bool CGame::Initialize()
{
	p_cRendererManager = new CRendererManager();

	return true;
}

void CGame::Run()
{

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
	GW::SYSTEM::GWindowInputEvents gLastEvent;
	while (G_SUCCESS(g_pWindow->GetLastEvent(gLastEvent)) && gLastEvent != GW::SYSTEM::GWindowInputEvents::DESTROY)
	{
		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;
		for (int i = 0; i < objects.size(); ++i)
		{
			if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
			{
				p_cRendererManager->RenderObject(*objects[i]);
			}
		}

	
#pragma region Input

		if (g_pInputRecord->GetState(G_KEY_SPACE, errorCode) == 1)
		{
			std::cout << "SPACE WAS PRESSED, G INPUT STYLE";
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			if (G_SUCCESS(g_pAudioHolder->CreateSound(placeHolderSFX, &g_pSoundPlayer)))
			{
				g_pSoundPlayer->Play();
				std::cout << "SPACE WAS PRESSED";
			}
		}

#pragma endregion



		p_cRendererManager->Draw();
	}

}

void CGame::LoadObject()
{
	OBJLoadInfo loadInfo;
	loadInfo.position = { 0.0f, 0.0f, 0.0f };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);

	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5.0f, 0.0f, 0.0f };
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;

	loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	loadInfo.meshID = 1;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

CGame::CGame()
{

}


CGame::~CGame()
{
	delete p_cRendererManager;
}
