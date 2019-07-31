#include "CGame.h"

struct key
{
	bool prevState = false;
	bool currState = false;

	inline bool pressed() { return currState && !prevState; }
	inline bool held() { return currState && prevState; }
	inline bool idle() { return !currState && !prevState; }
	inline bool released() { return !currState && prevState; }
};

key fullScreenButton;
key GameStateButton;

bool CGame::Initialize()
{
	p_cRendererManager = new CRendererManager();

	return true;
}

void CGame::Run()
{
	GW::SYSTEM::GWindowInputEvents gLastEvent;
	while (G_SUCCESS(g_pWindow->GetLastEvent(gLastEvent)) && gLastEvent != GW::SYSTEM::GWindowInputEvents::DESTROY)
	{
		fullScreenButton.prevState = fullScreenButton.currState;
		
		fullScreenButton.currState = GetAsyncKeyState('F') & 0x8000;
		
		GameStateButton.prevState = GameStateButton.currState;

		GameStateButton.currState = GetAsyncKeyState('G') & 0x8000;

		if (fullScreenButton.pressed())
		{
			FullScreen = !FullScreen;
			this->WindowResize();
		}
		
		if (GameStateButton.pressed())
		{

			if (curGameState == 0)
				curGameState = 3;
			else if(curGameState == 3)
				curGameState = 0;

		}
		
		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;
		for (int i = 0; i < objects.size(); ++i)
		{
			if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
			{
				//TComponent* cRenderer = nullptr;
				TRendererComponent* renderer = nullptr;
				if (objects[i]->GetComponent(COMPONENT_TYPE::RENDERER, renderer))
				{
					//renderer = (TRendererComponent*)cRenderer;
					if(renderer->iUsedLoadState == curGameState)
						p_cRendererManager->RenderObject(*objects[i]);
				}

				
				
			}
		}

		p_cRendererManager->Draw();
	}
}


void CGame::LoadObject()
{
	g_pWindow->GetClientWidth(width);
	g_pWindow->GetClientHeight(height);

	OBJLoadInfo loadInfo;
	
		loadInfo.position = { 0.0f, 0.0f, 0.0f };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);

		
	
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));


		loadInfo.position = { -5.0f, 0.0f, 0.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
		loadInfo.LoadState = 3;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		loadInfo.meshID = 1;
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { 0.0f, -13.3f, 15.0f };
		loadInfo.forwardVec = { 0.0f, 0.95f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::NAMES_HUD;

		loadInfo.scale = DirectX::XMFLOAT3(2.4f, 0.25f, 1.0f);
		loadInfo.meshID = 2;
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { 2.5f, 0.0f, 0.0f };
		loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
		loadInfo.meshID = 0;
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::RED_TEX;
		loadInfo.usedVertex = VERTEX_SHADER::BASIC;
		loadInfo.usedPixel = PIXEL_SHADER::BASIC;
		loadInfo.usedInput = INPUT_LAYOUT::BASIC;
		loadInfo.usedGeo = -1;
		loadInfo.LoadState = 3;
		loadInfo.scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

		loadInfo.position = { -2.5f, 0.0f, 0.0f };
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


	
		loadInfo.position = { 0.0f, 0.77f, 5.0f };
		loadInfo.forwardVec = { 0.0f, 0.85f, -1.0f };
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::MAIN_MENU;
		loadInfo.scale = DirectX::XMFLOAT3(1.75f, 1.99f, 1.0f);
		loadInfo.meshID = 2;
		loadInfo.LoadState = 0;
		objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
	

	
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
	



	if (FullScreen )
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