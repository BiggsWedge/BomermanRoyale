#include "Header.h"

using namespace std;
using namespace DirectX;

struct FileSet
{
	const char* meshFile = nullptr;
	const char* animFile = nullptr;
	const char* bastFile = nullptr;
};

void ProcessFbxMeshComplete(FbxNode* Node, vector<SimpleVertex>* vertices, vector<int>* indices, int* numIndices, int* numVertices)
{
	// set up output console
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//FBX Mesh stuff
	const char* _name = Node->GetName();
	cout << "\nName:" << _name;

	int childrenCount = Node->GetChildCount();

	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode *childNode = Node->GetChild(i);
		FbxMesh *mesh = childNode->GetMesh();

		if (mesh != NULL)
		{
			cout << "\nMesh:" << childNode->GetName();

			// Get index count from mesh
			*numIndices = mesh->GetPolygonVertexCount();
			cout << "\nIndice Count:" << *numIndices;

			indices->resize(*numIndices);
			// No need to allocate int array, FBX does for us
			int* _indices = mesh->GetPolygonVertices();
			for (int i = 0; i < *numIndices; ++i)
				indices->data()[i] = _indices[i];

			// Get vertex count from mesh
			*numVertices = mesh->GetControlPointsCount();
			cout << "\nVertex Count:" << *numVertices;

			// Create (DirectX) SimpleVertex array to size of this mesh
			vertices->resize(*numVertices);

			//================= Process Vertices ====================================
			for (int j = 0; j < *numVertices; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				vertices->data()[j].Pos[0] = (float)vert.mData[0];
				vertices->data()[j].Pos[1] = (float)vert.mData[1];
				vertices->data()[j].Pos[2] = (float)vert.mData[2];
				//vertices[j].Color = RAND_COLOR;
				// Generate random normal for first attempt at getting to render
				vertices->data()[j].Normal[0] = rand() / float(RAND_MAX);
				vertices->data()[j].Normal[1] = rand() / float(RAND_MAX);
				vertices->data()[j].Normal[2] = rand() / float(RAND_MAX);

				vertices->data()[j].Tex[0] = vertices->data()[j].Pos[0];
				vertices->data()[j].Tex[1] = vertices->data()[j].Pos[1];

				influence_set _theSet = control_point_influences[j];
				float sum = _theSet[0].weight + _theSet[1].weight + _theSet[2].weight + _theSet[3].weight;
				for (int k = 0; k < MAX_INFLUENCES; ++k)
				{
					vertices->data()[j].joints[k] = _theSet[k].joint;
					vertices->data()[j].weights[k] = _theSet[k].weight / sum;
				}
			}

			// expand vertices to match with all normals
			if (true)
			{
				// Get the Normals array from the mesh
				FbxArray<FbxVector4> normalsVec;
				mesh->GetPolygonVertexNormals(normalsVec);
				cout << "\nNormalVec Count:" << normalsVec.Size();

				//get all UV set names
				FbxStringList lUVSetNameList;
				mesh->GetUVSetNames(lUVSetNameList);
				const char* lUVSetName = lUVSetNameList.GetStringAt(0);
				const FbxGeometryElementUV* lUVElement = mesh->GetElementUV(lUVSetName);

				// Declare a new array for the new vertex array
				// Note the size is numIndices not numVertices
				SimpleVertex *vertices2 = new SimpleVertex[*numIndices];

				// align (expand) vertex array and set the normals
				for (int j = 0; j < *numIndices; j++)
				{
					vertices2[j].Pos[0] = vertices->data()[indices->data()[j]].Pos[0];
					vertices2[j].Pos[1] = vertices->data()[indices->data()[j]].Pos[1];
					vertices2[j].Pos[2] = vertices->data()[indices->data()[j]].Pos[2];
					vertices2[j].Normal[0] = normalsVec.GetAt(j)[0];
					vertices2[j].Normal[1] = normalsVec.GetAt(j)[1];
					vertices2[j].Normal[2] = normalsVec.GetAt(j)[2];

					vertices2[j].joints[0] = vertices->data()[indices->data()[j]].joints[0];
					vertices2[j].joints[1] = vertices->data()[indices->data()[j]].joints[1];
					vertices2[j].joints[2] = vertices->data()[indices->data()[j]].joints[2];
					vertices2[j].joints[3] = vertices->data()[indices->data()[j]].joints[3];

					vertices2[j].weights[0] = vertices->data()[indices->data()[j]].weights[0];
					vertices2[j].weights[1] = vertices->data()[indices->data()[j]].weights[1];
					vertices2[j].weights[2] = vertices->data()[indices->data()[j]].weights[2];
					vertices2[j].weights[3] = vertices->data()[indices->data()[j]].weights[3];

					if (lUVElement->GetReferenceMode() == FbxLayerElement::eDirect)
					{
						FbxVector2 lUVValue = lUVElement->GetDirectArray().GetAt(indices->data()[j]);

						vertices2[j].Tex[0] = lUVValue[0];
						vertices2[j].Tex[1] = 1.0f - lUVValue[1];
					}
					else if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						auto& index_array = lUVElement->GetIndexArray();

						FbxVector2 lUVValue = lUVElement->GetDirectArray().GetAt(index_array[j]);

						vertices2[j].Tex[0] = lUVValue[0];
						vertices2[j].Tex[1] = 1.0f - lUVValue[1];
					}

					if (false)
					{
						cout << "\nPos " << vertices2[j].Pos[0] << " " << vertices2[j].Pos[1] << " " << vertices2[j].Pos[2];
						cout << "\nNormal " << vertices2[j].Normal[0] << " " << vertices2[j].Normal[1] << " " << vertices2[j].Normal[2];
						cout << "\nTex " << vertices2[j].Tex[0] << " " << vertices2[j].Tex[1];
					}
				}

				// vertices is an "out" var so make sure it points to the new array
				// and clean up first array
				vertices->clear();
				vertices->resize(*numIndices);
				for (int i = 0; i < *numIndices; ++i)
					vertices->data()[i] = vertices2[i];

				// make new indices to match the new vertex(2) array
				indices->clear();
				for (int j = 0; j < *numIndices; j++)
					indices->push_back(j);

				if (true)
				{
					Compactify(vertices, indices, numIndices, numVertices);
				}
				else
				{
					// numVertices is an "out" var so set to new size
					*numVertices = *numIndices;
				}
			}
			//================= Texture ========================================

			int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();

			for (int index = 0; index < materialCount; index++)
			{
				FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(index);

				if (material != NULL)
				{
					//cout << "\nmaterial: " << material->GetName() << std::endl;
					// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
					FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

					// Check if it's layeredtextures
					int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

					if (layeredTextureCount > 0)
					{
						for (int j = 0; j < layeredTextureCount; j++)
						{
							FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
							int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

							for (int k = 0; k < lcount; k++)
							{
								FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
								// Then, you can get all the properties of the texture, include its name
								const char* textureName = texture->GetFileName();
								//cout << textureName;
							}
						}
					}
					else
					{
						// Directly get textures
						int textureCount = prop.GetSrcObjectCount<FbxTexture>();
						for (int j = 0; j < textureCount; j++)
						{
							FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(j));
							// Then, you can get all the properties of the texture, include its name
							const char* textureName = texture->GetFileName();
							cout << "\nTexture Filename " << textureName;

							FbxProperty p = texture->RootProperty.Find("Filename");

						}
					}
				}
			}
		}
		ProcessFbxMeshComplete(childNode, vertices, indices, numIndices, numVertices);
	}
}

