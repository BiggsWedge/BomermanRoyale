#include "Utilities.h"

using namespace std;
using namespace GW;
using namespace SYSTEM;
using namespace AUDIO;

constexpr size_t MAX_LINE_VERTS = 4096;

size_t line_vert_count = 0;
std::array< TLineVertex, MAX_LINE_VERTS> line_verts;

std::vector<const wchar_t*> diffuseTextures = {
	L".//Assets//Cube.fbm//Crate.jpg",
	L".//Assets//BattleMage.fbm//PPG_3D_Player_D.png",
	L".//Assets//Menus.fbm//Menu.png",
	L".//Assets//Menus.fbm//In-Game Barnyard Blast.png",
	L".//Assets//Menus.fbm//Playe Status.png",
	L".//Assets//Menus.fbm//Win_Screen.png",
	L".//Assets//Cube.fbm//red_texture.jpg",
	L".//Assets//Cube.fbm//blue_texture.jpg",
	L".//Assets//Cube.fbm//black_texture.jpg",
	L".//Assets//Cube.fbm//fire.jpg"

};

GLog* g_pLogger = nullptr;
GWindow* g_pWindow = nullptr;
GInput* g_pInputRecord = nullptr;
GAudio* g_pAudioHolder = nullptr;
GMusic* g_pMusicStream = nullptr;
GSound* g_pSoundPlayer = nullptr;

std::vector<TMeshTemplate> v_tMeshTemplates = {};

