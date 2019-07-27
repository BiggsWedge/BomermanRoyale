#include "Utilities.h"

using namespace std;
using namespace GW;
using namespace SYSTEM;


std::vector<const wchar_t*> diffuseTextures =
{
	L".//Assets//Cube.fbm//Crate.jpg",
	L".//Assets//BattleMage.fbm//PPG_3D_Player_D.png",
	L".//Assets//Menus.fbm//Menu.png"

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

void LoadMenuScreen(int width, int height, int numbuttons, const char* matFile)
{

	//for (int i = 0; i < 1 + numbuttons; ++i)
	//{
	//	if (i == 0)
	//	{
			int index = v_tMeshTemplates.size();

			TMeshTemplate temp;
			temp.uID = index;

			int numIndices = 6;
			int numVerts = 4;



			temp.v_iIndices.resize(numIndices);
			temp.v_tVertices.resize(numVerts);

			float SSpaceWidth = (float)width * 0.5f;
			float SSpaceHeight = (float)height * 0.5f;
			
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
	//	}
	//	else
	//	{
	//		int index = v_tMeshTemplates.size();
	//
	//		TMeshTemplate temp;
	//		temp.uID = index;
	//
	//		int numIndices = 6;
	//		int numVerts = 4;
	//
	//
	//
	//		temp.v_iIndices.resize(numIndices);
	//		temp.v_tVertices.resize(numVerts);
	//
	//		for (auto& v : temp.v_tVertices)
	//		{
	//			v.fPosition.x = -v.fPosition.x;
	//			v.fNormal.x = -v.fNormal.x;
	//		}
	//
	//		v_tMeshTemplates.push_back(temp);
	//	}
	//
	//}

	
}


void LoadTextures()
{
	for (int i = 0; i < diffuseTextures.size(); ++i)
	{
		if (FAILED(DirectX::CreateWICTextureFromFile(g_d3dData->d3dDevice, diffuseTextures[i], nullptr, &g_d3dData->d3dDiffuseTextures[i])))
			g_pLogger->LogCatergorized("FAILURE", "Failed to load texture");
	}
}



