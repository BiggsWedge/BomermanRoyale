#pragma once
#include "Component.h"

struct OBJLoadInfo
{
	int meshID;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 forwardVec;
	DirectX::XMFLOAT3 scale;
	TCollider collider;
	bool hasCollider;
	int usedVertex, usedPixel, usedGeo, usedInput, usedDiffuse, LoadState;
	bool floor;
};

class CObject
{

	std::vector<TComponent*>			v_tComponents;
	ID3D11DeviceContext*			d3dDeferredContext;

public:
	CObject();
	inline std::vector<TComponent*> GetComponenets() { return v_tComponents; }
	inline void AddComponent(TComponent* _toAdd) { v_tComponents.push_back(_toAdd); }
	bool GetComponent(int componentType, TComponent* & component);
	void Draw();
	bool Move(float _x, float _z);
	
	void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b);

	inline void add_line(float3 p, float3 q, float4 color) { add_line(p, q, color, color); }

	void clear_lines();

	const TLineVertex* get_line_verts();

	size_t get_line_vert_count();

	size_t get_line_vert_capacity();

	void drawAABB(float3 point_a, float3 point_b, float3 point_c, float3 point_d, float3 point_e, float3 point_f, float3 point_g, float3 point_h, float4 color1, float4 color2);

	float3 XMVector2Float3(DirectX::XMVECTOR vector);

	DirectX::XMVECTOR Float32XMVector(float3 point);

	void LoadLines(/*std::vector<CObject*> Objects*/);

	TCollider GetCenter(TMeshTemplate _verts);

	DirectX::XMFLOAT3 GetExtents(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ);

	void GetCorners(float3 _center, float3 _extents, float3*& corners);
};




