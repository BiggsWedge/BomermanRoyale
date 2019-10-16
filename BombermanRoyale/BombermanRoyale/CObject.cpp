#pragma once

#include "CObject.h"
#include <iostream>


CObject::CObject()
{
}

CObject::~CObject()
{
	for (int i = 0; i < v_tComponents.size(); ++i)
		delete v_tComponents[i];

	v_tComponents.clear();
}

void CObject::Draw(double timepassed)
{
	animTime += timepassed;
	totalTime += timepassed;
	explosiontime += timepassed;


	float fractionalTime = timepassed - (int)timepassed;

	ID3D11CommandList* d3dCommandList = nullptr;
	TRendererComponent* renderer = nullptr;
	TMeshComponent* mesh = nullptr;
	TTransformComponent* transform = nullptr;
	TTextureComponent* tex = nullptr;
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

	g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState[RASTERIZER_STATE::DEFAULT]);

	g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, strides, offsets);
	g_d3dData->d3dContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[renderer->iUsedVertexShaderIndex], 0, 0);

	g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[renderer->iUsedPixelShaderIndex], 0, 0);
	if (renderer->iUsedGeometryShaderIndex >= 0)
		g_d3dData->d3dContext->GSSetShader(g_d3dData->d3dGeometryShader[renderer->iUsedGeometryShaderIndex], 0, 0);
	else
		g_d3dData->d3dContext->GSSetShader(nullptr, 0, 0);
	bombconstbuffer bombconst;
	if (renderer->iUsedGeometryShaderIndex == GEOMETRY_SHADER::MESH_EXPLOSION)
	{
	/*	bombconst.world = DirectX::XMMatrixTranspose(transform->mObjMatrix);
		bombconst.world *= DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&transform->fScale));*/

		/*if (g_d3dData->bUseDebugRenderCamera)
			bombconst.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
		else
			bombconst.view = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

		bombconst.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);*/
		bombconst.time = totalTime * 40.0f;
	/*	DirectX::XMVECTOR scale = DirectX::XMLoadFloat3(&transform->fScale);
		DirectX::XMStoreFloat3(&bombconst.padding, scale);*/
		


		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST], 0, nullptr, &bombconst, 0, 0);
		g_d3dData->d3dContext->GSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST]);
	}
	/*g_d3dData->d3dContext->OMSetBlendState();*/

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

	if (anim)
	{
		AnimationClip currentClip = *anim->currentAnimation;

		double startTime, endTime;
		startTime = currentClip.frames[anim->currentFrameIndex].time;
		endTime = currentClip.frames[anim->currentFrameIndex + 1].time;

		double ratio = (anim->_time - startTime) / (endTime - startTime);

		KeyFrame _key = currentClip.frames[anim->currentFrameIndex];
		KeyFrame _nextKey = currentClip.frames[anim->currentFrameIndex + 1];

		jointCB jcb;

		jcb.numJoints = _key.joints.size();

		for (int i = 0; i < jcb.numJoints; ++i)
		{
			DirectX::XMMATRIX tween;
			tween = matLerp(DirectX::XMLoadFloat4x4(&_key.joints[i]._mat), DirectX::XMLoadFloat4x4(&_nextKey.joints[i]._mat), ratio);

			jcb._joints[i] = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(&currentClip._bindPose[i]._mat))*tween);
		}

		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS], 0, nullptr, &jcb, 0, 0);
	}
	
	if (renderer->iUsedVertexShaderIndex != VERTEX_SHADER::EXPLOSION)
	{
		if (g_d3dData->bUseDebugRenderCamera)
			g_d3dData->basicConstBuff.mViewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
		else
			g_d3dData->basicConstBuff.mViewMatrix = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

		g_d3dData->basicConstBuff.mProjMatrix = DirectX::XMMatrixTranspose(g_d3dData->projMat);

		g_d3dData->basicConstBuff.mModelMatrix *= DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&transform->fScale));
		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_BASIC], 0, nullptr, &g_d3dData->basicConstBuff, 0, 0);
		g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_BASIC]);
		g_d3dData->d3dContext->VSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS]);
	}
	else
	{

		bombconst.world = DirectX::XMMatrixTranspose(transform->mObjMatrix);
		bombconst.world *= DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&transform->fScale));

		if (g_d3dData->bUseDebugRenderCamera)
			bombconst.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
		else
			bombconst.view = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

		bombconst.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);
		bombconst.time = totalTime * 10.0f;
		DirectX::XMVECTOR scale = DirectX::XMLoadFloat3(&transform->fScale);
		DirectX::XMStoreFloat3(&bombconst.padding, scale);


		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST], 0, nullptr, &bombconst, 0, 0);
		g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::BOMBCONST]);
	}
	

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
}

