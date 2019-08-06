#pragma once

#include "CObject.h"

constexpr size_t MAX_LINE_VERTS = 4096;

size_t line_vert_count = 0;
std::array< TLineVertex, MAX_LINE_VERTS> line_verts;


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
	if(componentType == 0)
		TRendererComponent* renderer;
	if (componentType == 1)
		TMeshComponent* renderer;
	if (componentType == 2)
		TTransformComponent* renderer;
	if (componentType == 3)
		TTextureComponent* renderer;

	
	for (TComponent* c : v_tComponents)
	{
		if (c->GetComponentType() == componentType)
		{
			if (componentType == 0)
				component = (TRendererComponent*)c;
			else if (componentType == 1)
				component = (TMeshComponent*)c;
			else if (componentType == 2)
				component = (TTransformComponent*)c;
			else
				component = (TTextureComponent*)c;
			//component = (TRendererComponent*)c;
			//component = new TRendererComponent(component->iUsedVertexShaderIndex, component->iUsedPixelShaderIndex, component->iUsedInputLayout, component->iUsedGeometryShaderIndex, component->iUsedLoadState);
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

	transform->mObjMatrix = transform->mObjMatrix * DirectX::XMMatrixTranslation(_x, 0, _z);
	
	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, transform->mObjMatrix.r[3]);
	transform->fPosition = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);

	return true;
}


void CObject::add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b) {
	line_verts[line_vert_count].pos = point_a;
	line_verts[line_vert_count].color = color_a;
	line_vert_count += 1;

	line_verts[line_vert_count].pos = point_b;
	line_verts[line_vert_count].color = color_b;
	line_vert_count += 1;
}

void CObject::clear_lines() {
	line_vert_count = 0;
}

const TLineVertex* CObject::get_line_verts() {
	return line_verts.data();
}

size_t CObject::get_line_vert_count() {
	return line_vert_count;
}

size_t CObject::get_line_vert_capacity() {
	return MAX_LINE_VERTS;
}

void CObject::drawAABB(float3 point_a, float3 point_b, float3 point_c, float3 point_d, float3 point_e, float3 point_f, float3 point_g, float3 point_h, float4 color1, float4 color2) {
	add_line(point_a, point_b, color1, color2);
	add_line(point_b, point_c, color1, color2);
	add_line(point_c, point_d, color1, color2);
	add_line(point_d, point_a, color1, color2);
	add_line(point_e, point_f, color1, color2);
	add_line(point_f, point_g, color1, color2);
	add_line(point_g, point_h, color1, color2);
	add_line(point_h, point_e, color1, color2);
	add_line(point_a, point_e, color1, color2);
	add_line(point_b, point_f, color1, color2);
	add_line(point_c, point_g, color1, color2);
	add_line(point_d, point_h, color1, color2);
}

float3 CObject::XMVector2Float3(DirectX::XMVECTOR vector) {
	float3 point;

	point.x = DirectX::XMVectorGetX(vector);
	point.y = DirectX::XMVectorGetY(vector);
	point.z = DirectX::XMVectorGetZ(vector);

	return point;
}

DirectX::XMVECTOR CObject::Float32XMVector(float3 point) {
	DirectX::XMVECTOR vector = { point.x, point.y, point.z, 1.0f };

	return vector;
}

