#include "CRendererManager.h"
#include "CGame.h"


bool CRendererManager::HasComponent(CObject _objToCheck, int _componentToCheck)
{
	for (TComponent* c : _objToCheck.GetComponenets())
		if (c->GetComponentType() == COMPONENT_TYPE::RENDERER)
			return true;
	return false;
}

void CRendererManager::RenderObject(CObject* _objToDraw)
{
	rendereableObjects.push_back(_objToDraw);
}

void CRendererManager::RenderParticle(CObject* _particleToDraw)
{
	renderableParticles.push_back(_particleToDraw);
}

void CRendererManager::RenderObjectCollider(CObject * _objToDraw)
{
	TComponent* tCollider = nullptr;
	if (_objToDraw->GetComponent(COMPONENT_TYPE::COLLIDER, tCollider))
	{
		TColliderComponent* collider = (TColliderComponent*)tCollider;
	}
}

bool CRendererManager::Draw(double timepassed, int gamestate, CGame* parentGame)
{

	if (!g_d3dData->d3dSurface)
	{
		//Log Error
		return false;
	}

	if (!G_SUCCESS(g_d3dData->d3dSurface->GetRenderTarget((void**)&g_d3dData->d3dRenderTargetView)))
	{
		//Log Failure
		return false;
	}

	if (G_SUCCESS(g_d3dData->d3dSurface->GetDepthStencilView((void**)&g_d3dData->d3dDepthStencilView)))
	{
		g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
	}

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };


	g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 0);
	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	const float bg_green[] = { 0.2f, 0.5f, 0.2f, 1.0f };


	g_d3dData->d3dContext->ClearRenderTargetView(g_d3dData->d3dRenderTargetView, DirectX::Colors::Wheat);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);

	if (gamestate == GAME_STATE::ARCADE_GAME || gamestate == GAME_STATE::BATTLE_GAME)
	{
		theSkybox->Update();
		theSkybox->Render();

		SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
		if (G_SUCCESS(g_d3dData->d3dSurface->GetDepthStencilView((void**)&g_d3dData->d3dDepthStencilView)))
		{
			g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
		}

	}


	g_d3dData->d3dContext->OMSetBlendState(g_d3dData->d3dBlendState[BLEND_STATE::DEFAULT], 0, 0xFFFFFFFF);

	/*********DRAW OTHER STUFF HERE************/
#pragma region BATTLE MAGE ANIMATION DRAW

	//g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState2);
	//if (gamestate == 3)
	//	v_tMeshTemplates[MODELS::BATTLEMAGE].render(g_d3dData->d3dContext, timepassed);

