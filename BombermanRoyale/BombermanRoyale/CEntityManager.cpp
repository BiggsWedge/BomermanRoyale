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
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID), loadInfo.collider, loadInfo.hasCollider);
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TMaterialComponent* mat = new TMaterialComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mat);

	TAnimComponent* anim = new TAnimComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)anim);

	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	return temp;
}

CPlayer* CEntityManager::CreatePlayerFromTemplate(OBJLoadInfo loadInfo)
{
	CPlayer* temp = new CPlayer();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID), loadInfo.collider, loadInfo.hasCollider);
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TMaterialComponent* mat = new TMaterialComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mat);

	TAnimComponent* anim = new TAnimComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)anim);

	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	return temp;
}

CBomb * CEntityManager::CreateBombFromTemplate(OBJLoadInfo loadInfo)
{
	CBomb* temp = new CBomb();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID), loadInfo.collider, loadInfo.hasCollider);
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	return temp;
}

CBomb* CEntityManager::DropBomb(CPlayer* playerSource)
{
	CBomb* bomb;
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

	loadInfo.position = pos;
	loadInfo.forwardVec = { 1.0f, 0.0f, 0.0f };
	loadInfo.meshID = MODELS::BOMB;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::BOMB;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f);
	bomb = CreateBombFromTemplate(loadInfo);
	bomb->initialize(playerSource);

	return bomb;
}

CObject* CEntityManager::BombExplosionX(CBomb* BombSource)
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
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::FIRE_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::EXPLOSION;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 15.0f, 1.0f / 75.0f, 1.0f / 75.0f);
	explosion = CreateOBJFromTemplate(loadInfo);

	return explosion;
}

CObject* CEntityManager::BombExplosionZ(CBomb* BombSource)
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
	loadInfo.meshID = MODELS::CUBE;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::FIRE_TEX;
	loadInfo.collisionLayer = COLLISION_LAYERS::EXPLOSION;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 75.0f, 1.0f / 75.0f, 1.0f / 15.0f);
	explosion = CreateOBJFromTemplate(loadInfo);

	return explosion;
}

CPlayer* CEntityManager::InstantiatePlayer(int numPlayer, int playerModel, DirectX::XMFLOAT3 spawnPos)
{
	OBJLoadInfo pLoadInfo;
	pLoadInfo.meshID = MODELS::CHICKEN;
	pLoadInfo.position = spawnPos;
	pLoadInfo.forwardVec = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	pLoadInfo.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	pLoadInfo.hasCollider = false;
	pLoadInfo.usedDiffuse = playerModel;
	pLoadInfo.usedVertex = VERTEX_SHADER::BASIC;
	pLoadInfo.usedPixel = PIXEL_SHADER::BASIC;
	pLoadInfo.collisionLayer = COLLISION_LAYERS::PLAYER;
	pLoadInfo.usedInput = INPUT_LAYOUT::BASIC;
	pLoadInfo.usedGeo = -1;
	pLoadInfo.LoadState = 3;

	CPlayer* player;
	player = CreatePlayerFromTemplate(pLoadInfo);
	player->resetStats();
	player->setControllerIndex(numPlayer - 1);
	return player;
}
