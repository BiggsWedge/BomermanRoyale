#include "Utilities.h"

using namespace std;
using namespace GW;
using namespace SYSTEM;

// Maximum number of debug lines at one time (i.e: Capacity)
constexpr size_t MAX_LINE_VERTS = 4096;

// CPU-side buffer of debug-line verts
// Copied to the GPU and reset every frame.
size_t line_vert_count = 0;
std::array< TLineVertex, MAX_LINE_VERTS> line_verts;

std::vector<const wchar_t*> diffuseTextures =
{
	L".//Assets//Cube.fbm//Crate.jpg",
	L".//Assets//BattleMage.fbm//PPG_3D_Player_D.png"
};


GLog* g_pLogger = nullptr;
GWindow* g_pWindow = nullptr;
std::vector<TMeshTemplate> v_tMeshTemplates = {};

string GetCurrentDateAndTime()
{
	time_t now = time(0);
	tm lmt;
	localtime_s(&lmt, &now);

	string currDateTime;
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
		g_pLogger->EnableConsoleLogging(true);
		g_pLogger->EnableVerboseLogging(true);
		g_pLogger->LogCatergorized("SUCCESS", "Error Logging system successfully initialized.");
		return true;
	}
	return false;
}

bool InitializeWindow()
{
	if (G_SUCCESS(CreateGWindow(100, 100, 1024, 720, GWindowStyle::WINDOWEDBORDERED, &g_pWindow)))
	{
		g_pLogger->LogCatergorized("SUCCESS", "Window successfully created.");
		return true;
	}
	else
	{
		g_pLogger->LogCatergorized("FAILURE", "Window unsuccessfully created.");
		return false;
	}
}

bool InitializeGlobals()
{
	if (!InitializeLogger())
		return false;
	if (!InitializeWindow())
		return false;
	g_d3dData = new DirectXData();
	if (!g_d3dData->Initialize())
		return false;

	return true;
}

void LoadModel(const char* meshFile, const char* matFile)
{

	int index = v_tMeshTemplates.size();

	TMeshTemplate temp;
	temp.uID = index;

	int numIndices;
	int numVerts;

	fstream file{ meshFile, ios::in | ios::binary };

	if (!file.is_open())
	{
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

	for (auto& v : temp.v_tVertices)
	{
		v.fPosition.x = -v.fPosition.x;
		v.fNormal.x = -v.fNormal.x;
	}

	v_tMeshTemplates.push_back(temp);
}


void LoadTextures()
{
	for (int i = 0; i < diffuseTextures.size(); ++i)
	{
		if (FAILED(DirectX::CreateWICTextureFromFile(g_d3dData->d3dDevice, diffuseTextures[i], nullptr, &g_d3dData->d3dDiffuseTextures[i])))
			g_pLogger->LogCatergorized("FAILURE", "Failed to load texture");
	}
}


void add_line(float3 point_a, float3 point_b, float4 color_a, float4 color_b)
{
	// Add points to debug_verts, increments debug_vert_count
	line_verts[line_vert_count].pos = point_a;
	line_verts[line_vert_count].color = color_a;
	line_vert_count += 1;

	line_verts[line_vert_count].pos = point_b;
	line_verts[line_vert_count].color = color_b;
	line_vert_count += 1;
}

void clear_lines()
{
	// Resets debug_vert_count
	line_vert_count = 0;
}

const TLineVertex* get_line_verts()
{
	// Does just what it says in the name
	return line_verts.data();
}

size_t get_line_vert_count()
{
	// Does just what it says in the name
	return line_vert_count;
}

size_t get_line_vert_capacity()
{
	// Does just what it says in the name
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

void LoadLines() {
	drawAABB(v_tMeshTemplates[0].v_tVertices.at(0).fPosition, v_tMeshTemplates[0].v_tVertices.at(1).fPosition, v_tMeshTemplates[0].v_tVertices.at(2).fPosition, v_tMeshTemplates[0].v_tVertices.at(3).fPosition,
		v_tMeshTemplates[0].v_tVertices.at(5).fPosition, v_tMeshTemplates[0].v_tVertices.at(6).fPosition, v_tMeshTemplates[0].v_tVertices.at(9).fPosition, v_tMeshTemplates[0].v_tVertices.at(10).fPosition, float4{ 0,0,1,1 }, float4{ 0,0,1,1 });

}

TCollider GetCenter(TMeshTemplate _verts) {
	TCollider collider;
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

	return collider;
}

DirectX::XMFLOAT3 GetExtents(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ) {
	float3 extents;

	extents.x = (_maxX - _minX) * 0.5f;
	extents.y = (_maxY - _minY) * 0.5f;
	extents.z = (_maxZ - _minZ) * 0.5f;

	return extents;
}