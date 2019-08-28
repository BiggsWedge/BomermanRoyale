#include "Utilities.h"

using namespace std;
using namespace GW;
using namespace SYSTEM;
using namespace AUDIO;

constexpr size_t MAX_LINE_VERTS = 4096;

size_t line_vert_count = 0;
std::array< TLineVertex, MAX_LINE_VERTS> line_verts;


GLog* g_pLogger = nullptr;
GWindow* g_pWindow = nullptr;
GInput* g_pInputRecord = nullptr;
GAudio* g_pAudioHolder = nullptr;
GMusic* g_pMusicStream = nullptr;
GSound* g_pSoundPlayer = nullptr;
GSound* walkSound1;
GSound* bombPlaceSound1;
GSound* spawnSound1;
GSound* bombPlaceSound2;
GSound* spawnSound2;
GSound* warnSound;
GSound* fallingSound;
GSound* playerfallingSound;
GSound* WinScreenSound;

std::vector<GSound*> MenuSounds;
std::vector<GSound*> explosionSound;
std::vector<GSound*> bombPlaceSound;
std::vector<GSound*> powerUpSound;
GController* g_pControllerInput = nullptr;

std::vector<TMeshTemplate> v_tMeshTemplates = {};

string GetCurrentDateAndTime()
{
	string currDateTime;
	time_t now = time(0);
	tm lmt;
	localtime_s(&lmt, &now);

	currDateTime.clear();
	currDateTime.append(std::to_string(1 + lmt.tm_mon));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_mday));
	currDateTime.append(".");
	currDateTime.append(std::to_string(1900 + lmt.tm_year));
	currDateTime.append("_");
	currDateTime.append(std::to_string(lmt.tm_hour));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_min));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_sec));

	return currDateTime;
}

bool InitializeLogger()
{
	string cOutFileName;
	cOutFileName.append("ErrorLog");
	cOutFileName.append(GetCurrentDateAndTime());
	cOutFileName.append(".log");

	if (G_SUCCESS(CreateGLog(cOutFileName.c_str(), &g_pLogger)))
	{
#ifdef _DEBUG
		g_pLogger->EnableConsoleLogging(true);
#else
		g_pLogger->EnableConsoleLogging(false);
#endif // !_DEBUG

		g_pLogger->EnableVerboseLogging(true);
		g_pLogger->LogCatergorized("SUCCESS", "Error Logging system successfully initialized.");
		return true;
	}

	return false;
}

bool InitializeWindow() {
	if (G_SUCCESS(CreateGWindow(100, 100, 1024, 720, GWindowStyle::WINDOWEDBORDERED, &g_pWindow))) {
		g_pLogger->LogCatergorized("SUCCESS", "Window successfully created.");
		return true;
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Window unsuccessfully created.");
		return false;
	}
}

