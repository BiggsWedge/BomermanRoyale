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
DirectX::XMFLOAT3 bombPos1, bombPos2, bombPos3, bombPos4;
bool pos1Full, pos2Full, pos3Full, pos4Full;

CObject* CEntityManager::CreateOBJFromTemplate(OBJLoadInfo loadInfo)
{
	CObject* temp = new CObject();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable, loadInfo.item, loadInfo.itemType, loadInfo.layer);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TMaterialComponent* mat = new TMaterialComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mat);

	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	return temp;
}

CPlayer* CEntityManager::CreatePlayerFromTemplate(OBJLoadInfo loadInfo)
{
	CPlayer* temp = new CPlayer();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable, false, loadInfo.itemType, loadInfo.layer);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TMaterialComponent* mat = new TMaterialComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mat);
	if (v_tMeshTemplates[loadInfo.meshID]._animations.size() > 0)
	{
		TAnimComponent* anim = new TAnimComponent(v_tMeshTemplates.at(loadInfo.meshID));
		temp->AddComponent((TComponent*)anim);
	}
	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	return temp;
}

CBomb * CEntityManager::CreateBombFromTemplate(OBJLoadInfo loadInfo)
{
	CBomb* temp = new CBomb();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable, loadInfo.item, loadInfo.itemType, loadInfo.layer);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	return temp;
}

CItem * CEntityManager::CreateItemFromTemplate(OBJLoadInfo loadInfo)
{
	CItem* temp = new CItem();
	TTransformComponent* transform = new TTransformComponent(loadInfo.position, loadInfo.forwardVec, loadInfo.scale, loadInfo.floor, loadInfo.destroyable, loadInfo.item, loadInfo.itemType, loadInfo.layer);
	temp->AddComponent((TComponent*)transform);

	TRendererComponent* renderer = new TRendererComponent(loadInfo.usedVertex, loadInfo.usedPixel, loadInfo.usedInput, loadInfo.usedGeo, loadInfo.LoadState);
	temp->AddComponent((TComponent*)renderer);

	TMeshComponent* mesh = new TMeshComponent(v_tMeshTemplates.at(loadInfo.meshID));
	temp->AddComponent((TComponent*)mesh);

	TTextureComponent* tex = new TTextureComponent(loadInfo.usedDiffuse);
	temp->AddComponent((TComponent*)tex);

	TColliderComponent* collider = new TColliderComponent(v_tMeshTemplates[loadInfo.meshID], loadInfo.scale, loadInfo.position, loadInfo.collisionLayer);
	temp->AddComponent((TComponent*)collider);

	temp->SetItemType(loadInfo.itemType);

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
	if (cTransform->fPosition.y == 0)
	{
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}
	else
	{
		pos.y = 0;
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}

	loadInfo.position = pos;
	loadInfo.forwardVec = { 1.0f, 0.0f, 0.0f };
	loadInfo.meshID = MODELS::BOMB;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB;
	loadInfo.usedVertex = VERTEX_SHADER::BOMB;
	loadInfo.usedPixel = PIXEL_SHADER::BOMB;
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

std::vector<CBomb*> CEntityManager::DropBomb0(CPlayer* playerSource, std::vector<CObject*> objects)
{
	std::vector<CBomb*> bombs;
	CBomb* bomb;
	TComponent* transform;
	TTransformComponent* cTransform;
	TTransformComponent* oTransform;
	playerSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;
	if (cTransform->fPosition.y == 0)
	{
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}
	else
	{
		pos.y = 0;
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}




	loadInfo.forwardVec = { 1.0f, 0.0f, 0.0f };
	loadInfo.meshID = MODELS::BOMB2;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB3;
	loadInfo.usedVertex = VERTEX_SHADER::BOMB;
	loadInfo.usedPixel = PIXEL_SHADER::BOMB;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::BOMB;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f);

	//for (int i = 0; i < objects.size(); i++)
	//{
	//	objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	//	oTransform = (TTransformComponent*)transform;
	//	if (oTransform->fPosition.x == bombPos1.x)
	//	{
	//		if (oTransform->fPosition.z == bombPos1.z)
	//		{
	//			bombPos1.y = 2.5f;
	//		}
	//	}
	//	if (oTransform->fPosition.x == bombPos2.x)
	//	{
	//		if (oTransform->fPosition.z == bombPos2.z)
	//		{
	//			bombPos2.y = 2.5f;
	//		}
	//	}
	//	if (oTransform->fPosition.x == bombPos3.x)
	//	{
	//		if (oTransform->fPosition.z == bombPos3.z)
	//		{
	//			bombPos3.y = 2.5f;
	//		}
	//	}
	//	if (oTransform->fPosition.x == bombPos4.x)
	//	{
	//		if (oTransform->fPosition.z == bombPos4.z)
	//		{
	//			bombPos4.y = 2.5f;
	//		}
	//	}
	//}

	if (cTransform->fForwardVector.z > 0.5f)
	{
		pos.x += 2.5f;
		pos.y += 0;
		pos.z += 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.z += 2.5f;
		bombPos2 = pos;


		pos.x -= 5;
		pos.y += 0;
		bombPos3 = pos;


		pos.y += 0;
		pos.z -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.z < -0.5f)
	{
		pos.x += 2.5f;
		pos.y += 0;
		pos.z -= 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.z -= 2.5f;
		bombPos2 = pos;

		pos.x -= 5;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.z += 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x > 0.5f)
	{
		pos.x += 2.5f;
		pos.y += 0;
		pos.z -= 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.x += 2.5f;
		bombPos2 = pos;

		pos.z += 5;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.x -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);


		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x < -0.5f)
	{
		pos.x -= 2.5f;
		pos.y += 0;
		pos.z += 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.x -= 2.5f;
		bombPos2 = pos;

		pos.z -= 5;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.x += 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}

	return bombs;
}

