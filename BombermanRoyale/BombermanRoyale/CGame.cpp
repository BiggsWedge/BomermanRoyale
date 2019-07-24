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

		p_cRendererManager->Draw();
	}
}

void CGame::LoadObject()
{
	OBJLoadInfo loadInfo;
	loadInfo.position = { 0.0f, 0.0f, 0.0f };
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::CRATE];
	loadInfo.usedVertex = g_d3dData->d3dVertexShader[VERTEX_SHADER::BASIC];
	loadInfo.usedPixel = g_d3dData->d3dPixelShader[PIXEL_SHADER::BASIC];
	loadInfo.usedInput = g_d3dData->d3dInputLayout[INPUT_LAYOUT::BASIC];
	loadInfo.usedGeo = nullptr;

	objects.push_back(p_cEntityManager->CreateOBJFromTemplate(loadInfo));
}

CGame::CGame()
{

}


CGame::~CGame()
{
	delete p_cRendererManager;
}
