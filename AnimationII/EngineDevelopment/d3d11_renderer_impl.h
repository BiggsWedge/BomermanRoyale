#pragma once

#define VSYNC_ON 0

#define SAFE_RELEASE(ptr) {if(ptr) {ptr->Release(); ptr = nullptr;}}

#define SWAP(var, opt1, opt2) {var == opt1 ? var = opt2 : var = opt1;}

#define ANIM BattleMage._anim.frames

#include "renderer.h"
#include "view.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <vector>
#include "ps_cube.csh"
#include "vs_cube.csh"
#include "debug_renderer_vs.csh"
#include "debug_renderer_ps.csh"
#include "debug_renderer.h"
#include <vector>
#include "pools.h"
#include <ctime>
#include <thread>
#include <ratio>
#include <iostream>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DXGI.lib")

// NOTE: This header file must *ONLY* be included by renderer.cpp

namespace end
{
	struct cbuffer
	{
		DirectX::XMMATRIX modeling;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};

	struct model
	{
		float3 pos;
		DirectX::XMMATRIX matrix;
	};


	void launchParticle(particle* _out, float4 color)
	{
		uint32_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		srand(seed);
		int dirx = (rand() % 21) - 10, diry = (rand() % 21) - 10, dirz = (rand() % 21) - 10;
		float3 _newDir = { (float)dirx, (float)diry, (float)dirz };
		particle _temp(normalize(_newDir), color);
		_temp.speed = ((rand() % 1000) / 10.0f);
		*_out = _temp;
	}

	struct renderer::impl
	{
		// platform/api specific members, functions, etc.
		// Device, swapchain, resource views, states, etc. can be members here
		ID3D11Device *device = nullptr;
		ID3D11DeviceContext *context = nullptr;
		IDXGISwapChain *swapchain = nullptr;

		ID3D11RenderTargetView*		render_target[VIEW_RENDER_TARGET::COUNT]{};

		ID3D11DepthStencilView*		depthStencilView[VIEW_DEPTH_STENCIL::COUNT]{};

		ID3D11DepthStencilState*	depthStencilState[STATE_DEPTH_STENCIL::COUNT]{};

		ID3D11Buffer*				vertex_buffer[VERTEX_BUFFER::COUNT]{};

		ID3D11Buffer*				index_buffer[INDEX_BUFFER::COUNT]{};

		ID3D11InputLayout*			input_layout[INPUT_LAYOUT::COUNT]{};

		ID3D11VertexShader*			vertex_shader[VERTEX_SHADER::COUNT]{};

		ID3D11Buffer*				constant_buffer[CONSTANT_BUFFER::COUNT]{};

		ID3D11PixelShader*			pixel_shader[PIXEL_SHADER::COUNT]{};

		ID3D11RasterizerState*		rasterState[STATE_RASTERIZER::COUNT]{};

		D3D11_VIEWPORT				view_port[VIEWPORT::COUNT]{};

		/* Add more as needed...
		ID3D11SamplerState*			sampler_state[STATE_SAMPLER::COUNT]{};

		ID3D11BlendState*			blend_state[STATE_BLEND::COUNT]{};
		*/

		int currKeyFrame = 0;

		float frameTime = 0;

		float currFrameIndex = 0;

		float animTime = 0.0f;

		bool manualInput = true;


		SimpleModel BattleMage;

		D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID;

		void toggleFillMode()
		{
			SWAP(fillMode, D3D11_FILL_SOLID, D3D11_FILL_WIREFRAME);
			recreateRasterState();
		}

		float totalTime;

	private:
		void recreateRasterState()
		{
			SAFE_RELEASE(rasterState[STATE_RASTERIZER::DEFAULT]);

			D3D11_RASTERIZER_DESC rasterDesc;
			ZeroMemory(&rasterDesc, sizeof(rasterDesc));
			rasterDesc.FillMode = fillMode;
			rasterDesc.CullMode = D3D11_CULL_BACK;
			rasterDesc.FrontCounterClockwise = false;
			rasterDesc.DepthBias = false;
			rasterDesc.DepthBiasClamp = 1.0f;
			rasterDesc.SlopeScaledDepthBias = 0;
			rasterDesc.DepthClipEnable = true;
			rasterDesc.ScissorEnable = false;
			rasterDesc.MultisampleEnable = true;
			rasterDesc.AntialiasedLineEnable = true;


			HRESULT result = device->CreateRasterizerState(&rasterDesc, &rasterState[STATE_RASTERIZER::DEFAULT]);
			if (FAILED(result))
				printf("Failed to create Rasterizer State!\n");
		}
	public:


