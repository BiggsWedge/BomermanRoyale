#pragma once
#include<fbxsdk.h>
#include <direct.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <array>
#include <vector>
#include <iostream>

using file_path_t = std::array<char, 260>;

struct FileSet
{
	const char* meshFile = nullptr;
	const char* animFile = nullptr;
	const char* baseFileName = nullptr;
	const char* animName = nullptr;
};

#define MAX_INFLUENCES 4

struct Influence
{
	int joint;
	float weight;

	Influence() { joint = -1; weight = 0.0f; }
};

using Influence_Set = std::array<Influence, MAX_INFLUENCES>;

struct Vertex
{
	DirectX::XMFLOAT3	_position;
	DirectX::XMFLOAT3	_normals;
	DirectX::XMFLOAT2	_texCoords;
	DirectX::XMFLOAT4	_tangents;
	DirectX::XMINT4		_joints;
	DirectX::XMFLOAT4	_weights;
};

struct FBXJoint
{
	FbxNode* _node;
	int _parentIndex;
};

struct Joint
{
	DirectX::XMFLOAT4X4 _mat;
	int _parentIndex;
};

struct Keyframe
{
	double _time;
	std::vector<Joint> _joints;
};

struct Clip
{
	std::vector<Joint> _bindPose;
	double _duration;
	std::vector<Keyframe> _keyFrames;
};

inline DirectX::XMFLOAT3 NormalizeFloat3(DirectX::XMFLOAT3 toNorm)
{
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&toNorm);
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMFLOAT3 toRet;
	DirectX::XMStoreFloat3(&toRet, vec);
	return toRet;
}

inline DirectX::XMFLOAT3 subFloat3(DirectX::XMFLOAT3 toSub, DirectX::XMFLOAT3 from)
{
	DirectX::XMFLOAT3 toRet =
	{
		from.x - toSub.x,
		from.y - toSub.y,
		from.z - toSub.z
	};
	return toRet;
}
