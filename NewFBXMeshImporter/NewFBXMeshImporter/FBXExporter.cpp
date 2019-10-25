#include "FBXExporter.h"
#include <string>
#include <fstream>
using namespace std;
using namespace DirectX;


FBXMeshExporter::FBXMeshExporter()
{
}


FBXMeshExporter::~FBXMeshExporter()
{
}

void FBXMeshExporter::Initialize()
{
	_theManager = FbxManager::Create();
	_theIOSettings = FbxIOSettings::Create(_theManager, IOSROOT);
	_theScene = FbxScene::Create(_theManager, "");
	_theImporter = FbxImporter::Create(_theManager, "");
}

void FBXMeshExporter::ConvertFBX(const char* animFile, const char* baseName)
{
	bool res;
	res = _theImporter->Initialize(animFile, -1, _theManager->GetIOSettings());
	res = _theImporter->Import(_theScene);

	res = GetBindPose(_theScene, _theBindPose);

	res = GetInfluences(_theScene, control_point_influences);

	ProcessFBXMeshComplete(_theScene->GetRootNode());

	std::string outMesh;
	outMesh.append(".//Assets//Output//");
	outMesh.append(baseName);
	outMesh.append(".mesh");

	ofstream file(outMesh.c_str(), ios::trunc | ios::binary | ios::out);

	file.write((const char*)&numIndices, sizeof(unsigned int));

	file.write((const char*)indices.data(), sizeof(unsigned int) * numIndices);

	file.write((const char*)&numVertices, sizeof(unsigned int));

	file.write((const char*)vertices.data(), sizeof(Vertex) * numVertices);

	file.close();
}

void FBXMeshExporter::ProcessFBXMeshComplete(FbxNode* Node)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	const char* _name = Node->GetName();
	cout << "Name: " << _name << endl;

	int childrenCount = Node->GetChildCount();

	for (int i = 0; i < childrenCount; ++i)
	{
		FbxNode* childNode = Node->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();

		if (mesh != nullptr)
		{
			cout << "Mesh: " << childNode->GetName() << endl;

			numIndices = mesh->GetPolygonVertexCount();
			cout << "Indice Count: " << numIndices << endl;
			indices.resize(numIndices);

			int* _indices = mesh->GetPolygonVertices();
			for (int j = 0; j < numIndices; ++j)
				indices.data()[j] = _indices[j];

			numVertices = mesh->GetControlPointsCount();
			cout << "Vertex Count: " << numVertices << endl;
			vertices.resize(numVertices);

			FbxArray<FbxVector4> normalsVec;
			mesh->GetPolygonVertexNormals(normalsVec);


			for (int j = 0; j < numVertices; ++j)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);


				vertices[j]._position.x = (float)vert.mData[0];
				vertices[j]._position.y = (float)vert.mData[1];
				vertices[j]._position.z = (float)vert.mData[2];

				if (control_point_influences.size() > 0)
				{
					Influence_Set _theCurrSet = control_point_influences[j];
					float sum = _theCurrSet[0].weight + _theCurrSet[1].weight + _theCurrSet[2].weight + _theCurrSet[3].weight;

					vertices[j]._joints.x = _theCurrSet[0].joint;
					vertices[j]._weights.x = _theCurrSet[0].weight / sum;

					vertices[j]._joints.y = _theCurrSet[1].joint;
					vertices[j]._weights.y = _theCurrSet[1].weight / sum;

					vertices[j]._joints.z = _theCurrSet[2].joint;
					vertices[j]._weights.z = _theCurrSet[2].weight / sum;

					vertices[j]._joints.w = _theCurrSet[3].joint;
					vertices[j]._weights.w = _theCurrSet[3].weight / sum;
				}
			}


			cout << "Normals Vec Count: " << normalsVec.Size() << endl;

			FbxStringList lUVSetNameList;
			mesh->GetUVSetNames(lUVSetNameList);
			const char* lUVSetName = lUVSetNameList.GetStringAt(0);
			const FbxGeometryElementUV* lUVElement = mesh->GetElementUV(lUVSetName);

			Vertex* vertices2 = new Vertex[numIndices];

			for (int j = 0; j < numIndices; ++j)
			{
				vertices2[j]._position.x = vertices[indices[j]]._position.x;
				vertices2[j]._position.y = vertices[indices[j]]._position.y;
				vertices2[j]._position.z = vertices[indices[j]]._position.z;

				vertices2[j]._normals.x = normalsVec.GetAt(j)[0];
				vertices2[j]._normals.y = normalsVec.GetAt(j)[1];
				vertices2[j]._normals.z = normalsVec.GetAt(j)[2];

				vertices2[j]._joints.x = vertices[indices[j]]._joints.x;
				vertices2[j]._joints.y = vertices[indices[j]]._joints.y;
				vertices2[j]._joints.z = vertices[indices[j]]._joints.z;
				vertices2[j]._joints.w = vertices[indices[j]]._joints.w;

				vertices2[j]._weights.x = vertices[indices[j]]._weights.x;
				vertices2[j]._weights.y = vertices[indices[j]]._weights.y;
				vertices2[j]._weights.z = vertices[indices[j]]._weights.z;
				vertices2[j]._weights.w = vertices[indices[j]]._weights.w;

				if (lUVElement && lUVElement->GetReferenceMode() == FbxLayerElement::eDirect)
				{
					FbxVector2 lUVValue = lUVElement->GetDirectArray().GetAt(indices[j]);
					vertices2[j]._texCoords.x = lUVValue[0];
					vertices2[j]._texCoords.y = lUVValue[1];
				}
				else
				{
					auto& index_array = lUVElement->GetIndexArray();
					FbxVector2 LUVValue = lUVElement->GetDirectArray().GetAt(index_array[j]);

					vertices2[j]._texCoords.x = LUVValue[0];
					vertices2[j]._texCoords.y = 1.0f - LUVValue[1];
				}

			}

			vertices.clear();
			vertices.resize(numIndices);
			for (int j = 0; j < numIndices; ++j)
				vertices[j] = vertices2[j];

			indices.clear();

			for (int j = 0; j < numIndices; ++j)
				indices.push_back(j);

			numVertices = numIndices;
		}
		ProcessFBXMeshComplete(childNode);
	}
}