		// Constructor for renderer implementation
		// 
		impl(native_handle_type window_handle, view_t& default_view)
		{

#pragma region Model Loading



#pragma endregion

#pragma region Default View Creation

			DirectX::XMFLOAT3 _camPos = DirectX::XMFLOAT3(0.0f, 15.0f, -15.0f);
			DirectX::XMFLOAT3 _target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 _up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

			float3 camPos = { 0.0f, 15.0f, -15.0f };
			float3 target = { 0.0f, 0.0f, 0.0f };
			float3 up = { 0.0f, 1.0f, 0.0f };

			default_view._viewMatrix = LookAt(camPos, target, up);
			default_view._projetionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(40), 1280.0f / 720.0f, 0.1f, 1000.0f);
			//default_view._projetionMatrix = DirectX::XMMatrixOrthographicLH(16, 9, 0.1f, 1000.0f);

#pragma endregion

#pragma region Device and Swap Chain Creation

			DXGI_SWAP_CHAIN_DESC swapDesc;
			ZeroMemory(&swapDesc, sizeof(swapDesc));
			swapDesc.BufferCount = 1;
			swapDesc.BufferDesc.Width = 1280;
			swapDesc.BufferDesc.Height = 720;
			swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapDesc.OutputWindow = (HWND)window_handle;
			swapDesc.SampleDesc.Count = 1;
			swapDesc.SampleDesc.Quality = 0;
			swapDesc.Windowed = true;

			D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
			UINT numLevelsRequested = 1;
			D3D_FEATURE_LEVEL FeatureLevelsSupported;
			HRESULT result;
			result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &swapDesc, &swapchain, &device, &FeatureLevelsSupported, &context);
			if (FAILED(result))
				printf("Failed to create Device and Swap Chain!\n");

#pragma endregion

#pragma region Render Target Creation

			ID3D11Texture2D* rtvTex;

			result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&rtvTex));
			if (FAILED(result))
				printf("Failed to get Render Target Buffer from swapchain!\n");

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));

			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			rtvDesc.Texture2D.MipSlice = 0;

			result = device->CreateRenderTargetView(rtvTex, &rtvDesc, &render_target[VIEW_RENDER_TARGET::DEFAULT]);
			if (FAILED(result))
				printf("Failed to create RenderTarget View!\n");

			SAFE_RELEASE(rtvTex);

#pragma endregion

#pragma region Depth Stencil Buffer, State, and View Creation

			ID3D11Texture2D* depthStencil;
			D3D11_TEXTURE2D_DESC dsDesc;
			ZeroMemory(&dsDesc, sizeof(dsDesc));
			dsDesc.Width = 1280;
			dsDesc.Height = 720;
			dsDesc.MipLevels = 1;
			dsDesc.ArraySize = 1;
			dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsDesc.SampleDesc.Count = 1;
			dsDesc.SampleDesc.Quality = 0;
			dsDesc.Usage = D3D11_USAGE_DEFAULT;
			dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			dsDesc.CPUAccessFlags = 0;
			dsDesc.MiscFlags = 0;
			result = device->CreateTexture2D(&dsDesc, nullptr, &depthStencil);
			if (FAILED(result))
				printf("Failed to create DepthStencil Texture!\n");

			D3D11_DEPTH_STENCIL_DESC depthDesc;
			ZeroMemory(&depthDesc, sizeof(depthDesc));
			depthDesc.DepthEnable = TRUE;
			depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

			depthDesc.StencilEnable = TRUE;
			depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

			depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			result = device->CreateDepthStencilState(&depthDesc, &depthStencilState[STATE_DEPTH_STENCIL::DEFAULT]);
			if (FAILED(result))
				printf("Failed to create Depth Stencil State!\n");

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			ZeroMemory(&dsvDesc, sizeof(dsvDesc));
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			result = device->CreateDepthStencilView(depthStencil, &dsvDesc, &depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);
			if (FAILED(result))
				printf("Failed to create the Depth Stencil View!\n");

			SAFE_RELEASE(depthStencil);

