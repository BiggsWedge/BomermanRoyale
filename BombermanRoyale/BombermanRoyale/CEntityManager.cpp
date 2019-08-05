/****************************************************
*	Filename:			CEntityManager.cpp
*	Mod. Date:			07/27/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contains the
*						implementation for the
*						CEntityManger class and
*						all of its internals and
*						useful classes and structures
****************************************************/

#include "CEntityManager.h"

CObject* CEntityManager::CreateOBJFromTemplate(OBJLoadInfo loadInfo)
{
	CObject* temp = new CObject();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID), loadInfo.collider, loadInfo.hasCollider);
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	return temp;
}


CObject* CEntityManager::DropBomb(CObject* playerSource)
{
	CObject* bomb;
	TComponent* transform;
	TTransformComponent* cTransform;
	playerSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;
	int x = 0;
	int z = 0;
	if (pos.x >= 0)
	{
		 x = (pos.x + 1.25f) / 2.5f;
		pos.x = ((float)x * 2.5f);
	}
	else
	{
		x = (pos.x - 1.25f) / 2.5f;
		pos.x = ((float)x * 2.5f);
	}
	if (pos.z >= 0)
	{
		z = (pos.z + 1.25f) / 2.5f;
		pos.z = ((float)z * 2.5f);
	}
	else
	{
		z = (pos.z - 1.25f) / 2.5f;
		pos.z = ((float)z * 2.5f);
	}
	//pos.x = floorf(pos.x) + (2.5f * (pos.x / abs(pos.x)));
	//pos.z = floorf(pos.z) + (2.5f * (pos.z / abs(pos.z)));

	loadInfo.position = pos;
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BLACK_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 75.0f, 1.0f / 75.0f, 1.0f / 75.0f);
	bomb = CreateOBJFromTemplate(loadInfo);
	return bomb;
}


CObject* CEntityManager::BombExplosionX(CObject* BombSource)
{
	CObject* explosion;
	TComponent* transform;
	TTransformComponent* cTransform;
	BombSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;

	loadInfo.position = pos;
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::FIRE_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 15.0f, 1.0f / 75.0f, 1.0f / 75.0f);
	explosion = CreateOBJFromTemplate(loadInfo);

	return explosion;
}

CObject* CEntityManager::BombExplosionZ(CObject* BombSource)
{
	CObject* explosion;
	TComponent* transform;
	TTransformComponent* cTransform;
	BombSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;

	loadInfo.position = pos;
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::FIRE_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 75.0f, 1.0f / 75.0f, 1.0f / 15.0f);
	explosion = CreateOBJFromTemplate(loadInfo);

	return explosion;
}