std::vector<CBomb*> CEntityManager::DropBomb1(CPlayer* playerSource, std::vector<CObject*> objects)
{
	std::vector<CBomb*> bombs;
	CBomb* bomb;
	TComponent* transform;
	TTransformComponent* cTransform;
	TTransformComponent* oTransform;
	playerSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;
	if (cTransform->fPosition.y == 0)
	{
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}
	else
	{
		pos.y = 0;
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}

	loadInfo.forwardVec = { 1.0f, 0.0f, 0.0f };
	loadInfo.meshID = MODELS::BOMB;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB2;
	loadInfo.usedVertex = VERTEX_SHADER::BOMB;
	loadInfo.usedPixel = PIXEL_SHADER::BOMB;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::BOMB;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f);

	if (cTransform->fForwardVector.z > 0.5f)
	{
		pos.x += 0;
		pos.y += 0;
		pos.z += 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.z += 2.5f;
		bombPos2 = pos;

		pos.x -= 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.x += 5;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.z < -0.5f)
	{
		pos.x += 0;
		pos.z -= 2.5f;
		bombPos1 = pos;

		pos.z -= 2.5f;
		bombPos2 = pos;

		pos.x -= 2.5f;
		bombPos3 = pos;

		pos.x += 5;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x > 0.5f)
	{
		pos.x += 2.5f;
		pos.y += 0;
		pos.z -= 0;
		bombPos1 = pos;

		pos.y += 0;
		pos.x += 2.5f;
		bombPos2 = pos;

		pos.z += 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.z -= 5;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x < -0.5f)
	{
		pos.x -= 2.5f;
		pos.y += 0;
		pos.z += 0;
		bombPos1 = pos;

		pos.y += 0;
		pos.x -= 2.5f;
		bombPos2 = pos;

		pos.z -= 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.z += 5;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}

	return bombs;
}

std::vector<CBomb*> CEntityManager::DropBomb2(CPlayer* playerSource, std::vector<CObject*> objects)
{
	std::vector<CBomb*> bombs;
	CBomb* bomb;
	TComponent* transform;
	TTransformComponent* cTransform;
	TTransformComponent* oTransform;
	playerSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;
	if (cTransform->fPosition.y == 0)
	{
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}
	else
	{
		pos.y = 0;
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}

	loadInfo.forwardVec = { 1.0f, 0.0f, 0.0f };
	loadInfo.meshID = MODELS::BOMB;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
	loadInfo.usedVertex = VERTEX_SHADER::BOMB;
	loadInfo.usedPixel = PIXEL_SHADER::BOMB;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::BOMB;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f);


	if (cTransform->fForwardVector.z > 0.5f)
	{
		pos.x += 0;
		pos.y += 0;
		pos.z += 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.z += 2.5f;
		bombPos2 = pos;

		pos.z += 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.z += 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.z < -0.5f)
	{
		pos.x += 0;
		pos.y += 0;
		pos.z -= 2.5f;
		bombPos1 = pos;

		pos.y += 0;
		pos.z -= 2.5f;
		bombPos2 = pos;

		pos.z -= 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.z -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x > 0.5f)
	{
		pos.x += 2.5f;
		pos.y += 0;
		pos.z -= 0;
		bombPos1 = pos;

		pos.y += 0;
		pos.x += 2.5f;
		bombPos2 = pos;

		pos.x += 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.x += 2.5;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x < -0.5f)
	{
		pos.x -= 2.5f;
		pos.y += 0;
		pos.z += 0;
		bombPos1 = pos;

		pos.y += 0;
		pos.x -= 2.5f;
		bombPos2 = pos;

		pos.x -= 2.5f;
		pos.y += 0;
		bombPos3 = pos;

		pos.y += 0;
		pos.x -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}

	return bombs;
}