#pragma endregion

#pragma region Viewport and Rasterizer Creation

			view_port[VIEWPORT::DEFAULT].Height = 720;
			view_port[VIEWPORT::DEFAULT].Width = 1280;
			view_port[VIEWPORT::DEFAULT].TopLeftX = view_port[0].TopLeftY = 0;
			view_port[VIEWPORT::DEFAULT].MaxDepth = 1;
			view_port[VIEWPORT::DEFAULT].MinDepth = 0;

			D3D11_RASTERIZER_DESC rasterDesc;
			ZeroMemory(&rasterDesc, sizeof(rasterDesc));
			rasterDesc.FillMode = D3D11_FILL_SOLID;
			rasterDesc.CullMode = D3D11_CULL_BACK;
			rasterDesc.FrontCounterClockwise = true;
			rasterDesc.DepthBias = false;
			rasterDesc.DepthBiasClamp = 1.0f;
			rasterDesc.SlopeScaledDepthBias = 0;
			rasterDesc.DepthClipEnable = true;
			rasterDesc.ScissorEnable = false;
			rasterDesc.MultisampleEnable = true;
			rasterDesc.AntialiasedLineEnable = true;


			result = device->CreateRasterizerState(&rasterDesc, &rasterState[STATE_RASTERIZER::DEFAULT]);
			if (FAILED(result))
				printf("Failed to create Rasterizer State!\n");

#pragma endregion

#pragma region Shader Creation

			result = device->CreateVertexShader(vs_cube, sizeof(vs_cube), nullptr, &vertex_shader[VERTEX_SHADER::COLORED_VERTEX]);
			if (FAILED(result))
				printf("Failed to create Vertex Shader!\n");

			result = device->CreatePixelShader(ps_cube, sizeof(ps_cube), nullptr, &pixel_shader[PIXEL_SHADER::COLORED_VERTEX]);
			if (FAILED(result))
				printf("Failed to create Pixel Shader!\n");


			result = device->CreateVertexShader(debug_renderer_vs, sizeof(debug_renderer_vs), nullptr, &vertex_shader[VERTEX_SHADER::DEBUG_RENDERER]);
			if (FAILED(result))
				printf("Failed to create Grid Vertex Shader!\n");

			result = device->CreatePixelShader(debug_renderer_ps, sizeof(debug_renderer_ps), nullptr, &pixel_shader[PIXEL_SHADER::DEBUG_RENDERER]);
			if (FAILED(result))
				printf("Failed to create Grid Pixel Shader!\n");

#pragma endregion

#pragma region Constant Buffer Creation

			D3D11_BUFFER_DESC constBuffDesc;
			ZeroMemory(&constBuffDesc, sizeof(constBuffDesc));
			constBuffDesc.Usage = D3D11_USAGE_DEFAULT;
			constBuffDesc.ByteWidth = sizeof(cbuffer);
			constBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constBuffDesc.CPUAccessFlags = 0;
			constBuffDesc.MiscFlags = 0;

			result = device->CreateBuffer(&constBuffDesc, nullptr, &constant_buffer[CONSTANT_BUFFER::MVP]);

			if (FAILED(result))
				printf("Failed to create Constant Buffer!\n");

			result = device->CreateBuffer(&constBuffDesc, nullptr, &constant_buffer[CONSTANT_BUFFER::DEBUG_RENDERER]);
			if (FAILED(result))
				printf("Failed to create Debug Renderer Constant Buffer!\n");

			constBuffDesc.ByteWidth = sizeof(LightCB);

			result = device->CreateBuffer(&constBuffDesc, nullptr, &constant_buffer[CONSTANT_BUFFER::LIGHTS]);
			if (FAILED(result))
				printf("Failed to create Lights Constant Buffer!\n");

			constBuffDesc.ByteWidth = sizeof(matCB);

			result = device->CreateBuffer(&constBuffDesc, nullptr, &constant_buffer[CONSTANT_BUFFER::MATERIAL]);
			if (FAILED(result))
				printf("Failed to create Lights Constant Buffer!\n");

			constBuffDesc.ByteWidth = sizeof(jointCB);

			result = device->CreateBuffer(&constBuffDesc, nullptr, &constant_buffer[CONSTANT_BUFFER::JOINTS]);
			if (FAILED(result))
				printf("Failed to create Joints Constant Buffer!\n");

