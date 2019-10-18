#pragma once

#include <direct.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#include <DirectXCollision.h>
#include "ConstDefines.h"
#include "../Gateware/Interface/G_Graphics/GDirectX11Surface.h"
#include "DXTK/SpriteFont.h"

#define SAFE_RELEASE(ptr){if(ptr){ptr->Release(); ptr = nullptr;}}
#define GW_SAFE_RELEASE(ptr){if(ptr){ptr->DecrementCount(); ptr = nullptr;}}

struct TBasicVertexConstBuff
{
	DirectX::XMMATRIX mModelMatrix;
	DirectX::XMMATRIX mViewMatrix;
	DirectX::XMMATRIX mProjMatrix;
};

struct KeyVertex
{
	DirectX::XMFLOAT3 xyzw;
	DirectX::XMFLOAT2 uv;
};

struct TBasicPixelConstBuff
{
	int flags[8];
};

struct RENDER_TARGET_VIEW
{
	enum { DEFAULT = 0, COUNT };
};

struct VIEWPORT
{
	enum { DEFAULT = 0, COUNT };
};

struct CONSTANT_BUFFER
{
	enum { V_BASIC = 0, V_LINE, P_BASIC, P_LINE, MVP_t, LIGHTS, JOINTS, MATERIAL, SKY, BOMBCONST, BOMB_P_CONST, COUNT };
};

struct INDEX_BUFFER
{
	enum { DEFAULT = 0, ANIM, SKY, COUNT };
};

struct VERTEX_BUFFER
{
	enum { DEFAULT = 0, LINE, ANIM, SKY, COUNT };
};

struct VERTEX_SHADER
{
	enum { BASIC = 0, LINE, ANIM, SKY, BOMB, EXPLOSION, COUNT };
};

struct PIXEL_SHADER
{
	enum { BASIC = 0, LINE, ANIM, SKY, BOMB, FIRE, COUNT };
};

struct GEOMETRY_SHADER
{
	enum { DEFAULT = 0, PARTICLE, MESH_EXPLOSION, COUNT };
};

struct COMPUTE_SHADER
{
	enum { DEFAULT = 0, PARTICLE, COUNT };
};

struct INPUT_LAYOUT
{
	enum { BASIC = 0, LINE, SKY, COUNT };
};

struct RASTERIZER_STATE
{
	enum { DEFAULT = 0, SKYBOX, COUNT };
};

struct DEPTH_STENCIL_VIEW
{
	enum { DEFAULT = 0, COUNT };
};

struct DEPTH_STENCIL_STATE
{
	enum { DEFAULT = 0, TWO_D, COUNT };
};

struct COLLISION_LAYERS
{
	enum { PLAYER, BOMB, EXPLOSION, WALL, DESTROYABLE, FLOOR, ITEM, COUNT };
};

struct GRID_SYSTEM
{
	enum { FREE, DESTROYABLE, CRATE, BOMB, EXPLOSION_RADIUS, PLAYER, POWERUP, COUNT };
};

struct DIFFUSE_TEXTURES
{
	enum {
		CRATE = 0, BATTLE_MAGE, MAIN_MENU, MAIN_MENU2, MAIN_MENU3, MAIN_MENU4, HUD, NAMES_HUD, PLAYER_1_WIN, PLAYER_2_WIN, PLAYER_3_WIN, PLAYER_4_WIN, DRAW_SCREEN, LOADING_SCREEN, DC1, DC2, DC3, D4, GP_LOGO, FS_LOGO, DEV_ICON, PAUSE_MENU,
		HELP_MENU, ARCADE_MENU, ARCADE_MENU2, ARCADE_MENU3, ARCADE_MENU4, ARCADE_MENU5, ARCADE_MENU6, ARCADE_MENU7, ARCADE_MENU8, ARCADE_MENU9, CHARACTER_SCREEN1, 
		CHARACTER_SCREEN2, CHARACTER_SCREE3, CHARACTER_SCREEN4, CHARACTER_SCREEN5, CHARACTER_SCREEN6, CHARACTER_SCREEN7, CHARACTER_SCREEN8, ARCADE_BUTTON, 
		BATTLE_BUTTON, OPTIONS_BUTTON, EXIT_BUTTON, RED_TEX, BLUE_TEX, BLACK_TEX, FIRE_TEX, HAY_TEX, BOMB, BOMB2, BOMB3, BOMB4, BOMB5, CHICKEN1, CHICKEN2, CHICKEN3, CHICKEN4, FIRE_WALL1,
		FIRE_WALL2, FIRE_WALL3, FIRE_WALL4, FIRE_WALL5, FIRE_WALL6, FIRE_WALL7, FIRE_WALL8, FIRE_WALL9, FIRE_WALL10, FIRE_WALL11, FIRE_WALL12, FIRE_WALL13, EXPL_1, EXPL_2, EXPL_3, EXPL_4, EXPL_5, EXPL_6, EXPL_7, GOAT, BOAR, BOAR2,
		MAIN_MENU_1, ARCADE_MENU_1 = 151, ARCADE_MENU_END = 258, COUNT

	};
};

