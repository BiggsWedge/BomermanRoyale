#pragma once
#include "Structs.h"





class FBXMeshExporter
{
private:
	std::vector<Influence_Set> control_point_influences;
	std::vector<Joint> _theBindPose;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int numIndices, numVertices;

	FbxManager* _theManager = nullptr;
	FbxIOSettings* _theIOSettings = nullptr;
	FbxScene* _theScene = nullptr;
	FbxImporter* _theImporter = nullptr;

public:
	FBXMeshExporter();
	~FBXMeshExporter();
	void Initialize();

	void ConvertFBX(const char* animFile, const char* baseName);
	void ProcessFBXMeshComplete(FbxNode* Node);
	void ProcessAnimation(const char* AnimationInFile, const char* AnimationOutFile);
	void Clear();

};

bool FindBindPose(FbxScene* scene, FbxPose* &bindPose);
bool GetFBXJoints(FbxScene* scene, std::vector<FBXJoint>& joints);
bool GetBindPose(FbxScene* scene, std::vector<Joint>& bindPose);
bool GetInfluences(FbxScene* scene, std::vector<Influence_Set>& influences);
void GetAnimationInfo(FbxScene* animScene, Clip& clip);