void Compactify(std::vector<SimpleVertex>* vertices, std::vector<int>* indices, int* numIndices, int* numVertices)
{
	// Using vectors because we don't know what size we are
	// going to need until the end
	vector<SimpleVertex> vertexList;
	vector<int> indicesList;

	// copy first 3 verts (the first triangle) to start
	vertexList.push_back(vertices->data()[0]);
	vertexList.push_back(vertices->data()[1]);
	vertexList.push_back(vertices->data()[2]);

	// set first 3 indices (the first triangle) to start
	indicesList.push_back(0);
	indicesList.push_back(1);
	indicesList.push_back(2);

	// set running index to current end of array
	int compactedIndex = 2;

	float episilon = 0.001f;
	// for each vertex in the expanded array
	// compare to the compacted array for a matching
	// vertex, if found, skip adding and set the index
	for (int i = 3; i < vertices->size(); i++)
	{
		bool found = false;

		// search for match with the rest in the array
		for (int j = 0; j < vertexList.size(); j++)
		{
			SimpleVertex temp = vertices->data()[i];

			if (abs(temp.Pos[0] - vertexList[j].Pos[0]) < episilon &&
				abs(temp.Pos[1] - vertexList[j].Pos[1]) < episilon &&
				abs(temp.Pos[2] - vertexList[j].Pos[2]) < episilon &&
				abs(temp.Normal[0] - vertexList[j].Normal[0]) < episilon &&
				abs(temp.Normal[1] - vertexList[j].Normal[1]) < episilon &&
				abs(temp.Normal[2] - vertexList[j].Normal[2]) < episilon &&
				abs(temp.Tex[0] - vertexList[j].Tex[0]) < episilon &&
				abs(temp.Tex[1] - vertexList[j].Tex[1]) < episilon
				)
			{
				indicesList.push_back(j);
				found = true;
				break;
			}
		}
		// didn't find a duplicate so keep (push back) the current vertex
		// and increment the index count and push back that index as well
		if (!found)
		{
			vertexList.push_back(vertices->data()[i]);
			compactedIndex++;
			indicesList.push_back(compactedIndex);
		}
	}

	// print out some stats
	cout << "\nindex count BEFORE/AFTER compaction " << *numIndices;
	cout << "\nvertex count ORIGINAL (FBX source): " << *numVertices;
	cout << "\nvertex count AFTER expansion: " << *numIndices;
	cout << "\nvertex count AFTER compaction: " << vertexList.size();
	cout << "\nSize reduction: " << ((*numVertices - vertexList.size()) / (float)*numVertices)*100.00f << "%";
	cout << "\nor " << (vertexList.size() / (float)*numVertices) << " of the expanded size";

	// copy out indices

	indices->clear();
	indices->resize(indicesList.size());
	for (int i = 0; i < indicesList.size(); ++i)
		indices->data()[i] = indicesList[i];


	// copy out vertices

	vertices->clear();
	vertices->resize(vertexList.size());
	for (int i = 0; i < vertexList.size(); ++i)
		vertices->data()[i] = vertexList[i];
	// set numVertices to correct size
	*numVertices = vertexList.size();
}

