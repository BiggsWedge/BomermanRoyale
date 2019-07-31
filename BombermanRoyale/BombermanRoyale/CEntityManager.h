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

#include "CObject.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::XMFLOAT3 scale;
	int usedVertex, usedPixel, usedGeo, usedInput, usedDiffuse, LoadState;
};

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

};