struct BLEND_STATE
{
	enum { DEFAULT, UI, COUNT };
};

struct NORMAL_TEXTURES
{
	enum { COUNT = 0 };
};

class DirectXData
{
public:

	bool collisionMatrix[COLLISION_LAYERS::COUNT][COLLISION_LAYERS::COUNT];

#pragma region Essentials

	GW::GRAPHICS::GDirectX11Surface*	d3dSurface = nullptr;
	ID3D11Device*						d3dDevice = nullptr;
	IDXGISwapChain*						d3dSwapChain = nullptr;
	ID3D11DeviceContext*				d3dContext = nullptr;
	ID3D11RenderTargetView*				d3dRenderTargetView = nullptr;
	ID3D11RenderTargetView*				d3dPreMSAATargetView = nullptr;
	ID3D11DepthStencilView*				d3dDepthStencilView = nullptr;
	ID3D11DepthStencilState*			d3dDepthStencilState[DEPTH_STENCIL_STATE::COUNT] = {};

#pragma endregion

#pragma region Shader Stuff

	ID3D11InputLayout*					d3dInputLayout[INPUT_LAYOUT::COUNT] = {};
	ID3D11VertexShader*					d3dVertexShader[VERTEX_SHADER::COUNT] = {};
	ID3D11PixelShader*					d3dPixelShader[PIXEL_SHADER::COUNT] = {};
	ID3D11GeometryShader*				d3dGeometryShader[GEOMETRY_SHADER::COUNT] = {};
	ID3D11ComputeShader*				d3dComputeShader[COMPUTE_SHADER::COUNT] = {};
	ID3D11Buffer*						d3dConstBuffers[CONSTANT_BUFFER::COUNT] = {};

#pragma endregion

#pragma region Texture Stuff

	ID3D11RasterizerState*				d3dRasterizerState[RASTERIZER_STATE::COUNT] = {};
	ID3D11SamplerState*					d3dSamplerState = nullptr;
	ID3D11ShaderResourceView*			d3dDiffuseTextures[DIFFUSE_TEXTURES::COUNT] = {};
	ID3D11BlendState*					d3dBlendState[BLEND_STATE::COUNT] = {};

#pragma endregion

#pragma region Sprite Stuff

	DirectX::SpriteBatch*				d3dSpriteBatch;
	DirectX::SpriteFont*				d3dSpriteFont;

#pragma endregion

#pragma region Skybox Stuff

	ID3D11Texture2D*					Jungle = nullptr;
	ID3D11ShaderResourceView*			JungleSRV = nullptr;
	ID3D11SamplerState*					JungleSampler = nullptr;
	ID3D11Buffer*						JungleVertexBuffer = nullptr;
	ID3D11Buffer*						JungleIndexBuffer = nullptr;

#pragma endregion

	bool								bUseDebugRenderCamera = false;
	bool								drawColliders;

	DirectX::XMMATRIX					camMat;
	DirectX::XMMATRIX					debugCamMat;
	DirectX::XMFLOAT3					debugCamPos;
	DirectX::XMFLOAT2					debugCamDelta;
	DirectX::XMFLOAT2					camDelta;
	DirectX::XMFLOAT2					debugCursorRot;
	DirectX::XMMATRIX					viewMat;
	DirectX::XMMATRIX					viewMatSky;
	DirectX::XMMATRIX					projMat;
	DirectX::XMFLOAT3					camPos;
	DirectX::XMFLOAT3					newCamPos;
	DirectX::XMMATRIX					tempCamera;

	TBasicVertexConstBuff				basicConstBuff;



	D3D11_VIEWPORT						d3dViewport;

	DirectX::XMUINT2					windowWidthHeight;

	bool Initialize();
	DirectXData();
	~DirectXData();
	void Cleanup();
	inline void ToggleUseDebugCamera() { bUseDebugRenderCamera = !bUseDebugRenderCamera; }
	void updateCameras();
	DirectX::XMMATRIX screenShake();
	void resetCamera();
};

extern DirectXData* g_d3dData;