void ConvertFBX(FileSet files)
{
#pragma region Setup

	FbxManager* manager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);

	FbxScene* scene = FbxScene::Create(manager, "");

	FbxImporter* importer = FbxImporter::Create(manager, "");

#pragma endregion

	if (files.animFile)
	{

#pragma region Animation


	importer->Initialize(".//Assets/Run.fbx", -1, manager->GetIOSettings());
	importer->Import(scene);

	int numPoses;
	numPoses = scene->GetPoseCount();
	FbxPose* _bindPose = nullptr;

	for (int i = 0; i < numPoses; ++i)
	{
		FbxPose* _currPose = scene->GetPose(i);

		if (_currPose->IsBindPose())
		{
			_bindPose = _currPose;
			break;
		}
	}

	int skelNodeCount;

	skelNodeCount = _bindPose->GetCount();

	FbxNode* rootNode = nullptr;

	vector<FbxJoint> myJointNodes;

	bool foundRoot = false;
	for (int i = 0; i < skelNodeCount; ++i)
	{
		FbxNode* _CurrNode = _bindPose->GetNode(i);

		FbxSkeleton* _nodeSkel = _CurrNode->GetSkeleton();

		if (_nodeSkel && _nodeSkel->IsSkeletonRoot())
		{
			rootNode = _CurrNode;
			foundRoot = true;
		}

		if (foundRoot)
			break;
	}

	FbxJoint rootJoint;
	rootJoint._node = rootNode;
	rootJoint.parentIndex = -1;

	myJointNodes.push_back(rootJoint);

	for (int j = 0; j < myJointNodes.size(); ++j)
	{
		int childCount = myJointNodes[j]._node->GetChildCount();
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* child = myJointNodes[j]._node->GetChild(i);
			if (child->GetSkeleton())
			{
				FbxJoint temp;
				temp._node = child;
				temp.parentIndex = j;
				myJointNodes.push_back(temp);
			}
		}
	}

	for (int i = 0; i < myJointNodes.size(); ++i)
	{
		joint _tempJoint;
		FbxMatrix _tempMat = myJointNodes[i]._node->EvaluateGlobalTransform();

		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				_tempJoint._mat[(j * 4) + k] = _tempMat.mData[j][k];
		_tempJoint.parent_index = i;
	}


	FbxAnimStack* _theStack = scene->GetCurrentAnimationStack();

	FbxTimeSpan _theTimeSpan = _theStack->GetLocalTimeSpan();
	FbxTime _time = _theTimeSpan.GetDuration();

	FbxLongLong _frameCount = _time.GetFrameCount(FbxTime::EMode::eFrames24);


	AnimClip _clip;
	_clip.duration = _time.GetSecondDouble();


	std::vector<joint> bindPose;


	_time.SetFrame(0);

	for (auto& _joint : myJointNodes)
	{
		FbxAMatrix _mat = _joint._node->EvaluateGlobalTransform(_time);

		joint __joint;
		__joint.parent_index = _joint.parentIndex;

		for (int x = 0; x < 4; ++x)
			for (int y = 0; y < 4; ++y)
				__joint._mat[x * 4 + y] = _mat.mData[x][y];

		bindPose.push_back(__joint);
	}


	for (int i = 1; i < _frameCount; ++i)
	{
		Keyframe _key;
		_time.SetFrame(i, fbxsdk::FbxTime::eFrames24);
		_key.time = _time.GetSecondDouble();


		for (auto& _joint : myJointNodes)
		{
			FbxAMatrix _mat = _joint._node->EvaluateGlobalTransform(_time);

			joint __joint;
			__joint.parent_index = _joint.parentIndex;

			for (int x = 0; x < 4; ++x)
				for (int y = 0; y < 4; ++y)
					__joint._mat[x * 4 + y] = _mat.mData[x][y];

			_key.joints.push_back(__joint);
		}

		_clip.frames.push_back(_key);
	}


	int count = _bindPose->GetCount();

	FbxNode* _bindNode = _bindPose->GetNode(0);

	FbxMesh* _bindMesh = _bindNode->GetMesh();

	int controlPointsCount = _bindMesh->GetControlPointsCount();

	control_point_influences.resize(controlPointsCount);

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

	int _clusterCount;

	_clusterCount = _skin->GetClusterCount();

	for (int i = 0; i < _clusterCount; ++i)
	{
		FbxCluster* _currClust = _skin->GetCluster(i);
		FbxNode* _link = _currClust->GetLink();
		int JOINT_INDEX = -1;

		for (int j = 0; j < myJointNodes.size(); ++j)
			if (myJointNodes[j]._node == _link)
				JOINT_INDEX = j;

		for (int j = 0; j < _currClust->GetControlPointIndicesCount(); ++j)
		{

			float WEIGHT = _currClust->GetControlPointWeights()[j];
			int CONTROL_POINT_INDEX = _currClust->GetControlPointIndices()[j];

			influence _temp;
			_temp.joint = JOINT_INDEX;
			_temp.weight = WEIGHT;

			influence_set* _theInfluenceSet = &control_point_influences[CONTROL_POINT_INDEX];

			bool stronger = false;
			for (int k = 0; k < MAX_INFLUENCES; ++k)
			{
				if (WEIGHT > _theInfluenceSet->at(k).weight)
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

				_theInfluenceSet->at(weakest) = _temp;
			}
		}
	}


	ofstream animfile("..\\EngineDevelopment\\Assets\\BattleMage.anim", ios::trunc | ios::binary | ios::out);
	assert(animfile.is_open());

	int numJ = bindPose.size();
	animfile.write((const char*)&numJ, sizeof(int));
	for (int i = 0; i < numJ; ++i)
	{
		animfile.write((const char*)&bindPose[i].parent_index, sizeof(int));
		animfile.write((const char*)bindPose[i]._mat, sizeof(float) * 16);
	}





	animfile.write((const char*)&_clip.duration, sizeof(double));
	int numFrames = _clip.frames.size();
	animfile.write((const char*)&numFrames, sizeof(int));

	for (auto& frame : _clip.frames)
	{
		animfile.write((const char*)&frame.time, sizeof(double));
		int numJoints = frame.joints.size();
		animfile.write((const char*)&numJoints, sizeof(int));

		for (int i = 0; i < frame.joints.size(); ++i)
		{
			animfile.write((const char*)&frame.joints[i].parent_index, sizeof(int));
			animfile.write((const char*)frame.joints[i]._mat, sizeof(float) * 16);
		}
	}
	animfile.close();

