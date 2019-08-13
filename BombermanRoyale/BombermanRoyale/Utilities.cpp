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

bool InitializeWindow()
{
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

bool InitializeControllerInput()
{

	if (G_SUCCESS(CreateGController(G_XBOX_CONTROLLER, &g_pControllerInput)))
	{
		g_pLogger->LogCatergorized("SUCCESS", "Controller Input Manager successfully created.");
		return true;
	}
	else
	{
		g_pLogger->LogCatergorized("FAILURE", "Controller Input Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeAudio()
{
	if (G_SUCCESS(CreateGAudio(&g_pAudioHolder)))
	{
		if (G_SUCCESS(g_pAudioHolder->Init(2)))
		{

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

	TMeshTemplate temp;
	temp.uID = index;
	temp.sName = loadInfo.name;

	fstream file{ loadInfo.meshFile, ios::in | ios::binary };

	if (!file.is_open())
	{
		std::string fail = "Failed to load " + loadInfo.name;
		g_pLogger->LogCatergorized("FAILURE", fail.c_str());
		return;
	}

	file.read((char*)&numIndices, sizeof(int));
	temp.v_iIndices.resize(numIndices);
	file.read((char*)temp.v_iIndices.data(), sizeof(int) * numIndices);

	file.read((char*)&numVerts, sizeof(int));
	temp.v_tVertices.resize(numVerts);
	file.read((char*)temp.v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);

	file.close();

	for (auto& v : temp.v_tVertices)
	{
		v.fPosition.x = -v.fPosition.x;
		v.fNormal.x = -v.fNormal.x;
	}

	v_tMeshTemplates.push_back(temp);
}

void LoadMenuScreen(int width, int height, int numbuttons, const char* matFile) {
	int index = v_tMeshTemplates.size();
	int numIndices = 6;
	int numVerts = 4;
	float SSpaceWidth = (float)width * 0.5f;
	float SSpaceHeight = (float)height * 0.5f;

	TMeshTemplate temp;

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

	for (int i = 0; i < numbuttons; ++i)
	{
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
}

void LoadTextures()
{
	for (int i = 0; i < (int)diffuseTextures.size(); ++i)
	{
		if (FAILED(DirectX::CreateWICTextureFromFile(g_d3dData->d3dDevice, diffuseTextures[i], nullptr, &g_d3dData->d3dDiffuseTextures[i])))
			g_pLogger->LogCatergorized("FAILURE", "Failed to load texture");
	}
}

void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b)
{
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