void FBXMeshExporter::ProcessAnimation(const char * AnimationInFile, const char * AnimationOutFile)
{
	_theImporter->Initialize(AnimationInFile, -1, _theManager->GetIOSettings());

	FbxScene* _currScene = FbxScene::Create(_theManager, "");

	_theImporter->Import(_currScene);

	Clip _clip;
	GetAnimationInfo(_currScene, _clip);

	std::string outAnimFile;
	outAnimFile.append(".//Assets//Output//");
	outAnimFile.append(AnimationOutFile);

	ofstream animFile(outAnimFile.c_str(), ios::trunc | ios::binary | ios::out);

	unsigned int numBindJoints = (unsigned int)_clip._bindPose.size();

	animFile.write((const char*)&numBindJoints, sizeof(unsigned int));

	for (int i = 0; i < numBindJoints; ++i)
	{
		animFile.write((const char*)&_clip._bindPose[i]._mat, sizeof(float) * 16);
		animFile.write((const char*)&_clip._bindPose[i]._parentIndex, sizeof(int));
	}
	animFile.write((const char*)&_clip._duration, sizeof(double));

	unsigned int numFrames = _clip._keyFrames.size();
	animFile.write((const char*)&numFrames, sizeof(unsigned int));

	for (Keyframe frame : _clip._keyFrames)
	{
		animFile.write((const char*)&frame._time, sizeof(double));
		unsigned int numJoints = frame._joints.size();
		animFile.write((const char*)&numJoints, sizeof(unsigned int));

		for (int i = 0; i < frame._joints.size(); ++i)
		{
			animFile.write((const char*)&frame._joints[i]._parentIndex, sizeof(int));
			animFile.write((const char*)frame._joints[i]._mat.m, sizeof(float) * 16);
		}
	}
	animFile.close();
}

void FBXMeshExporter::Clear()
{
	control_point_influences.clear();
	_theBindPose.clear();
	vertices.clear();
	indices.clear();
	numIndices = numVertices = 0;
}

void GetAnimationInfo(FbxScene * animScene, Clip& _clip)
{
	std::vector<FBXJoint> jointNodes;
	GetFBXJoints(animScene, jointNodes);

	FbxAnimStack* _theStack = animScene->GetCurrentAnimationStack();

	FbxTimeSpan _theTimeSpan = _theStack->GetLocalTimeSpan();
	FbxTime _time = _theTimeSpan.GetDuration();

	FbxLongLong _frameCount = _time.GetFrameCount(FbxTime::EMode::eFrames30);

	_clip._duration = _time.GetSecondDouble();

	_time.SetMilliSeconds(_theTimeSpan.GetStart().GetMilliSeconds());
	_time.SetFrame(0);

	std::vector<Joint> animBindPose;
	GetBindPose(animScene, animBindPose);
	_clip._bindPose.resize(animBindPose.size());
	for (int i = 0; i < animBindPose.size(); ++i)
		_clip._bindPose[i] = animBindPose[i];

	_time.SetFrame(1);
	double timediff = _time.GetSecondDouble();

	for (int i = 1; i < _frameCount; ++i)
	{
		Keyframe _key;
		_time.SetFrame(i, fbxsdk::FbxTime::eFrames30);
		_key._time = _time.GetSecondDouble() - timediff;

		for (FBXJoint _joint : jointNodes)
		{
			FbxAMatrix jMat = _joint._node->EvaluateGlobalTransform(_time);
			Joint _newJoint;
			_newJoint._parentIndex = _joint._parentIndex;

			for (int x = 0; x < 4; ++x)
				for (int y = 0; y < 4; ++y)
					_newJoint._mat.m[x][y] = jMat.mData[x][y];

			_key._joints.push_back(_newJoint);
		}
		_clip._keyFrames.push_back(_key);
	}
}

