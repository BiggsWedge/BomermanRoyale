#include "CBomb.h"
#include "CPlayer.h"


CBomb::CBomb()
{
}


CBomb::~CBomb()
{
}

void CBomb::Explode()
{
	
	bool soundplaying;
	for (int i = 0; i < explosionSound.size(); ++i)
	{
		explosionSound.at(i)->isSoundPlaying(soundplaying);
		if (!soundplaying)
		{

			explosionSound.at(i)->Play();
			break;
		}
	}
	//DirectX::XMMATRIX viewPos = g_d3dData->screenShake();
	alive = false;

	//timer = maxTime - 0.01f;
}

void CBomb::initialize(CPlayer* parent)
{
	alive = true;
	timer = 0.0;
	maxTime = 2.5;
	p_cParentPlayer = parent;
}

void CBomb::Draw(double timepassed)
{

	ID3D11CommandList* d3dCommandList = nullptr;
	TRendererComponent* renderer = nullptr;
	TMeshComponent* mesh = nullptr;
	TTransformComponent* transform = nullptr;
	TTextureComponent* tex = nullptr;
	TMaterialComponent* mat = nullptr;
	TAnimComponent* anim = nullptr;

	for (TComponent* c : v_tComponents)
	{
		switch (c->GetComponentType())
		{
		case COMPONENT_TYPE::TRANSFORM:
		{
			transform = (TTransformComponent*)c;
		}
		case COMPONENT_TYPE::MESH:
		{
			mesh = (TMeshComponent*)c;
			break;
		}
		case COMPONENT_TYPE::RENDERER:
		{
			renderer = (TRendererComponent*)c;
			break;
		}
		case COMPONENT_TYPE::TEXTURE:
		{
			tex = (TTextureComponent*)c;
			break;
		}
		case COMPONENT_TYPE::MATERIAL:
		{
			mat = (TMaterialComponent*)c;
			break;
		}
		case COMPONENT_TYPE::ANIM:
		{
			anim = (TAnimComponent*)c;
			break;
		}
		default:
			break;
		}
	}

	const UINT offsets[] = { 0 };
	const UINT strides[] = { sizeof(TSimpleVertex) };

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };

	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);
	g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[renderer->iUsedInputLayout]);
	g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (mesh->mName != "BattleMage")
	{
		g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState);

		g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, strides, offsets);
		g_d3dData->d3dContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[renderer->iUsedVertexShaderIndex], 0, 0);

		g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[renderer->iUsedPixelShaderIndex], 0, 0);
		if (renderer->iUsedGeometryShaderIndex >= 0)
			g_d3dData->d3dContext->GSSetShader(g_d3dData->d3dGeometryShader[renderer->iUsedGeometryShaderIndex], 0, 0);

		if (tex->iUsedDiffuseIndex >= 0)
		{
			g_d3dData->d3dContext->PSSetShaderResources(0, 1, &g_d3dData->d3dDiffuseTextures[tex->iUsedDiffuseIndex]);
		}
		else
			g_d3dData->d3dContext->PSSetShaderResources(0, 0, nullptr);

		g_d3dData->d3dContext->PSSetSamplers(0, 1, &g_d3dData->d3dSamplerState);

		bombconstbuffer bombconst;
		bombconst.world = DirectX::XMMatrixTranspose(transform->mObjMatrix);

		bombpixelbuffer bombpconst;
		bombpconst.time = DirectX::XMFLOAT4(timer, 0.0f, 0.0f, 0.0f);

		if (g_d3dData->bUseDebugRenderCamera)
			bombconst.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
		else
			bombconst.view = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

		bombconst.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);
		bombconst.time = DirectX::XMFLOAT4(timer, 0.0f, 0.0f, 0.0f);

		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST], 0, nullptr, &bombconst, 0, 0);
		g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST]);

		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMB_P_CONST], 0, nullptr, &bombpconst, 0, 0);
		g_d3dData->d3dContext->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMB_P_CONST]);

		g_d3dData->d3dContext->DrawIndexed(mesh->indexCount, 0, 0);
	}
}

CBomb& CBomb::operator=(const CBomb& other)
{
	if (this != &other)
	{
		v_tComponents.clear();
		this->v_tComponents = other.v_tComponents;
		p_cParentPlayer = other.p_cParentPlayer;
		timer = other.timer;
		maxTime = other.maxTime;
		strength = other.strength;
		alive = other.alive;
	}
	return *this;
}
