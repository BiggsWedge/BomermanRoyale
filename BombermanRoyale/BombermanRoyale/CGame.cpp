#include "CGame.h"
#include <DirectXCollision.h>

struct key
{
	bool prevState = false;
	bool currState = false;

	inline bool pressed() { return currState && !prevState; }
	inline bool held() { return currState && prevState; }
	inline bool idle() { return !currState && !prevState; }
	inline bool released() { return !currState && prevState; }
};

static std::vector<key> keys(6);

struct KEYS
{
	enum { UP, DOWN, LEFT, RIGHT, ZERO, RMB };
};

static std::vector<int> keycodes =
{
	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,
	VK_NUMPAD0,
	VK_RBUTTON
};

bool CGame::Initialize()
{
	p_cRendererManager = new CRendererManager();

	return true;
}

void CGame::Run()
{
	POINT currCursor, prevCursor;
	GetCursorPos(&currCursor);
	GW::SYSTEM::GWindowInputEvents gLastEvent;
	while (G_SUCCESS(g_pWindow->GetLastEvent(gLastEvent)) && gLastEvent != GW::SYSTEM::GWindowInputEvents::DESTROY)
	{
		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;
		prevCursor = currCursor;
		GetCursorPos(&currCursor);

		for (int i = 0; i < 6; ++i)
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

		if (keys[KEYS::ZERO].pressed())
			g_d3dData->ToggleUseDebugCamera();

		if (keys[KEYS::RMB].held())
		{
			g_d3dData->debugCursorRot.y = 0.002f*(currCursor.y - prevCursor.y);
			g_d3dData->debugCursorRot.x = 0.002f*(currCursor.x - prevCursor.x);
		}
		g_d3dData->updateCameras();

		for (int i = 0; i < objects.size(); ++i)
		{
			if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
			{
				p_cRendererManager->RenderObject(*objects[i]);
			}
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

	loadInfo.usedDiffuse = -1;
	loadInfo.meshID = 2;

	loadInfo.position = { 0.0f, 0.0f, 0.0f };
	loadInfo.scale = DirectX::XMFLOAT3(10.0f, 10.0f, 10.0f);
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

CGame::CGame()
{

}

CGame::~CGame()
{
	delete p_cRendererManager;
}
