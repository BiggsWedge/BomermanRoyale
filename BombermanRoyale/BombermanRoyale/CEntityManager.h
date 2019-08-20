/****************************************************
*	Filename:			CEntityManager.h
*	Mod. Date:			07/27/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contains the
*						declerations for the
*						CEntityManger class and
*						all of its internals and
*						useful classes and structures
****************************************************/
#pragma once

#include "CPlayer.h"
#include "CItem.h"
#include <DirectXCollision.h>


class CEntityManager
{
private:

public:

	/*********************************************************
		CreateOBJFromTemplate()			Creates a CObject from
										the provided OBJLoadInfo
										information
		Ins:
										OBJLoadInfo loadInfo
		Outs:
										None
		Returns:
										CObject*

		Mod. Initials:					D.S.
		Mod. Date:						07/27/2019
	*********************************************************/
	CObject* CreateOBJFromTemplate(OBJLoadInfo loadInfo);
	CPlayer* CreatePlayerFromTemplate(OBJLoadInfo loadInfo);
	CBomb* CreateBombFromTemplate(OBJLoadInfo loadInfo);
	CItem* CreateItemFromTemplate(OBJLoadInfo loadInfo);

	CBomb* DropBomb(CPlayer* playerSource);
	std::vector<CBomb*> DropBomb0(CPlayer* playerSource);
	std::vector<CBomb*> DropBomb1(CPlayer* playerSource);
	std::vector<CBomb*> DropBomb2(CPlayer* playerSource);
	std::vector<CBomb*> DropBomb3(CPlayer* playerSource);

	CObject* BombExplosionX(CBomb* BombSource);
	CObject* BombExplosionZ(CBomb* BombSource);
	CItem* ItemDrop(CObject* ItemSource, int itemType);
	CObject* SpawnObject(CObject* obj);
	CPlayer* InstantiatePlayer(int numPlayer, int playerModel, DirectX::XMFLOAT3 spawnPos);
};

