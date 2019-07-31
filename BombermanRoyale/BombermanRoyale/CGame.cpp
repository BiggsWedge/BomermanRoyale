#include "CGame.h"


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
		if (G_FAIL(g_pWindow->ProcessWindowEvents()))
			break;
		for (int i = 0; i < objects.size(); ++i)
		{
			if (p_cRendererManager->HasComponent(*(objects[i]), COMPONENT_TYPE::RENDERER))
			{
				p_cRendererManager->RenderObject(*objects[i]);
			}
		}
		if (GetAsyncKeyState('A'))
		{
			LoadLines();
		}

		p_cRendererManager->Draw();
	}
}

void CGame::LoadObject()
{
	OBJLoadInfo loadInfo;
	TCollider collider;
	loadInfo.position = { 0.0f, 0.0f,0.0f };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::CRATE;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	collider.center.x = GetCenter(v_tMeshTemplates[0]).center.x + loadInfo.position.x;
	collider.center.y = GetCenter(v_tMeshTemplates[0]).center.y + loadInfo.position.y;
	collider.center.z = GetCenter(v_tMeshTemplates[0]).center.z + loadInfo.position.z;
	loadInfo.Collider.Center = collider.center;
	loadInfo.Collider.Extents = collider.extents;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));

	loadInfo.position = { -5.0f, 0.0f, -2.0f };
	collider.center.x = GetCenter(v_tMeshTemplates[0]).center.x + loadInfo.position.x;
	collider.center.y = GetCenter(v_tMeshTemplates[0]).center.y + loadInfo.position.y;
	collider.center.z = GetCenter(v_tMeshTemplates[0]).center.z + loadInfo.position.z;
	loadInfo.Collider.Center = collider.center;
	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

CGame::CGame()
{

}


CGame::~CGame()
{
	delete p_cRendererManager;
}
