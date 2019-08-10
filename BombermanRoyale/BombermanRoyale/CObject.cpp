#pragma once

#include "CObject.h"




CObject::CObject()
{
}

void CObject::Draw()
{
	ID3D11CommandList* d3dCommandList = nullptr;
	TRendererComponent* renderer = nullptr;
	TMeshComponent* mesh = nullptr;
	TTransformComponent* transform = nullptr;
	TTextureComponent* tex = nullptr;

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
		default:
			break;
		}
	}

	const UINT offsets[] = { 0 };
	const UINT strides[] = { sizeof(TSimpleVertex) };

	ID3D11RenderTargetView* const d3dTargets[] = { g_d3dData->d3dRenderTargetView };

	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState);
	g_d3dData->d3dContext->RSSetViewports(1, &g_d3dData->d3dViewport);
	g_d3dData->d3dContext->OMSetRenderTargets(1, d3dTargets, g_d3dData->d3dDepthStencilView);

	g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[renderer->iUsedInputLayout]);
	g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, strides, offsets);
	g_d3dData->d3dContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[renderer->iUsedVertexShaderIndex], 0, 0);

	g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[renderer->iUsedPixelShaderIndex], 0, 0);
	if (renderer->iUsedGeometryShaderIndex >= 0)
		g_d3dData->d3dContext->GSSetShader(g_d3dData->d3dGeometryShader[renderer->iUsedGeometryShaderIndex], 0, 0);

	TBasicPixelConstBuff pConst;
	for (int i = 0; i < 8; ++i)
		pConst.flags[i] = -1;

	if (tex->iUsedDiffuseIndex >= 0)
	{
		g_d3dData->d3dContext->PSSetShaderResources(0, 1, &g_d3dData->d3dDiffuseTextures[tex->iUsedDiffuseIndex]);
		pConst.flags[0] = 1;
	}
	else
		g_d3dData->d3dContext->PSSetShaderResources(0, 0, nullptr);

	if (tex->iUsedNormalIndex >= 0)
	{
		pConst.flags[1] = 1;
	}

	g_d3dData->d3dContext->PSSetSamplers(0, 1, &g_d3dData->d3dSamplerState);

	g_d3dData->basicConstBuff.mModelMatrix = DirectX::XMMatrixTranspose(transform->mObjMatrix);

	if (g_d3dData->bUseDebugRenderCamera)
		g_d3dData->basicConstBuff.mViewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
	else
		g_d3dData->basicConstBuff.mViewMatrix = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

	g_d3dData->basicConstBuff.mProjMatrix = DirectX::XMMatrixTranspose(g_d3dData->projMat);

	g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_BASIC], 0, nullptr, &g_d3dData->basicConstBuff, 0, 0);
	g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_BASIC]);

	g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::P_BASIC], 0, nullptr, &pConst, 0, 0);
	g_d3dData->d3dContext->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::P_BASIC]);

	g_d3dData->d3dContext->DrawIndexed(mesh->indexCount, 0, 0);


}

bool CObject::GetComponent(int componentType, TComponent* & component)
{
	for (TComponent* c : v_tComponents)
	{
		if (c->GetComponentType() == componentType)
		{
			component = c;
			return true;
		}
	}
	return false;


	/*
	if (componentType == COMPONENT_TYPE::RENDERER)
		TRendererComponent* renderer;
	if (componentType == COMPONENT_TYPE::MESH)
		TMeshComponent* renderer;
	if (componentType == COMPONENT_TYPE::TRANSFORM)
		TTransformComponent* transform;
	if (componentType == COMPONENT_TYPE::TEXTURE)
		TTextureComponent* texture;

	for (TComponent* c : v_tComponents)
	{
		if (c->GetComponentType() == componentType)
		{
			if (componentType == COMPONENT_TYPE::RENDERER)
				component = (TRendererComponent*)c;
			else if (componentType == COMPONENT_TYPE::MESH)
				component = (TMeshComponent*)c;
			else if (componentType == COMPONENT_TYPE::TRANSFORM)
				component = (TTransformComponent*)c;
			else if (componentType = COMPONENT_TYPE::TEXTURE)
				component = (TTextureComponent*)c;
			return true;
		}
	}
	return false;
	*/
}

bool CObject::Move(float _x, float _z)
{
	TComponent* cTransform;
	TTransformComponent* transform;
	if (!GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform))
		return false;
	transform = (TTransformComponent*)cTransform;
	TColliderComponent* collider;
	if (!GetComponent(COMPONENT_TYPE::COLLIDER, cTransform))
		return false;
	collider = (TColliderComponent*)cTransform;
	collider->d3dCollider.Center.x += _x;
	collider->d3dCollider.Center.z += _z;

	transform->mObjMatrix = transform->mObjMatrix * DirectX::XMMatrixTranslation(_x, 0, _z);

	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, transform->mObjMatrix.r[3]);
	transform->fPosition = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);

	return true;
}

bool CObject::Collides(CObject * _other)
{
	TComponent* temp = nullptr;
	if (!_other->GetComponent(COMPONENT_TYPE::COLLIDER, temp))
		return false;
	TColliderComponent* theyCollider = (TColliderComponent*)temp;
	temp = nullptr;
	if (!GetComponent(COMPONENT_TYPE::COLLIDER, temp))
		return false;
	TColliderComponent* thisCollider = (TColliderComponent*)temp;

	if (g_d3dData->collisionMatrix[thisCollider->collisionLayer][theyCollider->collisionLayer])
	{
		if (thisCollider->d3dCollider.Intersects(theyCollider->d3dCollider))
		{
			return true;
		}
	}
	return false;
}