#pragma endregion
	}

#pragma region Mesh

	importer->Initialize(files.meshFile, -1, manager->GetIOSettings());

	importer->Import(scene);

	int numIndices = 0;
	int numVerts = 0;
	vector<SimpleVertex> vertices = {};
	vector<int> indices = {};

	ProcessFbxMeshComplete(scene->GetRootNode(), &vertices, &indices, &numIndices, &numVerts);

	std::string outMesh;
	outMesh.append(".//Assets//");
	outMesh.append(files.bastFile);
	outMesh.append(".mesh");

	ofstream file(outMesh.c_str() , std::ios::trunc | std::ios::binary | std::ios::out);

	assert(file.is_open());

	file.write((const char*)&numIndices, sizeof(int));

	file.write((const char*)indices.data(), sizeof(int) * numIndices);
	file.write((const char*)&numVerts, sizeof(int));

	file.write((const char*)vertices.data(), sizeof(SimpleVertex) * numVerts);
	file.close();

#pragma endregion

#pragma region Material

	int numMats = scene->GetMaterialCount();

	vector<material_t> materials;
	vector<file_path_t> paths;

	for (int i = 0; i < numMats; ++i)
	{
		material_t tempMat;
		FbxSurfaceMaterial* mat = scene->GetMaterial(i);

		if (mat->Is<FbxSurfaceLambert>() == false)
			continue;

		FbxSurfaceLambert* lam = (FbxSurfaceLambert*)mat;


		FbxDouble3 diffuse_color = lam->Diffuse.Get();
		FbxDouble diffuse_factor = lam->DiffuseFactor.Get();

		tempMat[material_t::COMPONENT::DIFFUSE].value[0] = diffuse_color.mData[0];
		tempMat[material_t::COMPONENT::DIFFUSE].value[1] = diffuse_color.mData[1];
		tempMat[material_t::COMPONENT::DIFFUSE].value[2] = diffuse_color.mData[2];

		tempMat[material_t::COMPONENT::DIFFUSE].factor = diffuse_factor;

		if (FbxFileTexture* file_tex = lam->Diffuse.GetSrcObject<FbxFileTexture>())
		{
			const char* file_name = file_tex->GetRelativeFileName();
			file_path_t file_path;
			strcpy(file_path.data(), file_name);
			tempMat[material_t::COMPONENT::DIFFUSE].input = paths.size();
			paths.push_back(file_path);
		}

		FbxDouble3 emissive_color = lam->Emissive.Get();
		FbxDouble emissive_factor = lam->EmissiveFactor.Get();

		tempMat[material_t::COMPONENT::EMISSIVE].value[0] = emissive_color.mData[0];
		tempMat[material_t::COMPONENT::EMISSIVE].value[1] = emissive_color.mData[1];
		tempMat[material_t::COMPONENT::EMISSIVE].value[2] = emissive_color.mData[2];

		tempMat[material_t::COMPONENT::EMISSIVE].factor = emissive_factor;

		if (FbxFileTexture* file_tex = lam->Emissive.GetSrcObject<FbxFileTexture>())
		{
			const char* file_name = file_tex->GetRelativeFileName();
			file_path_t file_path;
			strcpy(file_path.data(), file_name);
			tempMat[material_t::COMPONENT::EMISSIVE].input = paths.size();
			paths.push_back(file_path);
		}


		if (mat->Is<FbxSurfacePhong>())
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong*)mat;

			FbxDouble3 specular_color = phong->Specular.Get();
			FbxDouble specular_factor = phong->SpecularFactor.Get();

			tempMat[material_t::COMPONENT::SPECULAR].value[0] = specular_color.mData[0];
			tempMat[material_t::COMPONENT::SPECULAR].value[1] = specular_color.mData[1];
			tempMat[material_t::COMPONENT::SPECULAR].value[2] = specular_color.mData[2];

			tempMat[material_t::COMPONENT::SPECULAR].factor = specular_factor;

			if (FbxFileTexture* file_tex = phong->Specular.GetSrcObject <FbxFileTexture>())
			{
				const char* file_name = file_tex->GetRelativeFileName();
				file_path_t file_path;
				strcpy(file_path.data(), file_name);
				tempMat[material_t::COMPONENT::SPECULAR].input = paths.size();
				paths.push_back(file_path);
			}

			FbxDouble shininess = phong->Shininess.Get();

			tempMat[material_t::COMPONENT::SHININESS].factor = shininess;

			if (FbxFileTexture* file_tex = phong->Shininess.GetSrcObject<FbxFileTexture>())
			{
				const char* file_name = file_tex->GetRelativeFileName();
				file_path_t file_path;
				strcpy(file_path.data(), file_name);
				tempMat[material_t::COMPONENT::SHININESS].input = paths.size();
				paths.push_back(file_path);
			}
			materials.push_back(tempMat);
		}
	}

	std::string outMat;
	outMat.append(".//Assets//");
	outMat.append(files.bastFile);
	outMat.append(".mat");

	ofstream matfile(outMat.c_str(), ios::trunc | ios::binary | ios::out);
	assert(matfile.is_open());

	if (!matfile.is_open())
		cout << "Failed to open file to write mat" << endl;

	int outnumMats = materials.size();

	matfile.write((const char*)&outnumMats, sizeof(int));

	for (int i = 0; i < materials.size(); ++i)
	{
		matfile.write((const char*)&materials[i][material_t::COMPONENT::DIFFUSE].value, sizeof(float) * 3);
		matfile.write((const char*)&materials[i][material_t::COMPONENT::DIFFUSE].factor, sizeof(float));
		matfile.write((const char*)&materials[i][material_t::COMPONENT::DIFFUSE].input, sizeof(int64_t));

		matfile.write((const char*)&materials[i][material_t::COMPONENT::EMISSIVE].value, sizeof(float) * 3);
		matfile.write((const char*)&materials[i][material_t::COMPONENT::EMISSIVE].factor, sizeof(float));
		matfile.write((const char*)&materials[i][material_t::COMPONENT::EMISSIVE].input, sizeof(int64_t));

		matfile.write((const char*)&materials[i][material_t::COMPONENT::SPECULAR].value, sizeof(float) * 3);
		matfile.write((const char*)&materials[i][material_t::COMPONENT::SPECULAR].factor, sizeof(float));
		matfile.write((const char*)&materials[i][material_t::COMPONENT::SPECULAR].input, sizeof(int64_t));

		matfile.write((const char*)&materials[i][material_t::COMPONENT::SHININESS].value, sizeof(float) * 3);
		matfile.write((const char*)&materials[i][material_t::COMPONENT::SHININESS].factor, sizeof(float));
		matfile.write((const char*)&materials[i][material_t::COMPONENT::SHININESS].input, sizeof(int64_t));
	}

	int outNumPaths = paths.size();
	matfile.write((const char*)&outNumPaths, sizeof(int));

	for (int i = 0; i < paths.size(); i++)
		matfile.write(paths[i].data(), sizeof(char) * 260);

	matfile.close();

#pragma endregion

}

void main()
{
	FileSet fileset;

	fileset.meshFile = ".//Assets//black-handbomb.fbx";
	fileset.bastFile = "Bomb";

	ConvertFBX(fileset);

	system("pause");
}