bool InitializeInput()
{
	if (G_SUCCESS(CreateGInput(g_pWindow, sizeof(g_pWindow), &g_pInputRecord))) {
		g_pLogger->LogCatergorized("SUCCESS", "Input Manager successfully created.");
		return true;
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Input Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeControllerInput() {
	if (G_SUCCESS(CreateGController(G_XBOX_CONTROLLER, &g_pControllerInput)))
	{
		g_pLogger->LogCatergorized("SUCCESS", "Controller Input Manager successfully created.");
		return true;
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Controller Input Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeAudio() {
	if (G_SUCCESS(CreateGAudio(&g_pAudioHolder))) {
		if (G_SUCCESS(g_pAudioHolder->Init(2))) {
			g_pLogger->LogCatergorized("SUCCESS", "Audio Manager successfully created.");
			return true;
		}
		else
		{
			g_pLogger->LogCatergorized("FAILURE", "Audio Manager unsuccessfully created.");
			return false;
		}

	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Audio Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeGlobals()
{
	if (!InitializeLogger())
		return false;
	if (!InitializeWindow())
		return false;
	if (!InitializeInput())
		return false;
	if (!InitializeAudio())
		return false;
	if (!InitializeControllerInput())
		return false;
	g_d3dData = new DirectXData();
	if (!g_d3dData->Initialize())
		return false;

	return true;
}

void LoadModels()
{
	for (int i = 0; i < modelLoadInfos.size(); ++i)
		LoadModel(modelLoadInfos[i]);
}

void LoadModel(TMeshLoadInfo loadInfo)
{
	int index = v_tMeshTemplates.size();
	int numIndices;
	int numVerts;

	TMeshTemplate temp/* = TMeshTemplate()*/;
	temp.uID = index;
	temp.sName = loadInfo.name;

	using namespace DirectX;
	if (loadInfo.meshFile)
	{
		std::fstream file{ loadInfo.meshFile, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		file.read((char*)&numIndices, sizeof(int));
		temp.v_iIndices.resize(numIndices);
		file.read((char*)temp.v_iIndices.data(), sizeof(int) * numIndices);

		file.read((char*)&numVerts, sizeof(int));
		temp.v_tVertices.resize(numVerts);
		file.read((char*)temp.v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);

		file.close();

		if (loadInfo.name == "BattleMage")
		{
			for (auto& v : temp.v_tVertices)
			{
				v.fPosition.x = v.fPosition.x;
			}
		}
		else
		{
			for (auto& v : temp.v_tVertices)
			{
				v.fPosition.x = -v.fPosition.x;
				v.fNormal.x = -v.fNormal.x;
			}
		}
	}

	if (loadInfo.matFile)
	{
		std::fstream inMatFile{ loadInfo.matFile, std::ios_base::in | std::ios_base::binary };

		assert(inMatFile.is_open());

		if (!inMatFile.is_open())
		{
			assert(false);
			return;
		}

		int numMatFiles;
		inMatFile.read((char*)&numMatFiles, sizeof(int));


		for (int i = 0; i < numMatFiles; ++i)
		{
			material_t tempmat;
			for (int j = 0; j < 4; ++j)
			{
				XMVECTOR temp2;
				inMatFile.read((char*)&tempmat[j].value, sizeof(float) * 3);
				inMatFile.read((char*)&tempmat[j].factor, sizeof(float));
				inMatFile.read((char*)&tempmat[j].input, sizeof(int64_t));
				if (j == material_t::COMPONENT::DIFFUSE)
				{
					temp2.m128_f32[0] = tempmat[j].value[0];
					temp2.m128_f32[1] = tempmat[j].value[1];
					temp2.m128_f32[2] = tempmat[j].value[2];
					XMStoreFloat3(&temp._mat.fSurfaceDiffuse, temp2);
					temp._mat.fDiffuseFactor = tempmat[j].factor;
				}
				if (j == material_t::COMPONENT::EMISSIVE)
				{
					temp2.m128_f32[0] = tempmat[j].value[0];
					temp2.m128_f32[1] = tempmat[j].value[1];
					temp2.m128_f32[2] = tempmat[j].value[2];
					XMStoreFloat3(&temp._mat.fSurfaceEmissive, temp2);
					temp._mat.fEmissiveFactor = tempmat[j].factor;
				}
				if (j == material_t::COMPONENT::SPECULAR)
				{
					temp2.m128_f32[0] = tempmat[j].value[0];
					temp2.m128_f32[1] = tempmat[j].value[1];
					temp2.m128_f32[2] = tempmat[j].value[2];
					XMStoreFloat3(&temp._mat.fSurfaceSpecular, temp2);
					temp._mat.fSpecularFactor = tempmat[j].factor;
				}
				if (j == material_t::COMPONENT::SHININESS)
				{
					temp2.m128_f32[0] = tempmat[j].value[0];
					temp2.m128_f32[1] = tempmat[j].value[1];
					temp2.m128_f32[2] = tempmat[j].value[2];
					XMStoreFloat3(&temp._mat.fSurfaceShiny, temp2);
					temp._mat.fShinyFactor = tempmat[j].factor;
				}
			}
			temp.mats.push_back(tempmat);
		}

		int numPathsFiles;
		inMatFile.read((char*)&numPathsFiles, sizeof(int));

		for (int i = 0; i < numPathsFiles; ++i)
		{
			file_path_t _file;
			inMatFile.read(_file.data(), sizeof(char) * 260);
			temp.filePaths.push_back(_file);
		}
		inMatFile.close();

	}
	if (loadInfo.animFile)
	{
		std::fstream inAnimFile(loadInfo.animFile, std::ios_base::in | std::ios_base::binary);

		assert(inAnimFile.is_open());

		int num;
		inAnimFile.read((char*)&num, sizeof(int));
		temp._bindPose.resize(num);
		for (int i = 0; i < num; ++i)
		{
			inAnimFile.read((char*)&temp._bindPose[i].parentIndex, sizeof(int));
			DirectX::XMFLOAT4X4 mat;
			inAnimFile.read((char*)mat.m, sizeof(float) * 16);
			temp._bindPose[i]._mat = DirectX::XMLoadFloat4x4(&mat);
		}




		double _dur;
		inAnimFile.read((char*)&_dur, sizeof(double));
		temp._anim.duration = _dur;
		int numFrames;
		inAnimFile.read((char*)&numFrames, sizeof(int));

		temp._anim.frames.resize(numFrames);
		for (int i = 0; i < numFrames; ++i)
		{
			KeyFrame _key;
			inAnimFile.read((char*)&_key.time, sizeof(double));
			int numJoints;
			inAnimFile.read((char*)&numJoints, sizeof(int));
			_key.joints.resize(numJoints);

			for (int j = 0; j < numJoints; ++j)
			{
				inAnimFile.read((char*)&_key.joints[j].parentIndex, sizeof(int));
				DirectX::XMFLOAT4X4 _j;
				inAnimFile.read((char*)_j.m, sizeof(float) * 16);
				_key.joints[j]._mat = DirectX::XMLoadFloat4x4(&_j);
			}
			temp._anim.frames[i] = _key;
		}
		inAnimFile.close();
		temp._anim.frames.push_back(temp._anim.frames[0]);
		temp._anim.frames[temp._anim.frames.size() - 1].time = temp._anim.duration;
	}

	v_tMeshTemplates.push_back(temp);

	//fstream file{ loadInfo.meshFile, ios::in | ios::binary };
	//
	//if (!file.is_open())
	//{
	//	std::string fail = "Failed to load " + loadInfo.name;
	//	g_pLogger->LogCatergorized("FAILURE", fail.c_str());
	//	return;
	//}
	//
	//file.read((char*)&numIndices, sizeof(int));
	//temp.v_iIndices.resize(numIndices);
	//file.read((char*)temp.v_iIndices.data(), sizeof(int) * numIndices);
	//
	//file.read((char*)&numVerts, sizeof(int));
	//temp.v_tVertices.resize(numVerts);
	//file.read((char*)temp.v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);
	//
	//file.close();
	//
	//for (auto& v : temp.v_tVertices)
	//{
	//	v.fPosition.x = -v.fPosition.x;
	//	v.fNormal.x = -v.fNormal.x;
	//}
}

void LoadMenuScreen(int width, int height, int numbuttons, const char* matFile) {
	int index = v_tMeshTemplates.size();
	int numIndices = 6;
	int numVerts = 4;
	float SSpaceWidth = (float)width * 0.5f;
	float SSpaceHeight = (float)height * 0.5f;

	TMeshTemplate temp/* = TMeshTemplate()*/;

	temp.uID = index;
	temp.v_iIndices.resize(numIndices);
	temp.v_tVertices.resize(numVerts);
	temp.v_tVertices.at(0).fPosition = DirectX::XMFLOAT3(-(SSpaceWidth), -(SSpaceHeight), 0.0f);
	temp.v_tVertices.at(1).fPosition = DirectX::XMFLOAT3((SSpaceWidth), -(SSpaceHeight), 0.0f);
	temp.v_tVertices.at(2).fPosition = DirectX::XMFLOAT3(-(SSpaceWidth), (SSpaceHeight), 0.0f);
	temp.v_tVertices.at(3).fPosition = DirectX::XMFLOAT3((SSpaceWidth), (SSpaceHeight), 0.0f);

	temp.v_tVertices.at(0).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	temp.v_tVertices.at(1).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	temp.v_tVertices.at(2).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	temp.v_tVertices.at(3).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	temp.v_tVertices.at(0).fTexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
	temp.v_tVertices.at(1).fTexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
	temp.v_tVertices.at(2).fTexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
	temp.v_tVertices.at(3).fTexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);

	temp.v_iIndices.at(0) = 0;
	temp.v_iIndices.at(1) = 2;
	temp.v_iIndices.at(2) = 1;
	temp.v_iIndices.at(3) = 1;
	temp.v_iIndices.at(4) = 2;
	temp.v_iIndices.at(5) = 3;

	v_tMeshTemplates.push_back(temp);

	for (int i = 0; i < numbuttons; ++i) {
		float bWidth = width / 3.0f;
		float bHeight = height / 10.0f;
		bWidth = bWidth / 2.0f;
		bHeight = bHeight / 2.0f;

		temp.uID = index;
		temp.v_iIndices.resize(numIndices);
		temp.v_tVertices.resize(numVerts);

		temp.v_tVertices.at(0).fPosition = DirectX::XMFLOAT3(-(bWidth), -(bHeight), 0.0f);
		temp.v_tVertices.at(1).fPosition = DirectX::XMFLOAT3((bWidth), -(bHeight), 0.0f);
		temp.v_tVertices.at(2).fPosition = DirectX::XMFLOAT3(-(bWidth), (bHeight), 0.0f);
		temp.v_tVertices.at(3).fPosition = DirectX::XMFLOAT3((bWidth), (bHeight), 0.0f);

		temp.v_tVertices.at(0).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		temp.v_tVertices.at(1).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		temp.v_tVertices.at(2).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		temp.v_tVertices.at(3).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

		temp.v_tVertices.at(0).fTexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
		temp.v_tVertices.at(1).fTexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
		temp.v_tVertices.at(2).fTexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
		temp.v_tVertices.at(3).fTexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);

		temp.v_iIndices.at(0) = 0;
		temp.v_iIndices.at(1) = 2;
		temp.v_iIndices.at(2) = 1;
		temp.v_iIndices.at(3) = 1;
		temp.v_iIndices.at(4) = 2;
		temp.v_iIndices.at(5) = 3;

		v_tMeshTemplates.push_back(temp);
	}
	temp._vertexBuffer = nullptr;
	temp._indexBuffer = nullptr;
}

void LoadTextures()
{
	for (int i = 0; i < (int)diffuseTextures.size(); ++i)
	{
		if (FAILED(DirectX::CreateWICTextureFromFile(g_d3dData->d3dDevice, diffuseTextures[i], nullptr, &g_d3dData->d3dDiffuseTextures[i])))
			g_pLogger->LogCatergorized("FAILURE", "Failed to load texture");
	}
}

void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b) {
	line_verts[line_vert_count].pos = point_a;
	line_verts[line_vert_count].color = color_a;
	line_vert_count += 1;

	line_verts[line_vert_count].pos = point_b;
	line_verts[line_vert_count].color = color_b;
	line_vert_count += 1;
}

void clear_lines()
{
	line_vert_count = 0;
}

const TLineVertex* get_line_verts()
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

void drawAABB(float3 point_a, float3 point_b, float3 point_c, float3 point_d, float3 point_e, float3 point_f, float3 point_g, float3 point_h, float4 color1, float4 color2)
{
	add_line(point_a, point_b, color1, color2);
	add_line(point_b, point_c, color1, color2);
	add_line(point_c, point_d, color1, color2);
	add_line(point_d, point_a, color1, color2);
	add_line(point_e, point_f, color1, color2);
	add_line(point_f, point_g, color1, color2);
	add_line(point_g, point_h, color1, color2);
	add_line(point_h, point_e, color1, color2);
	add_line(point_a, point_e, color1, color2);
	add_line(point_b, point_f, color1, color2);
	add_line(point_c, point_g, color1, color2);
	add_line(point_d, point_h, color1, color2);
}

void drawAABB(DirectX::XMFLOAT3 tlf, DirectX::XMFLOAT3 tlb, DirectX::XMFLOAT3 trf, DirectX::XMFLOAT3 trb, DirectX::XMFLOAT3 blf, DirectX::XMFLOAT3 brf, DirectX::XMFLOAT3 blb, DirectX::XMFLOAT3 brb, DirectX::XMFLOAT4 color)
{
	add_line(tlf, tlb, color);
	add_line(tlf, trf, color);
	add_line(tlf, blf, color);

	add_line(brb, trb, color);
	add_line(brb, blb, color);
	add_line(brb, brf, color);
}

float3 XMVector2Float3(DirectX::XMVECTOR vector)
{
	float3 point;

	point.x = DirectX::XMVectorGetX(vector);
	point.y = DirectX::XMVectorGetY(vector);
	point.z = DirectX::XMVectorGetZ(vector);

	return point;
}

DirectX::XMVECTOR Float32XMVector(float3 point)
{
	DirectX::XMVECTOR vector = { point.x, point.y, point.z, 1.0f };

	return vector;
}

void GetCorners(float3 _center, float3 _extents, float3*& corners)
{
	float newX, newY, newZ;

	newX = _center.x - _extents.x;
	newY = _center.y - _extents.y;
	newZ = _center.z - _extents.z;

	corners[0] = { newX,newY,newZ };

	newX = _center.x + _extents.x;
	newY = _center.y + _extents.y;
	newZ = _center.z + _extents.z;

	corners[1] = { newX,newY,newZ };

	corners[2] = { corners[0].x,corners[0].y,corners[1].z };
	corners[3] = { corners[0].x,corners[1].y,corners[0].z };
	corners[4] = { corners[1].x,corners[0].y,corners[0].z };
	corners[5] = { corners[0].x,corners[1].y,corners[1].z };
	corners[6] = { corners[1].x,corners[0].y,corners[1].z };
	corners[7] = { corners[1].x,corners[1].y,corners[0].z };
}

void TMeshTemplate::loadModel(const char* modelFile, const char* matFile, const char* animFile)
{
	using namespace DirectX;
	if (modelFile)
	{
		std::fstream file{ modelFile, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		file.read((char*)&numIndices, sizeof(int));
		v_iIndices.resize(numIndices);
		file.read((char*)v_iIndices.data(), sizeof(int) * numIndices);

		file.read((char*)&numVerts, sizeof(int));
		v_tVertices.resize(numVerts);
		file.read((char*)v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);

		file.close();


		for (auto& v : v_tVertices)
		{
			v.fPosition.x = v.fPosition.x;
		}
	}

	if (matFile)
	{
		std::fstream inMatFile{ matFile, std::ios_base::in | std::ios_base::binary };

		assert(inMatFile.is_open());

		if (!inMatFile.is_open())
		{
			assert(false);
			return;
		}

		int numMatFiles;
		inMatFile.read((char*)&numMatFiles, sizeof(int));


		for (int i = 0; i < numMatFiles; ++i)
		{
			material_t temp;
			for (int j = 0; j < 4; ++j)
			{
				XMVECTOR temp2;
				inMatFile.read((char*)&temp[j].value, sizeof(float) * 3);
				inMatFile.read((char*)&temp[j].factor, sizeof(float));
				inMatFile.read((char*)&temp[j].input, sizeof(int64_t));
				if (j == material_t::COMPONENT::DIFFUSE)
				{
					temp2.m128_f32[0] = temp[j].value[0];
					temp2.m128_f32[1] = temp[j].value[1];
					temp2.m128_f32[2] = temp[j].value[2];
					XMStoreFloat3(&_mat.fSurfaceDiffuse, temp2);
					_mat.fDiffuseFactor = temp[j].factor;
				}
				if (j == material_t::COMPONENT::EMISSIVE)
				{
					temp2.m128_f32[0] = temp[j].value[0];
					temp2.m128_f32[1] = temp[j].value[1];
					temp2.m128_f32[2] = temp[j].value[2];
					XMStoreFloat3(&_mat.fSurfaceEmissive, temp2);
					_mat.fEmissiveFactor = temp[j].factor;
				}
				if (j == material_t::COMPONENT::SPECULAR)
				{
					temp2.m128_f32[0] = temp[j].value[0];
					temp2.m128_f32[1] = temp[j].value[1];
					temp2.m128_f32[2] = temp[j].value[2];
					XMStoreFloat3(&_mat.fSurfaceSpecular, temp2);
					_mat.fSpecularFactor = temp[j].factor;
				}
				if (j == material_t::COMPONENT::SHININESS)
				{
					temp2.m128_f32[0] = temp[j].value[0];
					temp2.m128_f32[1] = temp[j].value[1];
					temp2.m128_f32[2] = temp[j].value[2];
					XMStoreFloat3(&_mat.fSurfaceShiny, temp2);
					_mat.fShinyFactor = temp[j].factor;
				}
			}
			mats.push_back(temp);
		}

		int numPathsFiles;
		inMatFile.read((char*)&numPathsFiles, sizeof(int));

		for (int i = 0; i < numPathsFiles; ++i)
		{
			file_path_t _file;
			inMatFile.read(_file.data(), sizeof(char) * 260);
			filePaths.push_back(_file);
		}
		inMatFile.close();

	}
	if (animFile)
	{
		std::fstream inAnimFile(animFile, std::ios_base::in | std::ios_base::binary);

		assert(inAnimFile.is_open());

		int num;
		inAnimFile.read((char*)&num, sizeof(int));
		_bindPose.resize(num);
		for (int i = 0; i < num; ++i)
		{
			inAnimFile.read((char*)&_bindPose[i].parentIndex, sizeof(int));
			DirectX::XMFLOAT4X4 mat;
			inAnimFile.read((char*)mat.m, sizeof(float) * 16);
			_bindPose[i]._mat = DirectX::XMLoadFloat4x4(&mat);
		}




		double _dur;
		inAnimFile.read((char*)&_dur, sizeof(double));
		_anim.duration = _dur;
		int numFrames;
		inAnimFile.read((char*)&numFrames, sizeof(int));

		_anim.frames.resize(numFrames);
		for (int i = 0; i < numFrames; ++i)
		{
			KeyFrame _key;
			inAnimFile.read((char*)&_key.time, sizeof(double));
			int numJoints;
			inAnimFile.read((char*)&numJoints, sizeof(int));
			_key.joints.resize(numJoints);

			for (int j = 0; j < numJoints; ++j)
			{
				inAnimFile.read((char*)&_key.joints[j].parentIndex, sizeof(int));
				DirectX::XMFLOAT4X4 _j;
				inAnimFile.read((char*)_j.m, sizeof(float) * 16);
				_key.joints[j]._mat = DirectX::XMLoadFloat4x4(&_j);
			}
			_anim.frames[i] = _key;
		}
		inAnimFile.close();
		_anim.frames.push_back(_anim.frames[0]);
		_anim.frames[_anim.frames.size() - 1].time = _anim.duration;
	}

}

void TMeshTemplate::initialize(ID3D11Device* _device)
{
	D3D11_BUFFER_DESC vertBufferDesc;
	D3D11_SUBRESOURCE_DATA vertBuffSubResc;

	ZeroMemory(&vertBufferDesc, sizeof(vertBufferDesc));
	ZeroMemory(&vertBuffSubResc, sizeof(vertBuffSubResc));

	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.ByteWidth = sizeof(TSimpleVertex) * v_tVertices.size();
	vertBufferDesc.CPUAccessFlags = 0;
	vertBufferDesc.MiscFlags = 0;
	vertBufferDesc.StructureByteStride = 0;
	vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	vertBuffSubResc.pSysMem = v_tVertices.data();

	_device->CreateBuffer(&vertBufferDesc, &vertBuffSubResc, &_vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexBufferSubResourceData;

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	ZeroMemory(&indexBufferSubResourceData, sizeof(indexBufferSubResourceData));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * v_iIndices.size();
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	indexBufferSubResourceData.pSysMem = v_iIndices.data();

	_device->CreateBuffer(&indexBufferDesc, &indexBufferSubResourceData, &_indexBuffer);


	for (int i = 0; i < mats.size(); ++i)
	{
		for (int j = 0; j < material_t::COMPONENT::COUNT; ++j)
		{
			if (mats[i][j].input < 0)
				continue;
			std::experimental::filesystem::path filePath;
			filePath = filePaths[mats[i][j].input].data();
			HRESULT result = DirectX::CreateWICTextureFromFile(_device, filePath.wstring().c_str(), nullptr, &_srv[j]);
			if (!SUCCEEDED(result))
			{
				std::string fail = "Failed to make texture! " + this->sName;
				g_pLogger->LogCatergorized("FAILURE", fail.c_str());
			}
		}
	}

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	_device->CreateSamplerState(&sampDesc, &_samState);

}

void TMeshTemplate::render(ID3D11DeviceContext* _context, double timepassed)
{

	animTime += timepassed;
	totalTime += timepassed;

	float fractionalTime = timepassed - (int)timepassed;

	UINT strides = sizeof(TSimpleVertex);
	UINT offsets = 0;

	if (currKeyFrame == _anim.frames.size())
		currKeyFrame = 0;

	while (animTime > _anim.frames[currFrameIndex + 1].time)
	{
		currFrameIndex++;
		if (currFrameIndex == _anim.frames.size() - 1)
		{
			animTime -= _anim.duration;
			currFrameIndex = 0;
		}
	}

	float startTime, endTime;
	startTime = _anim.frames[currFrameIndex].time;
	endTime = _anim.frames[currFrameIndex + 1].time;

	float ratio = (animTime - startTime) / (endTime - startTime);

	KeyFrame _key = _anim.frames[currFrameIndex];

	jointCB _jointsConst;
	_jointsConst.numJoints = _anim.frames[currFrameIndex].joints.size();

	for (int i = 0; i < _anim.frames[currFrameIndex].joints.size(); ++i)
	{

		DirectX::XMMATRIX tween = matLerp(_anim.frames[currFrameIndex].joints[i]._mat, _anim.frames[currFrameIndex + 1].joints[i]._mat, ratio);

		//debug_renderer::drawMatrix(tween);

		_jointsConst._joints[i] = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, _bindPose[i]._mat) * tween);

		if (_key.joints[i].parentIndex >= 0)
		{
			DirectX::XMFLOAT4X4 startMat, endMat;

			DirectX::XMStoreFloat4x4(&startMat, matLerp(_anim.frames[currFrameIndex].joints[i]._mat, _anim.frames[currFrameIndex + 1].joints[i]._mat, ratio));
			DirectX::XMStoreFloat4x4(&endMat, matLerp(_anim.frames[currFrameIndex].joints[_anim.frames[currFrameIndex].joints[i].parentIndex]._mat, _anim.frames[currFrameIndex + 1].joints[_anim.frames[currFrameIndex + 1].joints[i].parentIndex]._mat, ratio));
		}


	}

	MVP_t debugConstBuff;
	//debugConstBuff.world = DirectX::XMMatrixTranspose(mObjMatrix);
	//
	//if (g_d3dData->bUseDebugRenderCamera)
	//	debugConstBuff.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
	//else
	//	debugConstBuff.view = DirectX::XMMatrixTranspose(g_d3dData->viewMat);
	//
	//debugConstBuff.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);

#pragma endregion

	Light pointLight;
	pointLight.position = DirectX::XMFLOAT3(0.0, 6.0f, -2.0f);
	pointLight.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	pointLight.intensity = 0.25f;
	pointLight.type = 0;

	LightBuffer _light;
	_light.light = pointLight;

	//MatBuffer mat;
	//mat.material = _mat;



	debugConstBuff.world = DirectX::XMMatrixIdentity();
	debugConstBuff.world = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180));
	debugConstBuff.world = debugConstBuff.world * DirectX::XMMatrixTranslation(6.0f, 0.0f, 0.0f);
	debugConstBuff.world = DirectX::XMMatrixTranspose(debugConstBuff.world);

	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MVP_t], 0, nullptr, &debugConstBuff, 0, 0);

	_context->IASetInputLayout(g_d3dData->d3dInputLayout[INPUT_LAYOUT::BASIC]);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::LIGHTS], 0, nullptr, &_light, 0, 0);
	_context->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::LIGHTS]);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MATERIAL], 0, nullptr, &_mat, 0, 0);
	_context->PSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MATERIAL]);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS], 0, nullptr, &_jointsConst, 0, 0);
	_context->VSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS]);

	_context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &strides, &offsets);

	_context->VSSetShader(g_d3dData->d3dVertexShader[VERTEX_SHADER::ANIM], 0, 0);
	_context->PSSetShader(g_d3dData->d3dPixelShader[PIXEL_SHADER::ANIM], 0, 0);
	_context->PSSetSamplers(0, 1, &_samState);
	_context->PSSetShaderResources(0, 1, &_srv[DIFFUSE]);
	_context->PSSetShaderResources(1, 1, &_srv[EMISSIVE]);
	_context->PSSetShaderResources(2, 1, &_srv[SPECULAR]);

	_context->DrawIndexed(numIndices, 0, 0);
}