void CObject::TurnPlayerTo(float _x, float _z) {
	TComponent* cTransform;
	TTransformComponent* transform;
	GetComponent(COMPONENT_TYPE::TRANSFORM, cTransform);
	transform = (TTransformComponent*)cTransform;
	DirectX::XMFLOAT3 targetVec = { _x, 0.0f, _z };
	DirectX::XMFLOAT3 newTarget;
	DirectX::XMVECTOR up = { 0, 1, 0 };
	DirectX::XMStoreFloat3(&newTarget, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&targetVec)));

	DirectX::XMVECTOR z = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&newTarget), DirectX::XMLoadFloat3(&transform->fPosition));
	z = DirectX::XMVector3Normalize(z);
	DirectX::XMVECTOR x = DirectX::XMVector3Cross(up, z);
	x = DirectX::XMVector3Normalize(x);
	DirectX::XMVECTOR y = DirectX::XMVector3Cross(z, x);
	y = DirectX::XMVector3Normalize(y);
	DirectX::XMMATRIX result = DirectX::XMMATRIX(x, y, z, DirectX::XMLoadFloat3(&transform->fPosition));

	//transform->fForwardVector = newTarget;
	transform->mObjMatrix = result;
}

bool CObject::Move(float _x, float _z, bool rotation)
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

	DirectX::XMFLOAT3 targetVec = { _x, 0.0f, _z };
	if (rotation)
	{
		transform->mObjMatrix = TurnTo(transform->mObjMatrix, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform->fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&targetVec))), 0.5f);
	}
	transform->mObjMatrix = transform->mObjMatrix * DirectX::XMMatrixTranslation(_x, 0, _z);

	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, transform->mObjMatrix.r[3]);
	DirectX::XMStoreFloat3(&transform->fForwardVector, transform->mObjMatrix.r[2]);
	transform->fPosition = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);

	return true;
}

bool CObject::CrouchRoll(float _x, float _z, float _y, bool rotation)
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
	collider->d3dCollider.Center.y += _y;

	transform->mObjMatrix = transform->mObjMatrix * DirectX::XMMatrixTranslation(_x, _y, _z);
	DirectX::XMFLOAT3 targetVec = { _x, _y, _z };
	if (rotation)
	{
		transform->mObjMatrix = TurnTo(transform->mObjMatrix, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform->fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&targetVec))), 0.5f);
	}

	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, transform->mObjMatrix.r[3]);
	DirectX::XMStoreFloat3(&transform->fForwardVector, transform->mObjMatrix.r[2]);
	transform->fPosition = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);

	return true;
}

bool CObject::MoveOverTime(float start_x, float end_x, float start_z , float end_z, float timepassed, bool rotation)
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

	movetimer += timepassed;
	float ratio =  movetimer / 3.5f;
	float _x = lerp(start_x, end_x, ratio);
	_x = _x - start_x;
	float _z = lerp(start_z, end_z, ratio);
	_z = _z - start_z;
	collider->d3dCollider.Center.x += _x;
	collider->d3dCollider.Center.z += _z;
	if (ratio >= 1.0f)
		movetimer = 0.0f;


	transform->mObjMatrix = transform->mObjMatrix * DirectX::XMMatrixTranslation(_x, 0, _z);
	DirectX::XMFLOAT3 targetVec = { _x, 0, _z };
	if (rotation)
	{
		transform->mObjMatrix = TurnTo(transform->mObjMatrix, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform->fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&targetVec))), 0.5f);
	}

	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, transform->mObjMatrix.r[3]);
	DirectX::XMStoreFloat3(&transform->fForwardVector, DirectX::XMVector3Normalize(transform->mObjMatrix.r[2]));
	transform->fPosition = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);
	
	if (ratio >= 1.0f)
	{
		return true;
	}
	else
		return false;
	
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

void CObject::Cleanup()
{
	for (int i = 0; i < v_tComponents.size(); i++)
		delete v_tComponents[i];
	v_tComponents.clear();
}
