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

	float fractionalTime = timepassed - (int)timepassed;

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
	else
	{

		if (currKeyFrame == anim->_anim.frames.size())
			currKeyFrame = 0;

		while (animTime > anim->_anim.frames[currFrameIndex + 1].time)
		{
			currFrameIndex++;
			if (currFrameIndex == anim->_anim.frames.size() - 1)
			{
				animTime -= anim->_anim.duration;
				currFrameIndex = 0;
			}
		}

		float startTime, endTime;
		startTime = anim->_anim.frames[currFrameIndex].time;
		endTime = anim->_anim.frames[currFrameIndex + 1].time;

		float ratio = (animTime - startTime) / (endTime - startTime);

		KeyFrame _key = anim->_anim.frames[currFrameIndex];

		jointCB _jointsConst;
		_jointsConst.numJoints = anim->_anim.frames[currFrameIndex].joints.size();

		for (int i = 0; i < anim->_anim.frames[currFrameIndex].joints.size(); ++i)
		{

			DirectX::XMMATRIX tween = matLerp(anim->_anim.frames[currFrameIndex].joints[i]._mat, anim->_anim.frames[currFrameIndex + 1].joints[i]._mat, ratio);

			//debug_renderer::drawMatrix(tween);

			_jointsConst._joints[i] = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, anim->_bindPose[i]._mat) * tween);

			if (_key.joints[i].parentIndex >= 0)
			{
				DirectX::XMFLOAT4X4 startMat, endMat;

				DirectX::XMStoreFloat4x4(&startMat, matLerp(anim->_anim.frames[currFrameIndex].joints[i]._mat, anim->_anim.frames[currFrameIndex + 1].joints[i]._mat, ratio));
				DirectX::XMStoreFloat4x4(&endMat, matLerp(anim->_anim.frames[currFrameIndex].joints[anim->_anim.frames[currFrameIndex].joints[i].parentIndex]._mat, anim->_anim.frames[currFrameIndex + 1].joints[anim->_anim.frames[currFrameIndex + 1].joints[i].parentIndex]._mat, ratio));

				float3 start = { startMat._41, startMat._42, startMat._43 };
				float3 end = { endMat._41, endMat._42, endMat._43 };

				//debug_renderer::add_line(start, end, float4(1.0f, 1.0f, 1.0f, 1.0f));
			}


		}
		if (mat)
		{
			for (int i = 0; i < mat->mats.size(); ++i)
			{
				for (int j = 0; j < material_t::COMPONENT::COUNT; ++j)
				{
					if (mat->mats[i][j].input < 0)
						continue;
					std::experimental::filesystem::path filePath;
					filePath = mat->filepaths[mat->mats[i][j].input].data();
					HRESULT result = DirectX::CreateWICTextureFromFile(g_d3dData->d3dDevice, filePath.wstring().c_str(), nullptr, &mat->_srv[j]);
					if (!SUCCEEDED(result))
					{
						std::string fail = "Failed to make texture from anim data! ";
						g_pLogger->LogCatergorized("FAILURE", fail.c_str());
					}
				}
			}

			if (mat->_samState)
			{
				D3D11_SAMPLER_DESC sampDesc = {};
				sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sampDesc.MinLOD = 0;
				sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

				g_d3dData->d3dDevice->CreateSamplerState(&sampDesc, &mat->_samState);
			}
		}
		UINT strides = sizeof(TSimpleVertex);
		UINT offsets = 0;

		MVP_t debugConstBuff;
		debugConstBuff.world = DirectX::XMMatrixTranspose(transform->mObjMatrix);

		if (g_d3dData->bUseDebugRenderCamera)
			debugConstBuff.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
		else
			debugConstBuff.view = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

		debugConstBuff.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);


		//debugConstBuff.world = DirectX::XMMatrixTranspose(transform->mObjMatrix);
		//debugConstBuff.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);
		//debugConstBuff.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, g_d3dData->viewMat));

		/*debugConstBuff.world = DirectX::XMMatrixIdentity();
		debugConstBuff.world = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180));
		debugConstBuff.world = debugConstBuff.world * DirectX::XMMatrixTranslation(6.0f, 0.0f, 0.0f);
		debugConstBuff.world = DirectX::XMMatrixTranspose(debugConstBuff.world);*/

#pragma endregion

		Light pointLight;
		pointLight.position = DirectX::XMFLOAT3(0.0, 6.0f, -2.0f);
		pointLight.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		pointLight.intensity = 0.25f;
		pointLight.type = 0;

		LightBuffer _light;
		_light.light = pointLight;

		//MatBuffer mat;
		//mat.material = _mat;

		//jointCB _jointsConst;
		//_jointsConst.numJoints = anim->_bindPose.size();
		//for (int i = 0; i < anim->_bindPose.size(); ++i)
		//{
		//	_jointsConst._joints[i] = anim->_bindPose[i]._mat;
		//}

		g_d3dData->d3dContext->RSSetState(g_d3dData->d3dRasterizerState2);

		g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MVP_t], 0, nullptr, &debugConstBuff, 0, 0);

		g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[INPUT_LAYOUT::BASIC]);

		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::LIGHTS], 0, nullptr, &_light, 0, 0);
		g_d3dData->d3dContext->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::LIGHTS]);

		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MATERIAL], 0, nullptr, &mat->_mat, 0, 0);
		g_d3dData->d3dContext->PSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MATERIAL]);

		g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MVP_t]);
		g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS], 0, nullptr, &_jointsConst, 0, 0);
		g_d3dData->d3dContext->VSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS]);

		g_d3dData->d3dContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, &strides, &offsets);

		g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[VERTEX_SHADER::ANIM], 0, 0);
		g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[PIXEL_SHADER::ANIM], 0, 0);
		g_d3dData->d3dContext->PSSetSamplers(0, 1, &mat->_samState);
		g_d3dData->d3dContext->PSSetShaderResources(0, 1, &mat->_srv[mat->DIFFUSE]);
		g_d3dData->d3dContext->PSSetShaderResources(1, 1, &mat->_srv[mat->EMISSIVE]);
		g_d3dData->d3dContext->PSSetShaderResources(2, 1, &mat->_srv[mat->SPECULAR]);

		g_d3dData->d3dContext->DrawIndexed(mesh->indexCount, 0, 0);
	}


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
	DirectX::XMFLOAT3 targetVec = { _x, 0.0f, _z };
	transform->mObjMatrix = TurnTo(transform->mObjMatrix, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&transform->fPosition), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&targetVec))), 0.5f);

	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, transform->mObjMatrix.r[3]);
	DirectX::XMStoreFloat3(&transform->fForwardVector, DirectX::XMMatrixTranspose(transform->mObjMatrix).r[2]);
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

void CObject::Cleanup()
{
	for (int i = 0; i < v_tComponents.size(); i++)
		delete v_tComponents[i];
	v_tComponents.clear();
}