std::vector<CBomb*> CEntityManager::DropBomb3(CPlayer* playerSource, std::vector<CObject*> objects)
{
	std::vector<CBomb*> bombs;
	CBomb* bomb;
	TComponent* transform;
	TTransformComponent* cTransform;
	TTransformComponent* oTransform;
	playerSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;
	if (cTransform->fPosition.y == 0)
	{
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}
	else
	{
		pos.y = 0;
		int x = 0;
		int z = 0;
		if (pos.x >= 0) {
			x = (pos.x + 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}
		else {
			x = (pos.x - 1.25f) / 2.5f;
			pos.x = ((float)x * 2.5f);
		}

		if (pos.z >= 0) {
			z = (pos.z + 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
		else {
			z = (pos.z - 1.25f) / 2.5f;
			pos.z = ((float)z * 2.5f);
		}
	}

	loadInfo.forwardVec = { 1.0f, 0.0f, 0.0f };
	loadInfo.meshID = MODELS::BOMB2;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB4;
	loadInfo.usedVertex = VERTEX_SHADER::BOMB;
	loadInfo.usedPixel = PIXEL_SHADER::BOMB;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::BOMB;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.floor = false;
	loadInfo.scale = DirectX::XMFLOAT3(0.75f, 0.75f, 0.75f);

	if (cTransform->fForwardVector.z > 0.5f)
	{
		pos.x += 0;
		pos.y += 0;
		pos.z += 5;
		bombPos1 = pos;

		pos.x += 2.5f;
		pos.y += 0;
		pos.z += 2.5f;
		bombPos2 = pos;


		pos.x -= 2.5f;
		pos.z += 2.5f;
		bombPos3 = pos;

		pos.z -= 2.5f;
		pos.x -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.z < -0.5f)
	{
		pos.x += 0;
		pos.y += 0;
		pos.z -= 5;
		bombPos1 = pos;


		pos.x += 2.5f;
		pos.z -= 2.5f;
		bombPos2 = pos;


		pos.x -= 2.5f;
		pos.z -= 2.5f;
		bombPos3 = pos;

		pos.z += 2.5f;
		pos.x -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x > 0.5f)
	{

		pos.x += 5;
		pos.y += 0;
		pos.z -= 0;
		bombPos1 = pos;

		pos.z += 2.5f;
		pos.x += 2.5f;
		bombPos2 = pos;

		pos.z -= 2.5f;
		pos.x += 2.5f;
		bombPos3 = pos;

		pos.x -= 2.5f;
		pos.z -= 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	else if (cTransform->fForwardVector.x < -0.5f)
	{
		pos.x -= 5;
		pos.y += 0;
		pos.z += 0;
		bombPos1 = pos;

		pos.z -= 2.5f;
		pos.x -= 2.5f;
		bombPos2 = pos;

		pos.z += 2.5f;
		pos.x -= 2.5f;
		bombPos3 = pos;

		pos.x += 2.5f;
		pos.z += 2.5f;
		bombPos4 = pos;

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos1.x)
			{
				if (oTransform->fPosition.z == bombPos1.z)
				{
					if (oTransform->fPosition.y == bombPos1.y)
					{
						bombPos1.y = 2.5f;
						break;
					}
					else
					{
						bombPos1.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos1;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos2.x)
			{
				if (oTransform->fPosition.z == bombPos2.z)
				{
					if (oTransform->fPosition.y == bombPos2.y)
					{
						bombPos2.y = 2.5f;
						break;
					}
					else
					{
						bombPos2.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos2;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos3.x)
			{
				if (oTransform->fPosition.z == bombPos3.z)
				{
					if (oTransform->fPosition.y == bombPos3.y)
					{
						bombPos3.y = 2.5f;
						break;
					}
					else
					{
						bombPos3.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos3;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
			oTransform = (TTransformComponent*)transform;
			if (oTransform->fPosition.x == bombPos4.x)
			{
				if (oTransform->fPosition.z == bombPos4.z)
				{
					if (oTransform->fPosition.y == bombPos4.y)
					{
						bombPos4.y = 2.5f;
						break;
					}
					else
					{
						bombPos4.y = 0;
					}
				}
			}
		}
		loadInfo.position = bombPos4;
		bomb = CreateBombFromTemplate(loadInfo);
		bomb->initialize(playerSource);
		bombs.push_back(bomb);
	}
	return bombs;
}

void CEntityManager::Cleanup()
{

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

CItem* CEntityManager::ItemDrop(CObject* ItemSource, int itemType)
{
	CItem* Item;
	TComponent* transform;
	TTransformComponent* cTransform;
	ItemSource->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;

	pos.y += 1.5f;

	loadInfo.position = pos;
	loadInfo.forwardVec = cTransform->fForwardVector;
	loadInfo.meshID = 0;

	switch (itemType)
	{
	case 1:
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB2;
		break;
	case 2:
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB3;
		break;
	case 4:
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BATTLE_MAGE;
		break;
	case 3:
		loadInfo.usedDiffuse = DIFFUSE_TEXTURES::BOMB4;
		break;
	}
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.collisionLayer = COLLISION_LAYERS::ITEM;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.destroyable = false;
	loadInfo.floor = false;
	loadInfo.itemType = itemType;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 60.0f, 1.0f / 180.0f, 1.0f / 60.0f);
	Item = CreateItemFromTemplate(loadInfo);

	return Item;
}

CObject* CEntityManager::SpawnObject(CObject* obj) {
	CObject* Item;
	TComponent* transform;
	TTransformComponent* cTransform;
	obj->GetComponent(COMPONENT_TYPE::TRANSFORM, transform);
	cTransform = (TTransformComponent*)transform;

	OBJLoadInfo loadInfo;

	DirectX::XMFLOAT3 pos = cTransform->fPosition;

	loadInfo.position = pos;
	loadInfo.forwardVec = { 0.0f, 0.0f, -1.0f };
	loadInfo.meshID = 0;
	loadInfo.usedDiffuse = DIFFUSE_TEXTURES::RED_TEX;
	loadInfo.usedVertex = VERTEX_SHADER::BASIC;
	loadInfo.usedPixel = PIXEL_SHADER::BASIC;
	loadInfo.usedInput = INPUT_LAYOUT::BASIC;
	loadInfo.usedGeo = -1;
	loadInfo.LoadState = 3;
	loadInfo.scale = DirectX::XMFLOAT3(1.0f / 10.0f, 1.0f / 10.0f, 1.0f / 10.0f);
	Item = CreateOBJFromTemplate(loadInfo);

	return Item;
}

CPlayer* CEntityManager::InstantiatePlayer(int numPlayer, int playerModel, int playerSkin, DirectX::XMFLOAT3 spawnPos, int loadState, DirectX::XMFLOAT3 forwardVec, DirectX::XMFLOAT3 scale)
{
	OBJLoadInfo pLoadInfo;
	pLoadInfo.meshID = playerModel;
	pLoadInfo.position = spawnPos;
	pLoadInfo.forwardVec = forwardVec;
	pLoadInfo.scale = scale;
	pLoadInfo.usedDiffuse = playerSkin;
	pLoadInfo.usedVertex = VERTEX_SHADER::ANIM;
	pLoadInfo.usedPixel = PIXEL_SHADER::ANIM;
	pLoadInfo.collisionLayer = COLLISION_LAYERS::PLAYER;
	pLoadInfo.usedInput = INPUT_LAYOUT::BASIC;
	pLoadInfo.usedGeo = -1;
	pLoadInfo.LoadState = loadState;

	CPlayer* player = nullptr;
	player = CreatePlayerFromTemplate(pLoadInfo);
	player->resetStats();
	player->Initialize();
	player->setControllerIndex(numPlayer - 1);
	player->GetCharacterController()->SetButtonCodes(G_SOUTH_BTN, G_START_BTN, G_SELECT_BTN, G_RIGHT_SHOULDER_BTN);
	return player;
}
