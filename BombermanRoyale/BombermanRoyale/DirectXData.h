#pragma once

#include <direct.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "ConstDefines.h"
#include "../Gateware/Interface/G_Graphics/GDirectX11Surface.h"

#define SAFE_RELEASE(ptr){if(ptr){ptr->Release(); ptr = nullptr;}}

struct TBasicVertexConstBuff
{
	DirectX::XMMATRIX mModelMatrix;
	DirectX::XMMATRIX mViewMatrix;
	DirectX::XMMATRIX mProjMatrix;
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
	enum { V_BASIC = 0, V_LINE, P_BASIC, P_LINE, COUNT };
};

struct INDEX_BUFFER
{
	enum { DEFAULT = 0, COUNT };
};

struct VERTEX_BUFFER
{
	enum { DEFAULT = 0, LINE, COUNT };
};

struct VERTEX_SHADER
{
	enum { BASIC = 0, LINE, COUNT };
};

struct PIXEL_SHADER
{
	enum { BASIC = 0, LINE, COUNT };
};

struct GEOMETRY_SHADER
{
	enum { DEFAULT = 0, COUNT };
};

struct INPUT_LAYOUT
{
	enum { BASIC = 0, LINE, COUNT };
};

struct RASTERIZER_STATE
{
	enum { DEFAULT = 0, COUNT };
};

struct DEPTH_STENCIL_VIEW
{
	enum { DEFAULT = 0, COUNT };
};

struct DEPTH_STENCIL_STATE
{
	enum { DEFAULT = 0, COUNT };
};

struct COLLISION_LAYERS
{
	enum { PLAYER, BOMB, EXPLOSION, WALL, DESTROYABLE, FLOOR, ITEM, COUNT };
};

struct DIFFUSE_TEXTURES
{
	enum {
		CRATE = 0, BATTLE_MAGE, MAIN_MENU, HUD, NAMES_HUD, WIN_SCREEN,
		HELP_MENU, ARCADE_BUTTON, BATTLE_BUTTON, OPTIONS_BUTTON, EXIT_BUTTON,
		RED_TEX, BLUE_TEX, BLACK_TEX, FIRE_TEX, HAY_TEX, BOMB, CHICKEN1, CHICKEN2,
		COUNT
	};
};

struct NORMAL_TEXTURES
{
	enum { COUNT = 0 };
};

class DirectXData
{
public:

	bool collisionMatrix[COLLISION_LAYERS::COUNT][COLLISION_LAYERS::COUNT];
	GW::GRAPHICS::GDirectX11Surface*	d3dSurface = nullptr;
	ID3D11Device*						d3dDevice = nullptr;
	IDXGISwapChain*						d3dSwapChain = nullptr;
	ID3D11DeviceContext*				d3dContext = nullptr;

	ID3D11VertexShader*					d3dVertexShader[VERTEX_SHADER::COUNT] = {};
	ID3D11InputLayout*					d3dInputLayout[INPUT_LAYOUT::COUNT] = {};
	ID3D11PixelShader*					d3dPixelShader[PIXEL_SHADER::COUNT] = {};
	ID3D11GeometryShader*				d3dGeometryShader[GEOMETRY_SHADER::COUNT] = {};

	ID3D11Buffer*						d3dConstBuffers[CONSTANT_BUFFER::COUNT] = {};
	ID3D11Buffer*						d3dVertexBuffers[VERTEX_BUFFER::COUNT] = {};
	ID3D11RenderTargetView*				d3dRenderTargetView = nullptr;
	ID3D11DepthStencilView*				d3dDepthStencilView = nullptr;
	ID3D11RasterizerState*				d3dRasterizerState = nullptr;
	ID3D11SamplerState*					d3dSamplerState = nullptr;

	ID3D11ShaderResourceView*			d3dDiffuseTextures[DIFFUSE_TEXTURES::COUNT] = {};

	bool								bUseDebugRenderCamera = false;


	DirectX::XMMATRIX					camMat;
	DirectX::XMMATRIX					debugCamMat;
	DirectX::XMFLOAT3					debugCamPos;
	DirectX::XMFLOAT2					debugCamDelta;
	DirectX::XMFLOAT2					debugCursorRot;
	DirectX::XMMATRIX					viewMat;
	DirectX::XMMATRIX					projMat;
	DirectX::XMFLOAT3					camPos;
	DirectX::XMFLOAT3					newCamPos;

	TBasicVertexConstBuff					basicConstBuff;

	std::vector<ID3D11CommandList*>		d3dCommandLists = {};

	D3D11_VIEWPORT						d3dViewport;

	bool Initialize();
	DirectXData();
	~DirectXData();
	void Cleanup();
	inline void ToggleUseDebugCamera() { bUseDebugRenderCamera = !bUseDebugRenderCamera; }
	void updateCameras();
};

extern DirectXData* g_d3dData;