void CObject::LoadLines(/*std::vector<CObject*> Objects*/) {
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	//TComponent* component;
	//TMeshComponent* tMeshComponent;
	//TTransformComponent* tTransformComponent;

	//TCollider debugCollider = TCollider();
	TCollider debugCollider = GetCenter(v_tMeshTemplates[0]);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	//for (int i = 0; i < Objects->size(); ++i)
	//{
	//
	//	Objects.at(i).GetComponent(COMPONENT_TYPE::TRANSFORM, component);
	//	tTransformComponent = (TTransformComponent*)component;
	//	Objects.at(i).GetComponent(COMPONENT_TYPE::MESH, component);
	//	Objects.at
	//		tMeshComponent = (TMeshComponent*)component;
	//	if (tMeshComponent->hasCollider)
	//	{
	//		for (size_t i = 0; i < 8; i++)
	//		{
	//			debugCollider = tMeshComponent->mCollider;
	//			debugCollider.corners[i].x = (debugCollider.corners[i].x * scale.x) - tTransformComponent->fPosition.x; //*25.0f;
	//			debugCollider.corners[i].y = (debugCollider.corners[i].y * scale.y) - tTransformComponent->fPosition.y; //*25.0f;
	//			debugCollider.corners[i].z = (debugCollider.corners[i].z * scale.z) - tTransformComponent->fPosition.z; //*25.0f;
	//		}
	//	}
	//	drawAABB(debugCollider.corners[2], debugCollider.corners[0], debugCollider.corners[4], debugCollider.corners[6], debugCollider.corners[5], debugCollider.corners[3], debugCollider.corners[7], debugCollider.corners[1], { 1,0,0,1 }, { 1,0,0,1 });
	//}
	for (size_t i = 0; i < 8; i++) {
		debugCollider.corners[i].x = (debugCollider.corners[i].x * scale.x) - position.x; //*25.0f;
		debugCollider.corners[i].y = (debugCollider.corners[i].y * scale.y) - position.y; //*25.0f;
		debugCollider.corners[i].z = (debugCollider.corners[i].z * scale.z) - position.z; //*25.0f;
	}
	
	drawAABB(debugCollider.corners[2], debugCollider.corners[0], debugCollider.corners[4], debugCollider.corners[6], debugCollider.corners[5], debugCollider.corners[3], debugCollider.corners[7], debugCollider.corners[1], { 1,0,0,1 }, { 1,0,0,1 });
}

TCollider CObject::GetCenter(TMeshTemplate _verts) {
	TCollider collider = TCollider();
	float3 center;
	float minX, maxX, minY, maxY, minZ, maxZ;

	minX = maxX = _verts.v_tVertices.at(0).fPosition.x;
	minY = maxY = _verts.v_tVertices.at(0).fPosition.y;
	minZ = maxZ = _verts.v_tVertices.at(0).fPosition.z;

	for (int i = 0; i < _verts.v_tVertices.size(); i++) {
		if (_verts.v_tVertices.at(i).fPosition.x < minX) {
			minX = _verts.v_tVertices.at(i).fPosition.x;
		}
		else if (_verts.v_tVertices.at(i).fPosition.x > maxX) {
			maxX = _verts.v_tVertices.at(i).fPosition.x;
		}
		else if (_verts.v_tVertices.at(i).fPosition.y < minY) {
			minY = _verts.v_tVertices.at(i).fPosition.y;
		}
		else if (_verts.v_tVertices.at(i).fPosition.y > maxY) {
			maxY = _verts.v_tVertices.at(i).fPosition.y;
		}
		else if (_verts.v_tVertices.at(i).fPosition.z < minZ) {
			minZ = _verts.v_tVertices.at(i).fPosition.z;
		}
		else if (_verts.v_tVertices.at(i).fPosition.z > maxZ) {
			maxZ = _verts.v_tVertices.at(i).fPosition.z;
		}
	}

	center.x = (minX + maxX) * 0.5f;
	center.y = (minY + maxY) * 0.5f;
	center.z = (minZ + maxZ) * 0.5f;

	collider.center = center;
	collider.extents = GetExtents(minX, maxX, minY, maxY, minZ, maxZ);

	GetCorners(collider.center, collider.extents, collider.corners);

	return collider;
}

DirectX::XMFLOAT3 CObject::GetExtents(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ) {
	float3 extents;

	extents.x = (_maxX - _minX) * 0.5f;
	extents.y = (_maxY - _minY) * 0.5f;
	extents.z = (_maxZ - _minZ) * 0.5f;

	return extents;
}

void CObject::GetCorners(float3 _center, float3 _extents, float3*& corners) {
	float newX, newY, newZ;

	newX = _center.x - _extents.x;
	newY = _center.y - _extents.y;
	newZ = _center.z - _extents.z;

	corners[0] = { newX,newY,newZ };

	newX = _center.x + _extents.x;
	newY = _center.y + _extents.y;
	newZ = _center.z + _extents.z;

	corners[1] = { newX,newY,newZ };

	corners[2] = { corners[0].x,corners[0].y,corners[1].z };
	corners[3] = { corners[0].x,corners[1].y,corners[0].z };
	corners[4] = { corners[1].x,corners[0].y,corners[0].z };
	corners[5] = { corners[0].x,corners[1].y,corners[1].z };
	corners[6] = { corners[1].x,corners[0].y,corners[1].z };
	corners[7] = { corners[1].x,corners[1].y,corners[0].z };
}