string GetCurrentDateAndTime() {
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

bool InitializeLogger() {
	string cOutFileName;
	cOutFileName.append("ErrorLog");
	cOutFileName.append(GetCurrentDateAndTime());
	cOutFileName.append(".log");

	if (G_SUCCESS(CreateGLog(cOutFileName.c_str(), &g_pLogger))) {
		g_pLogger->EnableConsoleLogging(true);
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
	} else {
		g_pLogger->LogCatergorized("FAILURE", "Window unsuccessfully created.");
		return false;
	}
}

bool InitializeInput() {
	if (G_SUCCESS(CreateGInput(g_pWindow, sizeof(g_pWindow),&g_pInputRecord))) {
		g_pLogger->LogCatergorized("SUCCESS", "Input Manager successfully created.");
		return true;
	} else {
		g_pLogger->LogCatergorized("FAILURE", "Input Manager unsuccessfully created.");
		return false;
	}
}


bool InitializeAudio() {
	if (G_SUCCESS(CreateGAudio(&g_pAudioHolder))) {
		if (G_SUCCESS(g_pAudioHolder->Init(2))) {
			g_pLogger->LogCatergorized("SUCCESS", "Audio Manager successfully created.");
			return true;
		} else {
			g_pLogger->LogCatergorized("FAILURE", "Audio Manager unsuccessfully created.");
			return false;
		}
	} else {
		g_pLogger->LogCatergorized("FAILURE", "Audio Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeGlobals() {
	if (!InitializeLogger())
		return false;
	if (!InitializeWindow())
		return false;
	if (!InitializeInput())
		return false;
	if (!InitializeAudio())
		return false;
	g_d3dData = new DirectXData();
	if (!g_d3dData->Initialize())
		return false;

	return true;
}

void LoadModel(const char* meshFile, const char* matFile) {
	int index = v_tMeshTemplates.size();
	int numIndices;
	int numVerts;

	TMeshTemplate temp;
	temp.uID = index;

	fstream file{ meshFile, ios::in | ios::binary };

	if (!file.is_open()) {
		// log failure
		return;
	}

	file.read((char*)&numIndices, sizeof(int));
	temp.v_iIndices.resize(numIndices);
	file.read((char*)temp.v_iIndices.data(), sizeof(int) * numIndices);

	file.read((char*)&numVerts, sizeof(int));
	temp.v_tVertices.resize(numVerts);
	file.read((char*)temp.v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);

	file.close();

	for (auto& v : temp.v_tVertices) {
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
	
	temp.v_tVertices.at(0).fPosition = DirectX::XMFLOAT3( -(SSpaceWidth), -(SSpaceHeight), 0.0f);
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
}


void LoadTextures() {
	for (int i = 0; i < diffuseTextures.size(); ++i) {
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

void clear_lines() {
	line_vert_count = 0;
}

const TLineVertex* get_line_verts() {
	return line_verts.data();
}

size_t get_line_vert_count() {
	return line_vert_count;
}

size_t get_line_vert_capacity() {
	return MAX_LINE_VERTS;
}

void drawAABB(float3 point_a, float3 point_b, float3 point_c, float3 point_d, float3 point_e, float3 point_f, float3 point_g, float3 point_h, float4 color1, float4 color2) {
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

float3 XMVector2Float3(DirectX::XMVECTOR vector) {
	float3 point;

	point.x = DirectX::XMVectorGetX(vector);
	point.y = DirectX::XMVectorGetY(vector);
	point.z = DirectX::XMVectorGetZ(vector);

	return point;
}

DirectX::XMVECTOR Float32XMVector(float3 point) {
	DirectX::XMVECTOR vector = { point.x, point.y, point.z, 1.0f };

	return vector;
}

void LoadLines() {
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f / 50.0f, 1.0f / 50.0f, 1.0f / 50.0f);
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	TCollider debugCollider = GetCenter(v_tMeshTemplates[0]);
	scale = DirectX::XMFLOAT3(1,1,1);

	for (size_t i = 0; i < 8; i++) {
		debugCollider.corners[i].x = (debugCollider.corners[i].x * scale.x) - position.x; //*25.0f;
		debugCollider.corners[i].y = (debugCollider.corners[i].y * scale.y) - position.y; //*25.0f;
		debugCollider.corners[i].z = (debugCollider.corners[i].z * scale.z) - position.z; //*25.0f;
	}

	drawAABB(debugCollider.corners[2], debugCollider.corners[0], debugCollider.corners[4], debugCollider.corners[6], debugCollider.corners[5], debugCollider.corners[3], debugCollider.corners[7], debugCollider.corners[1], { 1,0,0,1 }, { 1,0,0,1 });
}

TCollider GetCenter(TMeshTemplate _verts) {
	TCollider collider = TCollider();
	float3 center;
	float minX, maxX, minY, maxY, minZ, maxZ;

	minX = maxX = _verts.v_tVertices.at(0).fPosition.x;
	minY = maxY = _verts.v_tVertices.at(0).fPosition.y;
	minZ = maxZ = _verts.v_tVertices.at(0).fPosition.z;

	for (int i = 0; i < _verts.v_tVertices.size(); i++) {
		if (_verts.v_tVertices.at(i).fPosition.x < minX) {
			minX = _verts.v_tVertices.at(i).fPosition.x;
		} else if (_verts.v_tVertices.at(i).fPosition.x > maxX) {
			maxX = _verts.v_tVertices.at(i).fPosition.x;
		} else if (_verts.v_tVertices.at(i).fPosition.y < minY) {
			minY = _verts.v_tVertices.at(i).fPosition.y;
		} else if (_verts.v_tVertices.at(i).fPosition.y > maxY) {
			maxY = _verts.v_tVertices.at(i).fPosition.y;
		} else if (_verts.v_tVertices.at(i).fPosition.z < minZ) {
			minZ = _verts.v_tVertices.at(i).fPosition.z;
		} else if (_verts.v_tVertices.at(i).fPosition.z > maxZ) {
			maxZ = _verts.v_tVertices.at(i).fPosition.z;
		}
	}

	center.x = (minX + maxX) * 0.5f;
	center.y = (minY + maxY) * 0.5f;
	center.z = (minZ + maxZ) * 0.5f;
	
	collider.center = center;
	collider.extents = GetExtents(minX, maxX, minY, maxY, minZ, maxZ);

	GetCorners(collider.center, collider.extents, collider.corners);

	return collider;
}

DirectX::XMFLOAT3 GetExtents(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ) {
	float3 extents;

	extents.x = (_maxX - _minX) * 0.5f;
	extents.y = (_maxY - _minY) * 0.5f;
	extents.z = (_maxZ - _minZ) * 0.5f;

	return extents;
}

void GetCorners(float3 _center, float3 _extents, float3*& corners) {
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