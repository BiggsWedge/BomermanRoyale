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

	if (gamestate == GAME_STATE::ARCADE_GAME)
	{
		theSkybox->Update();
		theSkybox->Render();

		SAFE_RELEASE(g_d3dData->d3dDepthStencilView);
		if (G_SUCCESS(g_d3dData->d3dSurface->GetDepthStencilView((void**)&g_d3dData->d3dDepthStencilView)))
		{
			g_d3dData->d3dContext->ClearDepthStencilView(g_d3dData->d3dDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
		}

	}




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

	g_d3dData->d3dSpriteBatch->Begin();
	float aspectRatio = (float)g_d3dData->windowWidthHeight.x / (float)g_d3dData->windowWidthHeight.y;
	float invAspect = (float)g_d3dData->windowWidthHeight.y / (float)g_d3dData->windowWidthHeight.x;
	DirectX::GXMVECTOR scale = { 0.1f * aspectRatio, 0.1f * aspectRatio, 0.1f * aspectRatio, 1.0f };
	DirectX::GXMVECTOR scale2 = { 0.2f * aspectRatio, 0.2f * aspectRatio, 0.2f * aspectRatio, 1.0f };

	DirectX::XMVECTOR measurement = g_d3dData->d3dSpriteFont->MeasureString("Arcade Mode");

	if (gamestate == GAME_STATE::ARCADE_GAME)
	{
		RECT rect;
		rect.top = 0;
		rect.bottom = 0.1f * (float)g_d3dData->windowWidthHeight.y;
		rect.left = 0.0f;
		rect.right = (float)g_d3dData->windowWidthHeight.x;
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::HAY_TEX], rect, nullptr, { 0.75f, 0.75f, 0.75f, 0.75f });
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Arcade Mode", DirectX::XMVECTOR{ (0.5f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.01f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		if (parentGame->playerdisconnect)
		{
			rect.top = 0.13f * (float)g_d3dData->windowWidthHeight.y;
			rect.bottom = 0.53f * (float)g_d3dData->windowWidthHeight.y;
			rect.left = 0.25f * (float)g_d3dData->windowWidthHeight.x;
			rect.right = 0.75f * (float)g_d3dData->windowWidthHeight.x;
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::DC1 + parentGame->PlayerDiscIndex], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		}

	}

	if (gamestate == GAME_STATE::ARCADE_MENU)
	{
		wchar_t numP[10];
		wchar_t numA[10];
		swprintf_s(numP, L"%d", parentGame->numPLAYERS);
		swprintf_s(numA, L"%d", parentGame->numAI);
		if (parentGame->mapsize == 1)
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "SMALL", DirectX::XMVECTOR{ (0.51f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.35f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		else if (parentGame->mapsize == 3)
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "LARGE", DirectX::XMVECTOR{ (0.51f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.35f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		else
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "MEDIUM", DirectX::XMVECTOR{ (0.51f*(float)g_d3dData->windowWidthHeight.x) - ((measurement.m128_f32[0] * scale.m128_f32[0])*0.5f), 0.35f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, numA, DirectX::XMVECTOR{ (0.47f*(float)g_d3dData->windowWidthHeight.x), 0.63f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale2);
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, numP, DirectX::XMVECTOR{ (0.47f*(float)g_d3dData->windowWidthHeight.x), 0.48f * (float)g_d3dData->windowWidthHeight.y }, DirectX::Colors::Black, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale2);
	}
	RECT rect;

	int bombtype;
	float x = (0.5f - (0.6f * ((float)g_d3dData->windowWidthHeight.y / (float)g_d3dData->windowWidthHeight.x)));
	rect.top = 0.075f * (float)g_d3dData->windowWidthHeight.y;
	rect.bottom = 0.13f * (float)g_d3dData->windowWidthHeight.y;
	rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
	rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
	if (parentGame->GetPlayer(0))
	{
		bombtype = parentGame->GetPlayer(0)->GetBombType();
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, {1.0f, 1.0f, 1.0f, 1.0f});
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 1:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x , (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::Red, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.15f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(0)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(0)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(1))
	{
		x += 0.14f *invAspect;
		rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
		rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
		bombtype = parentGame->GetPlayer(1)->GetBombType();
		g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 2:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::Blue, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(1)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(1)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
	}
	if (parentGame->GetPlayer(2) || parentGame->GetAI(1))
	{
		x += 0.14*invAspect;
		rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
		rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 3:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::FXMVECTOR{ 0.059f, 0.231f, 0.047f, 1.0f }, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		if (parentGame->GetPlayer(2) && parentGame->GetAI(1))
		{
			bombtype = parentGame->GetPlayer(2)->GetBombType();
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(2)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(2)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

			x += 0.14f * invAspect;
			rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
			rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
			x += 0.16f *invAspect;
			bombtype = parentGame->GetAI(1)->GetBombType();
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(1)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetAI(1)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		}
		else if (parentGame->GetPlayer(2))
		{
			bombtype = parentGame->GetPlayer(2)->GetBombType();
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, (parentGame->GetPlayer(2)->isAlive()) ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(2)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
		else if (parentGame->GetAI(1))
		{
			bombtype = parentGame->GetAI(1)->GetBombType();
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(1)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetAI(1)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
	}
	if (parentGame->GetPlayer(3) || parentGame->GetAI(0))
	{
		x += 0.14f * invAspect;
		rect.left = (x * (float)g_d3dData->windowWidthHeight.x) - 20;
		rect.right = (x * (float)g_d3dData->windowWidthHeight.x) + 40;
		g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, "Player 4:", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, DirectX::Colors::DarkMagenta, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);

		x += 0.16f *invAspect;
		if (parentGame->GetPlayer(3))
		{
			bombtype = parentGame->GetPlayer(3)->GetBombType();
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetPlayer(3)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetPlayer(3)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}
		else if (parentGame->GetAI(0))
		{
			bombtype = parentGame->GetAI(0)->GetBombType();
			g_d3dData->d3dSpriteBatch->Draw(g_d3dData->d3dDiffuseTextures[DIFFUSE_TEXTURES::BD + bombtype], rect, nullptr, { 1.0f, 1.0f, 1.0f, 1.0f });
			g_d3dData->d3dSpriteFont->DrawString(g_d3dData->d3dSpriteBatch, parentGame->GetAI(0)->isAlive() ? "Alive" : "Dead", DirectX::XMVECTOR{ x* (float)g_d3dData->windowWidthHeight.x, (float)g_d3dData->windowWidthHeight.y * 0.05f }, (parentGame->GetAI(0)->isAlive()) ? DirectX::Colors::DarkGreen : DirectX::Colors::DarkRed, 0.0f, DirectX::XMVECTOR{ 0.0f, 0.0f }, scale);
		}

	}
	UINT strides[] = { sizeof(TLineVertex) };
	UINT offsets[] = { 0 };

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
}
