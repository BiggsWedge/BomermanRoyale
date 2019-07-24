#include "debug_renderer.h"
#include <array>

// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	constexpr size_t MAX_LINE_VERTS = 4096;

	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;
	std::array< end::colored_vertex, MAX_LINE_VERTS> line_verts;
}

namespace end
{
	namespace debug_renderer
	{
		void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b)
		{
			// Add points to debug_verts, increments debug_vert_count
			colored_vertex temp1 = {}, temp2 = {};
			temp1.color = color_a;
			temp1.pos = point_a;

			temp2.color = color_b;
			temp2.pos = point_b;

			line_verts[line_vert_count++] = temp1;
			line_verts[line_vert_count++] = temp2;
		}

		void clear_lines()
		{
			line_vert_count = 0;
		}

		const colored_vertex* get_line_verts()
		{
			return line_verts.data();
		}

		size_t get_line_vert_count()
		{
			return line_vert_count;
		}

		size_t get_line_vert_capacity()
		{
			return MAX_LINE_VERTS;
		}


		void drawMatrix(DirectX::XMMATRIX _toDraw)
		{

			DirectX::XMFLOAT3 tmX, tmY, tmZ, tmP, pos;
			DirectX::XMStoreFloat3(&tmX, _toDraw.r[0]);
			DirectX::XMStoreFloat3(&tmY, _toDraw.r[1]);
			DirectX::XMStoreFloat3(&tmZ, _toDraw.r[2]);
			DirectX::XMStoreFloat3(&pos, _toDraw.r[3]);
			float3 _tmX = float3(tmX.x, tmX.y, tmX.z), _tmY = float3(tmY.x, tmY.y, tmY.z), _tmZ = float3(tmZ.x, tmZ.y, tmZ.z);
			float3 _pos = { pos.x, pos.y, pos.z };


			add_line(_pos, _pos + (_tmX / 4.0f), float4(1.0f, 0.0f, 0.0f, 1.0f), float4(1.0f, 0.0f, 0.0f, 1.0f));
			add_line(_pos, _pos + (_tmY / 4.0f), float4(0.0f, 1.0f, 0.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f));
			add_line(_pos, _pos + (_tmZ / 4.0f), float4(0.0f, 0.0f, 1.0f, 1.0f), float4(0.f, 0.0f, 1.0f, 1.0f));

		}
	}
}