#pragma endregion

#pragma region Vertex Shader Input Layout

			std::vector<D3D11_INPUT_ELEMENT_DESC> regShaderDesc =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"JOINTS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			result = device->CreateInputLayout(regShaderDesc.data(), regShaderDesc.size(), vs_cube, sizeof(vs_cube), &input_layout[INPUT_LAYOUT::REG_SHADER]);
			if (FAILED(result))
				printf("Failed to create the Regular Vertex Shader Input Layout!");

#pragma endregion

#pragma region Debug Renderer Vertex Shader Input Layout

			std::vector<D3D11_INPUT_ELEMENT_DESC> gridDescription =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			result = device->CreateInputLayout(gridDescription.data(), gridDescription.size(), debug_renderer_vs, sizeof(debug_renderer_vs), &input_layout[INPUT_LAYOUT::DEBUG_RENDERER]);
			if (FAILED(result))
				printf("Failed to create the Debug Renderer Vertex Input Layout!\n");

#pragma endregion

			BattleMage.loadModel(".\\Assets\\BattleMage.mesh", ".\\Assets\\BattleMage.mat", ".\\Assets\\BattleMage.anim");
			BattleMage.initialize(device);

#pragma region Vertex Buffer Creation



			D3D11_BUFFER_DESC gridVertBuffDesc;
			D3D11_SUBRESOURCE_DATA gridSubResc;

			ZeroMemory(&gridVertBuffDesc, sizeof(gridVertBuffDesc));
			ZeroMemory(&gridSubResc, sizeof(gridSubResc));

			gridVertBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			gridVertBuffDesc.ByteWidth = sizeof(colored_vertex) * debug_renderer::get_line_vert_capacity();
			gridVertBuffDesc.CPUAccessFlags = 0;
			gridVertBuffDesc.MiscFlags = 0;
			gridVertBuffDesc.StructureByteStride = 0;
			gridVertBuffDesc.Usage = D3D11_USAGE_DEFAULT;

			gridSubResc.pSysMem = debug_renderer::get_line_verts();

			device->CreateBuffer(&gridVertBuffDesc, &gridSubResc, &vertex_buffer[VERTEX_BUFFER::DEBUG_RENDERER]);

			recreateRasterState();