//TMeshTemplate::~TMeshTemplate()
//{
//	SAFE_RELEASE(_vertexBuffer);
//	SAFE_RELEASE(_indexBuffer);
//	SAFE_RELEASE(_samState);
//	for (int i = 0; i < TEXTURES::COUNT; i++)
//	{
//		SAFE_RELEASE(_srv[i]);
//		SAFE_RELEASE(_textures[i]);
//	}
//
//
//}
//
//TMeshTemplate::TMeshTemplate()
//{
//	_vertexBuffer = nullptr;
//	_indexBuffer = nullptr;
//	_samState = nullptr;
//	for (int i = 0; i < TEXTURES::COUNT; i++)
//	{
//		_srv[i] = nullptr;
//		_textures[i] = nullptr;
//
//	}
//}

float lerp(float x, float y, float ratio)
{
	return (y - x) * ratio + x;

}

DirectX::XMMATRIX matLerp(DirectX::XMMATRIX x, DirectX::XMMATRIX y, float ratio)
{
	DirectX::XMMATRIX toRet = x;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			toRet.r[i].m128_f32[j] = lerp(x.r[i].m128_f32[j], y.r[i].m128_f32[j], ratio);
		}
	}
	/*
	for (int i = 0; i < 3; ++i)
		toRet.r[3].m128_f32[i] = lerp(x.r[3].m128_f32[i], y.r[3].m128_f32[i], ratio);
	*/

	return toRet;
}

