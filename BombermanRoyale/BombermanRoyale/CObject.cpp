#pragma once

#include "CObject.h"




CObject::CObject()
{
}

void CObject::Draw()
{
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

	g_d3dData->d3dContext->IASetInputLayout(g_d3dData->d3dInputLayout[renderer->iUsedInputLayout]);
	g_d3dData->d3dContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, strides, offsets);
	g_d3dData->d3dContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_d3dData->d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dData->d3dContext->VSSetShader(g_d3dData->d3dVertexShader[renderer->iUsedVertexShaderIndex], 0, 0);

	g_d3dData->d3dContext->PSSetShader(g_d3dData->d3dPixelShader[renderer->iUsedPixelShaderIndex], 0, 0);
	if (renderer->iUsedGeometryShaderIndex >= 0)
		g_d3dData->d3dContext->GSSetShader(g_d3dData->d3dGeometryShader[renderer->iUsedGeometryShaderIndex], 0, 0);

	TBasicPixelConstBuff pConst;
	for (int i = 0; i < 4; ++i)
		pConst.flags[i] = -1;

	if (tex->iUsedDiffuseIndex >= 0)
	{
		g_d3dData->d3dContext->PSSetShaderResources(0, 1, &g_d3dData->d3dDiffuseTextures[tex->iUsedDiffuseIndex]);
		pConst.flags[0] = 1;
	}

	if (tex->iUsedNormalIndex >= 0)
	{
		pConst.flags[1] = 1;
	}


	g_d3dData->d3dContext->PSSetSamplers(0, 1, &g_d3dData->d3dSamplerState);

	g_d3dData->basicConstBuff.mModelMatrix = DirectX::XMMatrixTranspose(transform->mObjMatrix);
	g_d3dData->basicConstBuff.mViewMatrix = DirectX::XMMatrixTranspose(g_d3dData->viewMat);
	g_d3dData->basicConstBuff.mProjMatrix = DirectX::XMMatrixTranspose(g_d3dData->projMat);

	g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_BASIC], 0, nullptr, &g_d3dData->basicConstBuff, 0, 0);
	g_d3dData->d3dContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::V_BASIC]);

	g_d3dData->d3dContext->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::P_BASIC], 0, nullptr, &pConst, 0, 0);
	g_d3dData->d3dContext->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::P_BASIC]);

	g_d3dData->d3dContext->DrawIndexed(mesh->indexCount, 0, 0);


}
