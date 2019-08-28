#pragma once

#include "fbxsdk.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <random>
#include <DirectXMath.h>
#include <ostream>
#include <fstream>
#include <ios>
#include <array>

#define RAND_NORMAL (rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX)

using file_path_t = std::array<char, 260>;

#define MAX_INFLUENCES 4

struct influence
{
	int joint;
	float weight;

	influence() { joint = -1; weight = 0; }
};

using influence_set = std::array < influence, MAX_INFLUENCES>;

std::vector<influence_set> control_point_influences;

struct SimpleVertex
{
	float Pos[3];
	float Normal[3];
	float Tex[2];
	int joints[4];
	float weights[4];
};

void ConvertFBX(const char* fileName);

void ProcessFbxMeshComplete(FbxNode* Node, std::vector<SimpleVertex>* vertices, std::vector<int>* indices, int* numIndices, int* numVertices);

void Compactify(std::vector<SimpleVertex>* vertices, std::vector<int>* indices, int* numIndices, int* numVertices);

struct FbxJoint
{
	FbxNode* _node;
	int parentIndex;
};



struct joint
{
	float _mat[16];
	int parent_index;
};


struct Keyframe
{
	double time;
	std::vector<joint> joints;
};


struct AnimClip
{
	double duration;
	std::vector<Keyframe> frames;
};



DirectX::XMFLOAT3 subFloat3(DirectX::XMFLOAT3 toSub, DirectX::XMFLOAT3 from)
{
	DirectX::XMFLOAT3 toRet =
	{
		from.x - toSub.x,
		from.y - toSub.y,
		from.z - toSub.z
	};
	return toRet;
}

DirectX::XMFLOAT3 NormalizeFloat3(DirectX::XMFLOAT3 toNorm)
{
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&toNorm);
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMFLOAT3 toRet;
	DirectX::XMStoreFloat3(&toRet, vec);
	return toRet;
}

struct material_t
{
	enum COMPONENT { EMISSIVE = 0, DIFFUSE, SPECULAR, SHININESS, COUNT };

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