DirectX::XMVECTOR RPYFromVector(DirectX::XMVECTOR RPY)
{
	DirectX::XMFLOAT3 angles = DirectX::XMFLOAT3((RPY.m128_f32[0] * 180.0f) + 180.0f, 0.0f, (RPY.m128_f32[2] * 180.0f) + 180.0f);
	DirectX::XMVECTOR toRet = DirectX::XMLoadFloat3(&angles);
	return toRet;
}

DirectX::XMMATRIX TurnTo(DirectX::XMMATRIX _mat, DirectX::XMVECTOR _target, float speed)
{
	DirectX::XMMATRIX toRet = _mat;

	DirectX::XMFLOAT4 _pos;
	DirectX::XMStoreFloat4(&_pos, _mat.r[3]);

	DirectX::XMFLOAT3 pos = { _pos.x, _pos.y, _pos.z };

	DirectX::XMFLOAT3 targVec = DirectX::XMFLOAT3(_target.m128_f32[0] - pos.x, _target.m128_f32[1] - pos.y, _target.m128_f32[2] - pos.z);
	DirectX::XMFLOAT3 _t = { targVec.x, targVec.y, targVec.z };

	DirectX::XMVECTOR
		xAxis = DirectX::XMVector3Normalize(_mat.r[0]),
		yAxis = DirectX::XMVector3Normalize(_mat.r[1]),
		zAxis = DirectX::XMVector3Normalize(_mat.r[2]),
		_targ = DirectX::XMLoadFloat3(&_t);

	float yRotation = DirectX::XMVectorGetX(DirectX::XMVector3Dot(xAxis, _targ));
	float xRotation = DirectX::XMVectorGetX(DirectX::XMVector3Dot(yAxis, _targ));

	DirectX::XMVECTOR thisPos = toRet.r[3];
	toRet.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	toRet = toRet * DirectX::XMMatrixRotationY(yRotation * speed);
	toRet = DirectX::XMMatrixRotationX(xRotation * -speed) * toRet;

	toRet.r[3] = thisPos;
	return toRet;
}

