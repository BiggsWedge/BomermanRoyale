#include "Utilities.h"

using namespace std;
using namespace GW;
using namespace SYSTEM;
using namespace AUDIO;


std::vector<const wchar_t*> diffuseTextures =
{
	L".//Assets//Cube.fbm//Crate.jpg",
	L".//Assets//BattleMage.fbm//PPG_3D_Player_D.png"
};



GLog* g_pLogger = nullptr;
GWindow* g_pWindow = nullptr;
GInput* g_pInputRecord = nullptr;
GAudio* g_pAudioHolder = nullptr;
GMusic* g_pMusicHolder = nullptr;

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


bool InitializeInput()
{

	if (G_SUCCESS(CreateGInput(g_pWindow, sizeof(g_pWindow),&g_pInputRecord)))
	{
		g_pLogger->LogCatergorized("SUCCESS", "Input Manager successfully created.");
		return true;
	}
	else
	{
		g_pLogger->LogCatergorized("FAILURE", "Input Manager unsuccessfully created.");
		return false;
	}
	
}


bool InitializeAudio()
{

	if (G_SUCCESS(CreateGAudio(&g_pAudioHolder)))
	{
		if (G_SUCCESS(g_pAudioHolder->Init(1)))
		{
			g_pLogger->LogCatergorized("SUCCESS", "Audio Manager successfully created.");
			return true;
		}
	}
	else
	{
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