#pragma endregion

		}

		void draw_view(view_t& v, float timePassed)
		{
			animTime += frameTime;
			totalTime += timePassed;
			static float gridRed = 0, gridBlue = 0, gridGreen = 0;

			float fractionalTime = timePassed - (int)timePassed;

			// TODO:
			// Apply view properties
			//	Set and clear render targets
			//	Set viewport(s), etc

#pragma region Cleans up window

			context->ClearDepthStencilView(depthStencilView[0], D3D11_CLEAR_DEPTH, 1, 0);

			ID3D11RenderTargetView* const targets[] = { render_target[VIEW_RENDER_TARGET::DEFAULT] };

			context->OMSetRenderTargets(1, targets, depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

			context->RSSetViewports(1, view_port);

			const float d_background[] = { 0.0f, 0.5f, 0.0f, 1.0f };

			context->ClearRenderTargetView(render_target[VIEW_RENDER_TARGET::DEFAULT], d_background);

			context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);

			debug_renderer::clear_lines();

#pragma endregion

#pragma region Draws Grid

			int start = 0;

			for (int i = 0; i < 11; i++)
			{
				float3 pos1, pos2;

				float4 col;

				pos1.x = pos2.x = -5.0f + i;
				pos1.y = pos2.y = 0.0f;
				pos1.z = 5.0;
				pos2.z = -5.0;

				col.x = gridRed;
				col.y = gridGreen;
				col.z = gridBlue;
				col.w = 1.0f;

				debug_renderer::add_line(pos1, pos2, col);

				pos1.x = 5.0f;
				pos2.x = -5.0f;
				pos1.y = pos2.y = 0.0f;
				pos1.z = pos2.z = -5.0f + i;

				debug_renderer::add_line(pos1, pos2, col);
			}

			switch ((int)timePassed % 6)
			{
			case 0: // TO MAGENTA
				gridRed = colorLerp(0, 1, fractionalTime);
				gridGreen = 0;
				gridBlue = 1;
				break;
			case 1: //  TO RED
				gridRed = 1;
				gridGreen = 0;
				gridBlue = colorLerp(1, 0, fractionalTime);
				break;
			case 2: //  TO YELLOW
				gridRed = 1;
				gridGreen = colorLerp(0, 1, fractionalTime);
				gridBlue = 0;
				break;
			case 3: // TO GREEN
				gridRed = colorLerp(1, 0, fractionalTime);
				gridGreen = 1;
				gridBlue = 0;
				break;
			case 4: // TO CYAN
				gridRed = 0;
				gridGreen = 1;
				gridBlue = colorLerp(0, 1, fractionalTime);
				break;
			case 5: // TO BLUE
				gridRed = 0;
				gridGreen = colorLerp(1, 0, fractionalTime);
				gridBlue = 1;
				break;
			default:
				gridRed = gridGreen = gridBlue = 0;
				break;
			}

#pragma endregion

			if (currKeyFrame == BattleMage._anim.frames.size())
				currKeyFrame = 0;

			while (animTime > BattleMage._anim.frames[currFrameIndex + 1].time)
			{
				currFrameIndex++;
				if (currFrameIndex == BattleMage._anim.frames.size() - 1)
				{
					animTime -= BattleMage._anim.duration;
					currFrameIndex = 0;
				}
			}

			float startTime, endTime;
			startTime = BattleMage._anim.frames[currFrameIndex].time;
			endTime = BattleMage._anim.frames[currFrameIndex + 1].time;

			float ratio = (animTime - startTime) / (endTime - startTime);

			KeyFrame _key = BattleMage._anim.frames[(manualInput) ? currKeyFrame : currFrameIndex];

			jointCB _jointsConst;
			_jointsConst.numJoints = ANIM[currFrameIndex].joints.size();

			for (int i = 0; i < ANIM[currFrameIndex].joints.size(); ++i)
			{
				DirectX::XMMATRIX tween = matLerp(ANIM[currFrameIndex].joints[i]._mat, ANIM[currFrameIndex + 1].joints[i]._mat, ratio);

				debug_renderer::drawMatrix(tween);

				_jointsConst._joints[i] = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, BattleMage._bindPose[i]._mat) * tween);

				if (_key.joints[i].parentIndex >= 0)
				{
					DirectX::XMFLOAT4X4 startMat, endMat;

					DirectX::XMStoreFloat4x4(&startMat, matLerp(ANIM[currFrameIndex].joints[i]._mat, ANIM[currFrameIndex + 1].joints[i]._mat, ratio));
					DirectX::XMStoreFloat4x4(&endMat, matLerp(ANIM[currFrameIndex].joints[ANIM[currFrameIndex].joints[i].parentIndex]._mat, ANIM[currFrameIndex + 1].joints[ANIM[currFrameIndex + 1].joints[i].parentIndex]._mat, ratio));

					float3 start = { startMat._41, startMat._42, startMat._43 };
					float3 end = { endMat._41, endMat._42, endMat._43 };

					debug_renderer::add_line(start, end, float4(1.0f, 1.0f, 1.0f, 1.0f));
				}

				
			}


#pragma region Draws Debug Renderer

			cbuffer debugConstBuff;
			debugConstBuff.modeling = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180));
			debugConstBuff.proj = DirectX::XMMatrixTranspose(v._projetionMatrix);
			debugConstBuff.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, v._viewMatrix));

			UINT strides = sizeof(colored_vertex);
			UINT offsets = 0;

			context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::DEBUG_RENDERER], 0, nullptr, &debugConstBuff, 0, 0);

			context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::DEBUG_RENDERER], &strides, &offsets);

			context->UpdateSubresource(vertex_buffer[VERTEX_BUFFER::DEBUG_RENDERER], 0, nullptr, debug_renderer::get_line_verts(), 0, 0);

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			context->IASetInputLayout(input_layout[INPUT_LAYOUT::DEBUG_RENDERER]);

			context->VSSetShader(vertex_shader[VERTEX_SHADER::DEBUG_RENDERER], 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::DEBUG_RENDERER]);

			context->PSSetShader(pixel_shader[PIXEL_SHADER::DEBUG_RENDERER], 0, 0);

			context->Draw(debug_renderer::get_line_vert_count(), 0);

