#pragma once

#include <ctime>
#include <string>
#include <array>
#include <experimental/filesystem>
#include "..\Gateware\Interface\G_System\GLog.h"
#include "..\Gateware\Interface\G_System\GWindow.h"
#include "..\Gateware\Interface\G_System\GController.h"
#include "..\Gateware\Interface\G_System\GInput.h"
#include "..\Gateware\Interface\G_Audio\GAudio.h"
#include "WICTextureLoader.h"
#include "BasicVertex.csh"
#include "BasicPixel.csh"
#include "LinePixel.csh"
#include "LineVertex.csh"
#include "AnimPixel.csh"
#include "AnimVertex.csh"
#include "BombShader.csh"
#include "BombPShader.csh"
#include "GeometryShader.csh"
#include "MeshExplosion.csh"
#include "ComputeShader.csh"
#include "ExplosionVertex.csh"
#include "FirePixel.csh"
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
extern GW::AUDIO::GSound* walkSound1;
extern GW::AUDIO::GSound* bombPlaceSound1;
extern GW::AUDIO::GSound* spawnSound1;
extern GW::AUDIO::GSound* bombPlaceSound2;
extern GW::AUDIO::GSound* spawnSound2;
extern GW::AUDIO::GSound* warnSound;
extern GW::AUDIO::GSound* fallingSound;
extern GW::AUDIO::GSound* playerfallingSound;
extern GW::AUDIO::GSound* WinScreenSound;
extern GW::AUDIO::GSound* DeathSound;

extern std::vector<GW::AUDIO::GSound*> MenuSounds;
extern std::vector<GW::AUDIO::GSound*> explosionSound;
extern std::vector<GW::AUDIO::GSound*> bombPlaceSound;
extern std::vector<GW::AUDIO::GSound*> powerUpSound;
extern GW::SYSTEM::GController* g_pControllerInput;


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
*	InitializeControllerInput():	Initializes the Controller InputManager.
									Returns true if successfully initialized
									or false if not.
*	Ins:
*								None
*	Outs:
*								None
*	Returns:
*								Bool
*
*	Mod. Date:					08/4/2019
*	Mod. Initials:				D.R.
***********************************************************************/
bool InitializeControllerInput();

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
	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//TLineVertex() = default;
	//TLineVertex(const TLineVertex&) = default;

	//inline TLineVertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT4& c) : pos{ p }, color{ c } {}
	//inline TLineVertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT4& c) : pos{ p }, color{ c.x, c.y, c.z, 1.0f } {}
	//inline TLineVertex(const DirectX::XMFLOAT3& p, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : pos{ p }, color{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f } {}
};

struct jointCB
{
	int numJoints;
	DirectX::XMMATRIX _joints[50];
};

struct bombconstbuffer
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	float time;
	DirectX::XMFLOAT3 padding;
};

struct bombpixelbuffer
{
	float4 time;
};


struct material_t
{
	enum COMPONENT { DIFFUSE = 0, EMISSIVE, SPECULAR, SHININESS, COUNT };

	struct component_t
	{
		float value[3] = { 0.0f, 0.0f, 0.0f };
		float factor = 0.0f;
		int64_t input = -1;
	};

	component_t& operator[](int i) { return components[i]; }
	const component_t& operator[](int i)const { return components[i]; }

private:
	component_t components[COMPONENT::COUNT];
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

struct joint
{
	DirectX::XMFLOAT4X4 _mat;
	int parentIndex;
};

struct KeyFrame
{
	double time;
	std::vector<joint> joints;
};

struct AnimationClip
{
	std::string AnimationName;
	std::vector<joint> _bindPose;
	double duration;
	std::vector<KeyFrame> frames;
};

struct TMeshTemplate
{
	UINT uID;
	std::string						sName;
	std::vector<TSimpleVertex>		v_tVertices;
	std::vector<int>				v_iIndices;

	std::vector<joint> _bindPose;

	std::vector<AnimationClip>		_animations;

	uint32_t numVerts;
	uint32_t numIndices;

	TMaterial _mat;

	std::vector<file_path_t> filePaths;
	std::vector<material_t> mats;

	enum TEXTURES { DIFFUSE = 0, EMISSIVE, SPECULAR, COUNT };

	void loadModel(const char* modelFile, const char* matFile = nullptr, const char* animFile = nullptr, float scale = 1.0f);

	/*~TMeshTemplate();
	TMeshTemplate();*/
};


struct MVP_t
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

//struct joints
//{
//	int numJoints;
//	DirectX::XMMATRIX joint[30];
//};

struct Light
{
	DirectX::XMFLOAT3 dir;
	float intensity;
	DirectX::XMFLOAT3 position;
	int type;
	DirectX::XMFLOAT3 color;
private:
	float pad2;
};

struct LightBuffer
{
	Light light;
};

struct MatBuffer
{
	TMaterial material;
};

extern std::vector<TMeshTemplate> v_tMeshTemplates;

void LoadModels();

void LoadModel(TMeshLoadInfo loadInfo);

void LoadMenuScreen(int width, int height, int numbuttons, const char* matFile);

void LoadTextures();

void add_line(DirectX::XMFLOAT3 point_a, DirectX::XMFLOAT3 point_b, DirectX::XMFLOAT4 color_a, DirectX::XMFLOAT4 color_b);

inline void add_line(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 q, DirectX::XMFLOAT4 color) { add_line(p, q, color, color); }

void clear_lines();

const TLineVertex* get_line_verts();

size_t get_line_vert_count();

size_t get_line_vert_capacity();

void drawAABB(float3 point_a, float3 point_b, float3 point_c, float3 point_d, float3 point_e, float3 point_f, float3 point_g, float3 point_h, float4 color1, float4 color2);

void drawAABB(DirectX::XMFLOAT3 tlf, DirectX::XMFLOAT3 tlb, DirectX::XMFLOAT3 trf, DirectX::XMFLOAT3 trb, DirectX::XMFLOAT3 bfl, DirectX::XMFLOAT3 bfr, DirectX::XMFLOAT3 bbl, DirectX::XMFLOAT3 bbr, DirectX::XMFLOAT4 color);

float3 XMVector2Float3(DirectX::XMVECTOR vector);

DirectX::XMVECTOR Float32XMVector(float3 point);

DirectX::XMVECTOR RPYFromVector(DirectX::XMVECTOR RPY);

DirectX::XMMATRIX TurnTo(DirectX::XMMATRIX _mat, DirectX::XMVECTOR _target, float speed);

float lerp(float x, float y, float ratio);

DirectX::XMMATRIX matLerp(DirectX::XMMATRIX x, DirectX::XMMATRIX y, float ratio);

void CleanGlobals();

void LoadAnimations();

AnimationClip ProcessAnimationFile(const char* animFileName);