#pragma endregion

	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState[RASTERIZER_STATE::DEFAULT]);
	for (CObject* c : rendereableObjects)
		c->Draw(timepassed);

	

	g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::TWO_D], 0);

	g_d3dData->d3dSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, g_d3dData->d3dBlendState[BLEND_STATE::UI]);
	float aspectRatio = (float)g_d3dData->windowWidthHeight.x / (float)g_d3dData->windowWidthHeight.y;
	float invAspect = (float)g_d3dData->windowWidthHeight.y / (float)g_d3dData->windowWidthHeight.x;
	DirectX::GXMVECTOR scale = { 0.1f * aspectRatio, 0.1f * aspectRatio, 0.1f * aspectRatio, 1.0f };
	DirectX::GXMVECTOR scale2 = { 0.2f * aspectRatio, 0.2f * aspectRatio, 0.2f * aspectRatio, 1.0f };
	DirectX::GXMVECTOR scale3 = { 0.5f * aspectRatio, 0.5f * aspectRatio, 0.5f * aspectRatio, 1.0f };
	DirectX::GXMVECTOR scale4 = { 0.13f * aspectRatio, 0.13f * aspectRatio, 0.13f * aspectRatio, 1.0f };


	//g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 0);
	g_d3dData->d3dContext->OMSetBlendState(g_d3dData->d3dBlendState[BLEND_STATE::UI], 0, 0xFFFFFFFF);
	for (CObject* p : renderableParticles)
		p->Draw(timepassed);

	g_d3dData->d3dContext->OMSetDepthStencilState(g_d3dData->d3dDepthStencilState[DEPTH_STENCIL_STATE::TWO_D], 0);

	DirectX::XMVECTOR measurement = g_d3dData->d3dSpriteFont->MeasureString("Arcade Mode");

	if (gamestate == GAME_STATE::ARCADE_GAME || gamestate == GAME_STATE::BATTLE_GAME)
	{
		RECT rect;
		rect.top = 0;
		rect.bottom = (float)g_d3dData->windowWidthHeight.y;
		rect.left = 0.0f;
		rect.right = (float)g_d3dData->windowWidthHeight.x;
		if(parentGame->gameStart >= 0)
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::RED_TEX], rect, nullptr, { (parentGame->gameStart <= 2.0f) ? 0.5f * parentGame->gameStart : 1.0f, (parentGame->gameStart <= 2.0f) ? 0.5f * parentGame->gameStart : 1.0f,  (parentGame->gameStart <= 2.0f) ? 0.5f * parentGame->gameStart : 1.0f,  (parentGame->gameStart <= 2.0f) ? 0.5f * parentGame->gameStart : 1.0f });
		rect.top = 0;
		rect.bottom = 0.13f * (float)g_d3dData->windowWidthHeight.y;
		rect.left = 0.0f;
		rect.right = (float)g_d3dData->windowWidthHeight.x;
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::NAMES_HUD], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		if (gamestate == GAME_STATE::BATTLE_GAME)
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Battle Mode", DirectX::XMVECTOR{ (0.5f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.005f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale4);
		if(gamestate == GAME_STATE::ARCADE_GAME)
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Arcade Mode", DirectX::XMVECTOR{ (0.5f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.005f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale4);
		if (parentGame->playerdisconnect && parentGame->PlayerDisconnectToggle)
		{
			rect.top = 0.13f * (float)g_d3dData->windowWidthHeight.y;
			rect.bottom = 0.53f * (float)g_d3dData->windowWidthHeight.y;
			rect.left = 0.25f * (float)g_d3dData->windowWidthHeight.x;
			rect.right = 0.75f * (float)g_d3dData->windowWidthHeight.x;
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::DC1 + parentGame->PlayerDiscIndex], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		}
		wchar_t startTime[10];
		wchar_t battleduration[10];
		swprintf_s(startTime, L"%d", (int)parentGame->gameStart);
		swprintf_s(battleduration, L"%d", (int)parentGame->BattleDuration);
		if (!parentGame->GetisPaused())
		{
			if (parentGame->BattleDuration >= 194)
			{
				rect.top = 0.32f * (float)g_d3dData->windowWidthHeight.y;
				rect.bottom = 0.6f * (float)g_d3dData->windowWidthHeight.y;
				rect.left = (parentGame->gameStart <= 1) ? 0.45f * (float)g_d3dData->windowWidthHeight.x : 0.45f * (float)g_d3dData->windowWidthHeight.x;
				rect.right = (parentGame->gameStart <= 1) ? 0.57f * (float)g_d3dData->windowWidthHeight.x : 0.52f * (float)g_d3dData->windowWidthHeight.x;
				g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::NAMES_HUD], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			}
			if (parentGame->gameStart >= 1)
				g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, startTime, DirectX::XMVECTOR{ (0.5f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.4f * (float)g_d3dData->windowWidthHeight.y }, (parentGame->gameStart >= 2.0f) ? DirectX::Colors::Red : DirectX::Colors::Green, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale3);
			if (parentGame->gameStart <= 1 && parentGame->BattleDuration >= 194)
				g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "GO", DirectX::XMVECTOR{ (0.5f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.4f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Green, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale3);
			if (parentGame->gameStart <= 0)
				g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, battleduration, DirectX::XMVECTOR{ (0.47f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.12f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale2);
		}
	}

	if (gamestate == GAME_STATE::ARCADE_MENU || gamestate == GAME_STATE::BATTLE_MENU)
	{
		RECT rect2;
		rect2.top = 0.08f * (float)g_d3dData->windowWidthHeight.y;
		rect2.bottom = 0.92f * (float)g_d3dData->windowWidthHeight.y;
		rect2.left = 0.08f * (float)g_d3dData->windowWidthHeight.x;
		rect2.right = 0.92f * (float)g_d3dData->windowWidthHeight.x;
		int texture;
		for (CObject* menu : parentGame->getMenuObjects())
		{
			TComponent* cRenderer;
			TComponent* cTexture;
			if (!menu->GetComponent(COMPONENT_TYPE::RENDERER, cRenderer))
				continue;
			menu->GetComponent(COMPONENT_TYPE::TEXTURE, cTexture);
			TRendererComponent* renderer = (TRendererComponent*)cRenderer;
			TTextureComponent* Texture = (TTextureComponent*)cTexture;
			if (gamestate == GAME_STATE::ARCADE_MENU && renderer->iUsedLoadState == GAME_STATE::ARCADE_MENU && Texture->iUsedDiffuseIndex < DIFFUSE_TEXTURES::BOAR)
			{
				texture = Texture->iUsedDiffuseIndex;
			}
			if (gamestate == GAME_STATE::BATTLE_MENU && renderer->iUsedLoadState == GAME_STATE::BATTLE_MENU && Texture->iUsedDiffuseIndex < DIFFUSE_TEXTURES::BOAR)
			{
				texture = Texture->iUsedDiffuseIndex;
			}
		}
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[texture], rect2, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		wchar_t numP[10];
		wchar_t numA[10];
		swprintf_s(numP, L"%d", parentGame->numPLAYERS);
		swprintf_s(numA, L"%d", parentGame->numAI);
		if (parentGame->mapsize == 1)
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "SMALL", DirectX::XMVECTOR{ (0.49f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.36f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		else if (parentGame->mapsize == 3)
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "LARGE", DirectX::XMVECTOR{ (0.49f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.36f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		else
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "MEDIUM", DirectX::XMVECTOR{ (0.49f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.36f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, numA, DirectX::XMVECTOR{ (0.46f*(float)g_d3dData->windowWidthHeight.x), 0.62f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale2);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, numP, DirectX::XMVECTOR{ (0.46f*(float)g_d3dData->windowWidthHeight.x), 0.48f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale2);
	}
	RECT rect;
	wchar_t score[10];
	int bombtype;
	float x = (0.5f - (0.6f * ((float)g_d3dData->windowWidthHeight.y / (float)g_d3dData->windowWidthHeight.x)));
	rect.top = 0.095f * (float)g_d3dData->windowWidthHeight.y;
	rect.bottom = 0.15f * (float)g_d3dData->windowWidthHeight.y;
	rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
	rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
	if (parentGame->GetPlayer(0))
	{
		bombtype = parentGame->GetPlayer(0)->GetBombType();
		swprintf_s(score, L"%d", parentGame->GetPlayer(0)->getScore());
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 1:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x , (float)g_d3dData->windowWidthHeight.y * 0.045f }, DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 1 Score:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x - (0.03f *(float)g_d3dData->windowWidthHeight.x) , (float)g_d3dData->windowWidthHeight.y * 0.075f }, DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.15f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(0)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetPlayer(0)->isAlive()) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetPlayer(0)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(1))
	{
		x += 0.14f *invAspect;
		rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
		rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
		bombtype = parentGame->GetPlayer(1)->GetBombType();
		swprintf_s(score, L"%d", parentGame->GetPlayer(1)->getScore());
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 2:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, DirectX::Colors::Blue, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 2 Score:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x - (0.03f *(float)g_d3dData->windowWidthHeight.x), (float)g_d3dData->windowWidthHeight.y * 0.075f }, DirectX::Colors::Blue, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(1)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetPlayer(1)->isAlive()) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetPlayer(1)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(2) || parentGame->GetAI(1))
	{
		x += 0.14*invAspect;
		rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
		rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 3:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, DirectX::FXMVECTOR{ 0.059f, 0.231f, 0.047f, 1.0f }, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 3 Score:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x - (0.03f *(float)g_d3dData->windowWidthHeight.x), (float)g_d3dData->windowWidthHeight.y * 0.075f }, DirectX::FXMVECTOR{ 0.059f, 0.231f, 0.047f, 1.0f }, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		x += 0.16f *invAspect;
		if (parentGame->GetPlayer(2) && parentGame->GetAI(1))
		{
			bombtype = parentGame->GetPlayer(2)->GetBombType();
			swprintf_s(score, L"%d", parentGame->GetPlayer(2)->getScore());
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(2)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetPlayer(2)->isAlive()) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetPlayer(2)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

			x += 0.14f * invAspect;
			rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
			rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4 Score:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x - (0.03f *(float)g_d3dData->windowWidthHeight.x), (float)g_d3dData->windowWidthHeight.y * 0.075f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			x += 0.16f *invAspect;
			bombtype = parentGame->GetAI(1)->GetBombType();
			swprintf_s(score, L"%d", parentGame->GetAI(1)->getScore());
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(1)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetAI(1)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetAI(1)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
		else if (parentGame->GetPlayer(2))
		{
			bombtype = parentGame->GetPlayer(2)->GetBombType();
			swprintf_s(score, L"%d", parentGame->GetPlayer(2)->getScore());
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(2)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetPlayer(2)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetPlayer(2)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
		else if (parentGame->GetAI(1))
		{
			bombtype = parentGame->GetAI(1)->GetBombType();
			swprintf_s(score, L"%d", parentGame->GetAI(1)->getScore());
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(1)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetAI(1)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetAI(1)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
	}
	if (parentGame->GetPlayer(3) || parentGame->GetAI(0))
	{
		x += 0.14f * invAspect;
		rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
		rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4 Score:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x - (0.03f *(float)g_d3dData->windowWidthHeight.x), (float)g_d3dData->windowWidthHeight.y * 0.075f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		x += 0.16f *invAspect;
		if (parentGame->GetPlayer(3))
		{
			bombtype = parentGame->GetPlayer(3)->GetBombType();
			swprintf_s(score, L"%d", parentGame->GetPlayer(3)->getScore());
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetPlayer(3)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetPlayer(3)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetPlayer(3)->getScore() < 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
		else if (parentGame->GetAI(0))
		{
			bombtype = parentGame->GetAI(0)->GetBombType();
			swprintf_s(score, L"%d", parentGame->GetAI(0)->getScore());
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BOMB + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(0)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.045f }, (parentGame->GetAI(0)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, score, DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.075f }, (parentGame->GetAI(0)->getScore() > 0) ? DirectX::Colors::Green : DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}

	}
	UINT strides[] = { sizeof(TLineVertex) };
	UINT offsets[] = { 0 };

	//g_d3dData->d3dContext->OMSetBlendState(g_d3dData->d3dBlendState[BLEND_STATE::DEFAULT], 0, 0xFFFFFFFF);
	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState[RASTERIZER_STATE::DEFAULT]);
	g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_LINE], 0, nullptr, get_line_verts(), 0, 0);
	g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_LINE], strides, offsets);
	g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[INPUT_LAYOUT::LINE]);
	g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[VERTEX_SHADER::LINE], 0, 0);

	g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[PIXEL_SHADER::LINE], nullptr, 0);
	g_d3dData->d3dContext->Draw(get_line_vert_count(), 0);
	clear_lines();
	g_d3dData->d3dSpriteBatch->End();

	g_d3dData->d3dSwapChain->Present(1, 0);

	rendereableObjects.clear();
	renderableParticles.clear();

	SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
	SAFE_RELEASE(g_d3dData->d3dRenderTargetView);
	return true;
}

CRendererManager::CRendererManager()
{
	theSkybox = new Skybox();
	theSkybox->Initialize();
}


CRendererManager::~CRendererManager()
{
	for (int i = 0; i < rendereableObjects.size(); ++i)
	{
		if (!(rendereableObjects[i] == nullptr))
			continue;
		delete rendereableObjects[i];
		rendereableObjects[i] = nullptr;
	}

	rendereableObjects.clear();

	for (int i = 0; i < renderableParticles.size(); ++i)
	{
		if (!(renderableParticles[i] == nullptr))
			continue;
		delete renderableParticles[i];
		renderableParticles[i] = nullptr;
	}

	renderableParticles.clear();
	delete theSkybox;
	theSkybox = nullptr;
}
