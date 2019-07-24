#pragma once

#include <ctime>
#include <string>
#include <array>
#include <experimental/filesystem>
#include "..\Gateware\Interface\G_System\GLog.h"
#include "..\Gateware\Interface\G_System\GWindow.h"
#include "WICTextureLoader.h"
#include "BasicVertex.csh"
#include "BasicPixel.csh"
#include "DirectXData.h"
#include <fstream>

using file_path_t = std::array<char, 260>;

extern GW::SYSTEM::GLog* g_pLogger;
extern GW::SYSTEM::GWindow* g_pWindow;

/***********************************************************************
*	GetCurrentDateAndTime():	Returns, in a string format, the
*								current date and time.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								String
*
*	Mod. Date:					07/19/2019
*	Mod. Initials:				D.S.
***********************************************************************/
std::string GetCurrentDateAndTime();

/***********************************************************************
*	InitializeLogger():			Initializes the errorlogger.
								Returns true if successfully initialized
								or false if not.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								Bool
*
*	Mod. Date:					07/19/2019
*	Mod. Initials:				D.S.
***********************************************************************/
bool InitializeLogger();

/***********************************************************************
*	InitializeWindow():			Initializes the Game Window.
								Returns true if successfully initialized
								or false if not.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								Bool
*
*	Mod. Date:					07/19/2019
*	Mod. Initials:				D.S.
***********************************************************************/
bool InitializeWindow();

/***********************************************************************
*	InitializeGlobals();		Initializes all global variables;
								Returns true if successfully initialized
								or false if not.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								Bool
*
*	Mod. Date:					07/19/2019
*	Mod. Initials:				D.S.
***********************************************************************/
bool InitializeGlobals();

struct TSimpleVertex
{
	DirectX::XMFLOAT3		fPosition;
	DirectX::XMFLOAT3		fNormal;
	DirectX::XMFLOAT2		fTexCoord;
	DirectX::XMINT4			iJointIndies;
	DirectX::XMFLOAT4		fWeights;
};

struct TMaterial
{
	DirectX::XMFLOAT3		fSurfaceDiffuse;
	float					fDiffuseFactor;
	DirectX::XMFLOAT3		fSurfaceEmissive;
	float					fEmissiveFactor;
	DirectX::XMFLOAT3		fSurfaceSpecular;
	float					fSpecularFactor;
	DirectX::XMFLOAT3		fSurfaceShiny;
	float					fShinyFactor;
};


struct TMeshTemplate
{
	UINT uID;
	std::vector<TSimpleVertex>		v_tVertices;
	std::vector<int>				v_iIndices;

};

extern std::vector<TMeshTemplate> v_tMeshTemplates;

void LoadModel(const char* meshFile, const char* matFile);

void LoadTextures();
