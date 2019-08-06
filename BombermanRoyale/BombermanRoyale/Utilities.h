#pragma once

#include <ctime>
#include <string>
#include <array>
#include <experimental/filesystem>
#include "..\Gateware\Interface\G_System\GLog.h"
#include "..\Gateware\Interface\G_System\GWindow.h"
#include "..\Gateware\Interface\G_System\GInput.h"
#include "..\Gateware\Interface\G_Audio\GAudio.h"
#include "WICTextureLoader.h"
#include "BasicVertex.csh"
#include "BasicPixel.csh"
#include "LinePixel.csh"
#include "LineVertex.csh"
#include "DirectXData.h"
#include "SharedDefines.h"
#include <fstream>

using file_path_t = std::array<char, 260>;

extern GW::SYSTEM::GLog* g_pLogger;
extern GW::SYSTEM::GWindow* g_pWindow;
extern GW::SYSTEM::GInput* g_pInputRecord;
extern GW::AUDIO::GAudio* g_pAudioHolder;
extern GW::AUDIO::GMusic* g_pMusicStream;
extern GW::AUDIO::GSound* g_pSoundPlayer;

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
*	InitializeInput():	Initializes the InputManager.
						Returns true if successfully initialized
						or false if not.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								Bool
*
*	Mod. Date:					07/29/2019
*	Mod. Initials:				D.R.
***********************************************************************/
bool InitializeInput();

/***********************************************************************
*	InitializeInput():	Initializes the AudioManager.
						Returns true if successfully initialized
						or false if not.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								Bool
*
*	Mod. Date:					07/29/2019
*	Mod. Initials:				D.R.
***********************************************************************/
bool InitializeAudio();

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

struct TSimpleVertex {
	DirectX::XMFLOAT3		fPosition;
	DirectX::XMFLOAT3		fNormal;
	DirectX::XMFLOAT2		fTexCoord;
	DirectX::XMINT4			iJointIndies;
	DirectX::XMFLOAT4		fWeights;
};

struct TLineVertex {
	float3 pos = { 0.0f, 0.0f, 0.0f };
	float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	TLineVertex() = default;
	TLineVertex(const TLineVertex&) = default;

	inline TLineVertex(const float3& p, const float4& c) : pos{ p }, color{ c } {}
	inline TLineVertex(const float3& p, const float3& c) : pos{ p }, color{ c.x, c.y, c.z, 1.0f } {}
	inline TLineVertex(const float3& p, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : pos{ p }, color{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f } {}
};

struct TMaterial {
	DirectX::XMFLOAT3		fSurfaceDiffuse;
	float					fDiffuseFactor;
	DirectX::XMFLOAT3		fSurfaceEmissive;
	float					fEmissiveFactor;
	DirectX::XMFLOAT3		fSurfaceSpecular;
	float					fSpecularFactor;
	DirectX::XMFLOAT3		fSurfaceShiny;
	float					fShinyFactor;
};


struct TMeshTemplate {
	UINT uID;
	std::vector<TSimpleVertex>		v_tVertices;
	std::vector<int>				v_iIndices;
};

struct TCollider {
	DirectX::XMFLOAT3 center;
	DirectX::XMFLOAT3 extents;
	DirectX::XMFLOAT3* corners;

	TCollider() { corners = new float3[8](); }
};

extern std::vector<TMeshTemplate> v_tMeshTemplates;

void LoadModel(const char* meshFile, const char* matFile);

void LoadMenuScreen(int width, int height, int numbuttons, const char* matFile);

void LoadTextures();