bool FindBindPose(FbxScene * scene, FbxPose *& bindPose)
{
	int numPoses = scene->GetPoseCount();
	if (numPoses == 0)
		return false;
	for (int i = 0; i < numPoses; ++i)
	{
		FbxPose* temp = scene->GetPose(i);
		if (temp->IsBindPose())
		{
			bindPose = scene->GetPose(i);
			return true;
		}
	}
	return false;
}

bool GetFBXJoints(FbxScene * scene, std::vector<FBXJoint>& joints)
{
	FbxPose* _theBindPose = nullptr;

	if (!FindBindPose(scene, _theBindPose))
		return false;

	int skelNodeCount = _theBindPose->GetCount();
	FbxNode* rootNode = nullptr;

	for (int i = 0; i < skelNodeCount; ++i)
	{
		FbxNode* temp = _theBindPose->GetNode(i);
		FbxSkeleton* tempSkel = temp->GetSkeleton();
		if (tempSkel && tempSkel->IsSkeletonRoot())
		{
			rootNode = _theBindPose->GetNode(i);
			break;
		}
	}

	FBXJoint rootJoint;
	rootJoint._node = rootNode;
	rootJoint._parentIndex = -1;
	joints.push_back(rootJoint);

	for (int i = 0; i < joints.size(); ++i)
	{
		int childCount = joints[i]._node->GetChildCount();
		for (int j = 0; j < childCount; ++j)
		{
			FbxNode* child = joints[i]._node->GetChild(j);
			if (child->GetSkeleton())
			{
				FBXJoint temp;
				temp._node = child;
				temp._parentIndex = i;
				joints.push_back(temp);
			}
		}
	}
	return true;
}

bool GetBindPose(FbxScene * scene, std::vector<Joint>& bindPose)
{
	std::vector<FBXJoint> myJointNodes;

	if (!GetFBXJoints(scene, myJointNodes))
		return false;

	for (FBXJoint currJoint : myJointNodes)
	{
		FbxAMatrix _mat = currJoint._node->EvaluateGlobalTransform();
		Joint _joint;
		_joint._parentIndex = currJoint._parentIndex;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				_joint._mat.m[i][j] = _mat.mData[i][j];
		}
		bindPose.push_back(_joint);
	}

	return true;
}

bool GetInfluences(FbxScene * scene, std::vector<Influence_Set>& influences)
{
	std::vector<FBXJoint> joints;
	FbxPose* _bindPose;
	if (!FindBindPose(scene, _bindPose))
		return false;
	if (!GetFBXJoints(scene, joints))
		return false;

	int count = _bindPose->GetCount();
	FbxNode* _bindNode = _bindPose->GetNode(0);
	FbxMesh* _bindMesh = _bindNode->GetMesh();
	int controlPointsCount = _bindMesh->GetControlPointsCount();
	influences.resize(controlPointsCount);

	int defCount = _bindMesh->GetDeformerCount();

	FbxSkin* _skin = nullptr;
	for (int i = 0; i < defCount; ++i)
	{
		if (_bindMesh->GetDeformer(i)->Is<FbxSkin>())
		{
			_skin = (FbxSkin*)_bindMesh->GetDeformer(i);
			break;
		}
	}

	int clusterCount = _skin->GetClusterCount();

	for (int i = 0; i < clusterCount; ++i)
	{
		FbxCluster* _currCluster = _skin->GetCluster(i);
		FbxNode* _link = _currCluster->GetLink();
		int jointIndex = -1;
		for (int j = 0; j < joints.size(); ++j)
		{
			if (joints[j]._node == _link)
			{
				jointIndex = j;
				break;
			}

		}
		int indicesCount = _currCluster->GetControlPointIndicesCount();
		for (int j = 0; j < indicesCount; ++j)
		{
			float weight = _currCluster->GetControlPointWeights()[j];
			int controlPointIndex = _currCluster->GetControlPointIndices()[j];

			Influence currInfl;
			currInfl.joint = jointIndex;
			currInfl.weight = weight;

			Influence_Set* _theInfluenceSet = &influences[controlPointIndex];

			bool stronger = false;
			for (int k = 0; k < MAX_INFLUENCES; ++k)
			{
				if (currInfl.weight > _theInfluenceSet->at(k).weight)
				{
					stronger = true;
					break;
				}
			}

			if (stronger)
			{
				int weakest = 0;
				for (int k = 1; k < MAX_INFLUENCES; ++k)
				{
					if (_theInfluenceSet->at(k).weight < _theInfluenceSet->at(weakest).weight)
						weakest = k;
				}
				_theInfluenceSet->at(weakest) = currInfl;
			}
		}
	}

	return true;
}