#pragma endregion

			Light pointLight;
			pointLight.position = DirectX::XMFLOAT3(0.0, 6.0f, -2.0f);
			pointLight.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			pointLight.intensity = 0.25f;
			pointLight.type = POINT_LIGHT;

			LightCB _light;
			_light._light = pointLight;

			strides = sizeof(Simple_Vertex);

			matCB _mat;
			_mat._mat = BattleMage._mat;


			debugConstBuff.modeling = DirectX::XMMatrixIdentity();
			debugConstBuff.modeling = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180));
			debugConstBuff.modeling = debugConstBuff.modeling * DirectX::XMMatrixTranslation(3.0f, 0.0f, 0.0f);
			debugConstBuff.modeling = DirectX::XMMatrixTranspose(debugConstBuff.modeling);

			context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, nullptr, &debugConstBuff, 0, 0);

			context->IASetInputLayout(input_layout[INPUT_LAYOUT::REG_SHADER]);

			context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::LIGHTS], 0, nullptr, &_light, 0, 0);
			context->PSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::LIGHTS]);

			context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MATERIAL], 0, nullptr, &_mat, 0, 0);
			context->PSSetConstantBuffers(1, 1, &constant_buffer[CONSTANT_BUFFER::MATERIAL]);

			context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::JOINTS], 0, nullptr, &_jointsConst, 0, 0);
			context->VSSetConstantBuffers(1, 1, &constant_buffer[CONSTANT_BUFFER::JOINTS]);

			context->VSSetShader(vertex_shader[VERTEX_SHADER::COLORED_VERTEX], 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

			context->PSSetShader(pixel_shader[PIXEL_SHADER::COLORED_VERTEX], 0, 0);

			context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			BattleMage.render(context);

			// **SKIP**:
			// Draw batches in visible set (Implemented In a future assignment)

			// TODO:
			// Using the vs_cube/ps_cube shaders, Draw 36 vertices to render a cube.
			// The vs_cube shader contains all the vertex data, so no vertex buffers are required

			swapchain->Present(1, 0);
		}

		~impl()
		{
			// Releases Vertex Buffers
			for (int i = 0; i < VERTEX_BUFFER::COUNT; ++i)
				SAFE_RELEASE(vertex_buffer[i]);

			// Releases Index Buffers
			for (int i = 0; i < INDEX_BUFFER::COUNT; ++i)
				SAFE_RELEASE(index_buffer[i]);

			// Releases Input Layouts
			for (int i = 0; i < INPUT_LAYOUT::COUNT; ++i)
				SAFE_RELEASE(input_layout[i]);

			// Releases Pixel Shaders
			for (int i = 0; i < PIXEL_SHADER::COUNT; ++i)
				SAFE_RELEASE(pixel_shader[i]);

			// Releases Vertex Shaders
			for (int i = 0; i < VERTEX_SHADER::COUNT; ++i)
				SAFE_RELEASE(vertex_shader[i]);

			// Releases Constant Buffers
			for (int i = 0; i < CONSTANT_BUFFER::COUNT; ++i)
				SAFE_RELEASE(constant_buffer[i]);

			// Releases Rasterizer States
			for (int i = 0; i < STATE_RASTERIZER::COUNT; ++i)
				SAFE_RELEASE(rasterState[i]);

			// Releases Depth Stencil Views
			for (int i = 0; i < VIEW_DEPTH_STENCIL::COUNT; ++i)
				SAFE_RELEASE(depthStencilView[i]);

			// Releases Depth Stencil States
			for (int i = 0; i < STATE_DEPTH_STENCIL::COUNT; ++i)
				SAFE_RELEASE(depthStencilState[i]);

			// Releases Render Target Views
			for (int i = 0; i < VIEW_RENDER_TARGET::COUNT; ++i)
				SAFE_RELEASE(render_target[i]);

			// Releases Swapchain
			SAFE_RELEASE(swapchain);

			// Releases Context
			SAFE_RELEASE(context);

			// Sets up Live Object Debugging
			ID3D11Debug* _myDebug = nullptr;
			HRESULT result;
			result = device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&_myDebug));
			if (SUCCEEDED(result))
				result = _myDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

			//Releases Debugger
			SAFE_RELEASE(_myDebug);

			// Releases Device
			SAFE_RELEASE(device);
		}



	};
}