void CleanGlobals()
{
	GW_SAFE_RELEASE(g_pWindow);
	//for (int i = 0; i < v_tMeshTemplates.size(); i++)
	//{
	//	if(v_tMeshTemplates.at(i)._vertexBuffer)
	//	SAFE_RELEASE(v_tMeshTemplates.at(i)._vertexBuffer);
	//	SAFE_RELEASE(v_tMeshTemplates[i]._indexBuffer);
	//	SAFE_RELEASE(v_tMeshTemplates[i]._samState);
	//	for (int j = 0; j < 3; j++)
	//	{
	//		SAFE_RELEASE(v_tMeshTemplates[i]._textures[j]);
	//		SAFE_RELEASE(v_tMeshTemplates[i]._srv[j]);
	//	}
	//
	//}
	v_tMeshTemplates.clear();
	g_d3dData->Cleanup();
	delete g_d3dData;
	g_d3dData = nullptr;

	GW_SAFE_RELEASE(g_pControllerInput);
	GW_SAFE_RELEASE(g_pMusicStream);
	GW_SAFE_RELEASE(g_pSoundPlayer);
	GW_SAFE_RELEASE(g_pInputRecord);
	GW_SAFE_RELEASE(g_pAudioHolder);

	GW_SAFE_RELEASE(g_pLogger);
}


