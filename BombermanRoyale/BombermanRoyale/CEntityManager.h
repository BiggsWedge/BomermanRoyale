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
	CExplosion* CreateExplosionFromTemplate(OBJLoadInfo loadInfo);
	CItem* CreateItemFromTemplate(OBJLoadInfo loadInfo);

	CBomb* DropBomb0(CPlayer* playerSource, int game_state);
	std::vector<CBomb*> DropBomb1(CPlayer* playerSource, std::vector<CObject*>, int game_state);
	std::vector<CBomb*> DropBomb2(CPlayer* playerSource, std::vector<CObject*>, int game_state);
	std::vector<CBomb*> DropBomb3(CPlayer* playerSource, std::vector<CObject*>, int game_state);
	std::vector<CBomb*> DropBomb4(CPlayer* playerSource, std::vector<CObject*>, int game_state);

	void Cleanup();

	CExplosion* BombExplosionX(CBomb* BombSource, CPlayer* _player, int game_state);
	CExplosion* BombExplosionZ(CBomb* BombSource, CPlayer* _player, int game_state);
	CItem* ItemDrop(CObject* ItemSource, int itemType, int game_state);
	CObject* SpawnObject(CObject* obj, int game_state);
	CPlayer* InstantiatePlayer(int numPlayer, int playerModel, int playerSkin, DirectX::XMFLOAT3 spawnPos, int loadState = 3, DirectX::XMFLOAT3 forwardVec = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 0.75f));
};

