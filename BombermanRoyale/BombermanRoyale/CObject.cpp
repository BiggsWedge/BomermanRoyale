#pragma once

#include "CObject.h"




CObject::CObject()
{
	g_d3dData->d3dDevice->CreateDeferredContext(0, &d3dDeferredContext);
}

ID3D11CommandList * CObject::Draw()
{

	ID3D11CommandList* toRet = nullptr;
	TRendererComponent* renderer = nullptr;
	TMeshComponent* mesh = nullptr;
	TTransformComponent* transform = nullptr;
	TTextureComponent* tex = nullptr;
	for (auto& c : v_tComponents)
	{
		switch ( c->GetComponentType())
		{
		case COMPONENT_TYPE::TRANSFORM:
		{
			transform = (TTransformComponent*)&c;
		}
		case COMPONENT_TYPE::MESH:
		{
			mesh = (TMeshComponent*)&c;
			break;
		}
		case COMPONENT_TYPE::RENDERER:
		{
			renderer = (TRendererComponent*)&c;
			break;
		}
		case COMPONENT_TYPE::TEXTURE:
		{
			tex = (TTextureComponent*)&c;
			break;
		}
		default:
			break;
		}
	}
	const UINT strides[] = { sizeof(TSimpleVertex) };

	d3dDeferredContext->IASetInputLayout(*renderer->p_d3dUsedInputLayout);
	d3dDeferredContext->IASetVertexBuffers(0, 1, &mesh->d3dVertexBuffer, 0, strides);
	d3dDeferredContext->IASetIndexBuffer(mesh->d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	d3dDeferredContext->VSSetShader(*renderer->p_d3dUsedVertexShader, 0, 0);
	d3dDeferredContext->PSSetShader(*renderer->p_d3dUsedPixelShader, 0, 0);
	if (renderer->p_d3dUsedGeometryShader)
		d3dDeferredContext->GSSetShader(*renderer->p_d3dUsedGeometryShader, 0, 0);

	d3dDeferredContext->PSSetShaderResources(0, 1, tex->p_d3dUsedDiffuse);

	d3dDeferredContext->PSSetSamplers(0, 1, &g_d3dData->d3dSamplerState);

	g_d3dData->basicConstBuff.mModelMatrix = transform->mObjMatrix;

	TBasicPixelConstBuff pConst;
	pConst.flags[0] = true;
	pConst.flags[1] = false;
	pConst.flags[2] = false;
	pConst.flags[3] = false;

	d3dDeferredContext->UpdateSubresource(g_d3dData->d3dConstantBuffers[CONSTANT_BUFFER::V_BASIC], 0, nullptr, &g_d3dData->basicConstBuff, 0, 0);
	d3dDeferredContext->UpdateSubresource(g_d3dData->d3dConstantBuffers[CONSTANT_BUFFER::P_BASIC], 0, nullptr, &pConst, 0, 0);

	d3dDeferredContext->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstantBuffers[CONSTANT_BUFFER::P_BASIC]);
	d3dDeferredContext->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstantBuffers[CONSTANT_BUFFER::V_BASIC]);

	d3dDeferredContext->DrawIndexed(mesh->indexCount, 0, 0);

	d3dDeferredContext->FinishCommandList(false, &toRet);
	return toRet;
}
