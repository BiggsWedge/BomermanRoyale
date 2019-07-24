/*******************************************************
*	Filename:			EntityManager.h
*	Date:				07/16/2019
*	Mod. Date:			07/16/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contains the Entity
*						manager and all of it sub-classes
*						and structures.
*******************************************************/
#pragma once
#include <vector>
#include "customUtil.h"
#include "RendererModel.h"
#include "DirectXData.h"
//#include "CGame.h"
struct AssetSet
{
	const char* meshFile;
	const char* matFile;
	ID3D11VertexShader* usedVertexShader;
	ID3D11PixelShader* usedPixelShader;
};


class CGame;

class CEntityManager
{
	DirectXData
	ID3D11Device** p_d3dGameDevice;

	int numCurrPlayers;


public:
	std::vector<TMeshTemplate> vtMeshTemplates;

	TRenderer3DModel InitializeModel(TMeshTemplate tTemplate, DirectX::XMFLOAT3 position);
	
	void Initialize(CGame* _theGame);

	void AddPlayerModel(AssetSet playerStuff);

	CEntityManager();


};




