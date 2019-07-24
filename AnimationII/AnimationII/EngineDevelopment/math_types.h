#pragma once

#include <array>
#include <cstdint>
#include <DirectXMath.h>
#include <iostream>
#include <vector>
#include <d3d11.h>
#include <fstream>
#include "WICTextureLoader.h"
#include <filesystem>

namespace end
{
	struct float2
	{
		float x;
		float y;

		inline float& operator[](int i) { return (&x)[i]; }
		inline float operator[](int i)const { return (&x)[i]; }
		float2& operator=(float2 _other);

		inline float* data() { return &x; }
		inline const float* data()const { return &x; }
		inline constexpr size_t size()const { return 2; }

		float2& operator-(float2 _other);
		float2(float _x, float _y) { x = _x; y = _y; }
		float2() {};
	};

	struct float3
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};

			float2 xy;
		};

		inline float& operator[](int i) { return (&x)[i]; }
		inline float operator[](int i)const { return (&x)[i]; }

		inline float* data() { return &x; }
		inline const float* data()const { return &x; }
		inline constexpr size_t size()const { return 3; }
		float3&  operator=(float3 _other);
		inline bool operator==(float _val) { return this->x == _val && this->y == _val && this->z == _val; }
		float3& operator*(float _val);
		float3& operator+(float3 _other);
		float3& operator-(float3 _other);
		float3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
		float3() { x = y = z = 0; }
		float3 operator/(float d) { return float3(x / d, y / d, z / d); }

	};

	struct float4
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			float3 xyz;

			struct
			{
				float2 xy;
				float2 zw;
			};
		};

		inline float& operator[](int i) { return (&x)[i]; }
		inline float operator[](int i)const { return (&x)[i]; }

		inline float* data() { return &x; }
		inline const float* data()const { return &x; }
		inline constexpr size_t size()const { return 4; }
		float4& operator=(float4 _other);
		inline bool operator==(float _val) { return (this->x == _val && this->y == _val && this->z == _val && this->w == _val); }
		float4() {};
		float4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
		float4 operator/(float _divisor) { return float4(x / _divisor, y / _divisor, z / _divisor, w / _divisor); }
	};

	struct alignas(8) float2_a : float2 {};

	struct alignas(16) float3_a : float3 {};

	struct alignas(16) float4_a : float4 {};

	using float4x4 = std::array< float4, 4 >;
	using float4x4_a = std::array< float4_a, 4 >;

}

namespace end
{

	using file_path_t = std::array<char, 260>;

	struct colored_vertex
	{
		float3 pos = { 0.0f, 0.0f, 0.0f };
		float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		colored_vertex() = default;
		colored_vertex(const colored_vertex&) = default;

		inline colored_vertex(const float3& p, const float4& c) : pos{ p }, color{ c } {}
		inline colored_vertex(const float3& p, const float3& c) : pos{ p }, color{ c.x, c.y, c.z, 1.0f } {}
		inline colored_vertex(const float3& p, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : pos{ p }, color{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f } {}
	};



	struct Simple_Vertex
	{
		float3 Position;
		float3 Normal;
		float2 Texture;
		int joints[4];
		float4 weights;
	};

	struct jointCB
	{
		int numJoints;
		DirectX::XMMATRIX _joints[28];
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

	struct Material
	{
		float surfaceDiffuse[3];
		float diffuseFactor;
		float surfaceEmissive[3];
		float emissiveFactor;
		float surfaceSpecular[3];
		float specularFactor;
		float surfaceShiny[3];
		float shininess;
	};

	struct joint
	{
		DirectX::XMMATRIX _mat;
		int parentIndex;
	};


	struct KeyFrame
	{
		double time;
		std::vector<joint> joints;
	};

	struct AnimationClip
	{
		double duration;
		std::vector<KeyFrame> frames;
	};

	struct SimpleModel
	{
		std::vector<Simple_Vertex> _vertices;
		std::vector<uint32_t> _indices;

		std::vector<joint> _bindPose;

		uint32_t numVerts;
		uint32_t numIndices;

		Material _mat;

		AnimationClip _anim;

		ID3D11Buffer* _vertexBuffer;
		ID3D11Buffer* _indexBuffer;

		std::vector<file_path_t> filePaths;
		std::vector<material_t> mats;

		enum TEXTURES { DIFFUSE = 0, EMISSIVE, SPECULAR, COUNT };

		ID3D11ShaderResourceView*	_srv[TEXTURES::COUNT];
		ID3D11Resource*				_textures[TEXTURES::COUNT];
		ID3D11SamplerState*			_samState;


		void loadModel(const char* modelFile, const char* matFile = nullptr, const char* animFile = nullptr);
		void initialize(ID3D11Device* _device);
		void render(ID3D11DeviceContext* _context);

	};

	enum LIGHT_TYPE
	{
		AMBIENT_LIGHT = 0,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};








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

	struct LightCB
	{
		Light _light;
	};
	struct matCB
	{
		Material _mat;
	};

}

namespace end
{
	float distance(float3 _a, float3 _b);

	float lerp(float x, float y, float ratio);

	DirectX::XMMATRIX matLerp(DirectX::XMMATRIX x, DirectX::XMMATRIX y, float ratio);

	float3 normalize(float3 _toNorm);

	float3 positionLerp(float3 _a, float3 _b, float ratio);

	DirectX::XMMATRIX LookAt(float3 _pos, float3 _target, float3 _localUp);

	DirectX::XMMATRIX TurnTo(DirectX::XMMATRIX _mat, float3 _target, float speed);

	DirectX::XMMATRIX MouseLook(DirectX::XMMATRIX camMat, float2 angles);
}